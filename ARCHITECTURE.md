# Arquitectura

Visión de la arquitectura del firmware y registro de sus decisiones. Las
convenciones de código y el stack están en [CLAUDE.md](CLAUDE.md).

## Contexto

```
   Red eléctrica ◄──► [ Medida de potencia ] ──► ESP32-U ──► [ Actuador ] ──► Carga resistiva
                                                    ▲
                                                    └── Configuración / telemetría (WiFi)
```

El firmware ejecuta un lazo de control: obtiene la potencia intercambiada con
la red, calcula cuánta potencia debe recibir la carga para acercar ese
intercambio a la consigna y la aplica mediante el actuador.

## Capas

```
┌───────────────────────────────────────────────┐
│ main.cpp            setup()/loop() → App      │
├───────────────────────────────────────────────┤
│ src/app/            composición y ciclo de vida│
├──────────────────────┬────────────────────────┤
│ src/services/        │ src/drivers/           │
│ WiFi, web, MQTT,     │ medida, actuador,      │
│ persistencia, OTA    │ cruce por cero…        │
├──────────────────────┴────────────────────────┤
│ lib/core/           dominio portable          │
│ regulador, supervisor de seguridad, modelos,  │
│ interfaces (IPowerMeter, ILoadOutput…)        │
└───────────────────────────────────────────────┘
```

**Regla de dependencias:** las flechas apuntan hacia abajo. `lib/core` no
conoce a nadie: no incluye `Arduino.h`, ni ESP-IDF, ni nada de `src/`. Los
drivers y servicios implementan las interfaces definidas en `core`; `app` es el
único lugar donde se instancian y conectan las piezas concretas.

| Directorio       | Responsabilidad                                                | Depende de        |
| ---------------- | -------------------------------------------------------------- | ----------------- |
| `lib/core/`      | Lógica de control, modelos de datos, interfaces de hardware    | Nada (C++ estándar) |
| `src/drivers/`   | Implementaciones de las interfaces sobre el hardware real      | `core`, Arduino   |
| `src/services/`  | Conectividad, persistencia, interfaz de usuario, telemetría; un subdirectorio por área (`wifi/`, `provisioning/`, `storage/`, `system/`) | `core`, Arduino   |
| `src/config/`    | Pines, valores por defecto, versión                            | —                 |
| `src/app/`       | Composition root: construye, inyecta y orquesta                | todo lo anterior  |
| `test/`          | Tests unitarios de `core` con dobles de las interfaces         | `core`, Unity     |

## Abstracciones principales

Se definirán en `lib/core` a medida que se implementen:

- **`IPowerMeter`**: entrega la potencia intercambiada con la red. Oculta si
  la medida viene de una pinza amperimétrica, un contador externo, etc.
- **`ILoadOutput`**: aplica un nivel de potencia normalizado `0.0 … 1.0` a la
  carga. Oculta si se usa triac, SSR o un regulador externo.
- **Regulador**: función pura de las lecturas y la consigna; calcula el nivel
  de salida. Sin acceso a hardware ni al reloj del sistema (el tiempo se le
  pasa como parámetro).
- **Supervisor de seguridad**: valida lecturas (frescura, rango) y fuerza la
  salida a 0 ante cualquier anomalía.

## Flujo del lazo de control

```
leer IPowerMeter ──► supervisor valida ──► regulador calcula ──► ILoadOutput aplica
        │                  │ inválido
        │                  └──────────────► ILoadOutput = 0 (fail-safe)
        └── publicar telemetría (servicios, sin bloquear el lazo)
```

## Modelo de concurrencia

- `loop()` actúa como **planificador cooperativo**: `App::update()` llama al
  `update()` de cada módulo, que hace un trabajo corto y vuelve. Cada módulo
  gestiona su propia temporización con `millis()`.
- Lo que requiera precisión temporal (cruce por cero, disparo de triac) va en
  **ISR y temporizadores hardware**, comunicándose con el resto mediante
  variables atómicas.
- Se crearán **tareas FreeRTOS** solo si un módulo no puede ser no bloqueante
  (se documentará aquí cada caso).

## Conectividad WiFi

Al arrancar, `WifiManager` decide el modo según haya o no una red guardada:

```
            ┌─ hay red guardada ──► Estación: conecta y reintenta cada 15 s
arranque ───┤
            └─ no hay red ────────► Aprovisionamiento:
                                     1. escaneo de redes (antes de levantar el AP)
                                     2. AP abierto "SimpleDS-XXXX" (XXXX = fin de la MAC)
                                     3. portal cautivo en http://192.168.4.1
                                     4. el usuario guarda red y contraseña
                                     5. se persisten en NVS y se reinicia
```

| Módulo                                  | Responsabilidad                                         |
| --------------------------------------- | ------------------------------------------------------- |
| `services/wifi/wifi_manager`            | Elige modo al arrancar; olvidar la red                  |
| `services/wifi/wifi_station`            | Conexión a la red configurada y reintentos              |
| `services/provisioning/captive_portal`  | DNS comodín + servidor web del formulario               |
| `services/provisioning/access_point`    | Punto de acceso abierto                                 |
| `services/provisioning/network_scanner` | Escaneo asíncrono de redes cercanas                     |
| `services/provisioning/portal_html`     | Generación del HTML del portal                          |
| `services/storage/wifi_credentials_store` | Persistencia de credenciales en NVS                   |
| `services/system/restart_scheduler`     | Reinicio diferido (deja terminar la respuesta HTTP)     |
| `drivers/input/reset_button`            | Pulsación larga del botón BOOT                          |
| `core/network/wifi_credentials`         | Modelo y validación de credenciales (portable)          |
| `core/network/scanned_network`          | Ordenación, deduplicado y nivel de señal de redes (portable) |

- **Portal cautivo:** el DNS responde a cualquier dominio con la IP del
  dispositivo y toda ruta desconocida redirige al formulario, de modo que los
  móviles lo abren solos al conectarse.
- **Cambiar de red:** mantener pulsado el botón BOOT 5 s borra la red guardada
  y reinicia en modo aprovisionamiento. Es la única vía si la contraseña
  guardada deja de ser válida.
- **Escaneo:** con el AP activo la radio alterna canales y un escaneo tarda
  ~12 s, así que el primero se hace de forma síncrona antes de levantar el AP
  y los siguientes son asíncronos.

## Convenciones del dominio

- **Signo de la potencia de red:** positiva = importación (consumo desde la
  red), negativa = exportación (excedente vertido).
- **Unidades:** vatios (W) en `float`; tiempos en milisegundos (`uint32_t`).
- **Salida de carga:** valor normalizado `0.0 … 1.0`; la traducción a ángulo de
  fase, ráfagas o PWM es responsabilidad del driver.

## Ciclo de vida

```
Boot ──► Init ──► Running ◄──► Fault
                    │            (salida = 0 hasta recuperar lecturas válidas)
```

En `Init` y `Fault` la salida está siempre a 0.

## Decisiones pendientes

- Fuente de medida de la potencia de red.
- Tipo de actuador y estrategia de modulación.
- Interfaz de usuario y telemetría (web, MQTT, Home Assistant…).
- OTA y esquema de particiones (el firmware ya ocupa ~62 % de la partición de
  aplicación por defecto).
- Seguridad del portal y del panel (hoy el AP es abierto y sin autenticación).

## Registro de decisiones

| Fecha      | Decisión | Motivo |
| ---------- | -------- | ------ |
| 2026-10-05 | Arquitectura en capas con dominio portable en `lib/core` | Testear la lógica sin hardware y aislarla de los detalles físicos. |
| 2026-10-05 | Hardware detrás de interfaces (`IPowerMeter`, `ILoadOutput`) | Poder cambiar de sensor o actuador sin tocar el control. |
| 2026-10-05 | `App` como único composition root | Un solo punto donde se decide qué implementación concreta se usa. |
| 2026-10-05 | `loop()` como planificador cooperativo; ISR para lo crítico en tiempo; FreeRTOS solo si se justifica | Simplicidad y determinismo; evitar condiciones de carrera innecesarias. |
| 2026-10-05 | Supervisor de seguridad independiente del regulador | La seguridad no depende de la corrección del algoritmo de control. |
| 2026-10-05 | Potencia de red con signo: + importación, − exportación | Convención única en todo el código. |
| 2026-10-05 | Sin red guardada, aprovisionamiento por AP abierto + portal cautivo; con red guardada, solo modo estación con reintentos | Configuración sin app ni cable; el AP no aparece mientras haya una red configurada. |
| 2026-10-05 | Credenciales en un namespace NVS propio (`wifi`) y `WiFi.persistent(false)` | Una única fuente de verdad, independiente de la copia interna del core. |
| 2026-10-05 | Pulsación larga (5 s) del botón BOOT para olvidar la red | Vía de recuperación si la red o la contraseña cambian. |
| 2026-10-05 | Reinicio tras guardar la configuración | Arranque limpio en modo estación en lugar de transiciones de modo en caliente. |
