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
| `src/generated/` | Código generado al compilar (no versionado)                    | —                 |
| `web/`           | Interfaz web (HTML, CSS, JS), un subdirectorio por aplicación (`portal/`) más `common/` | —   |
| `scripts/`       | Scripts de compilación de PlatformIO                           | —                 |
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
            ┌─ hay red guardada ──► Estación: conecta y reintenta cada 15 s;
            │                       panel web en http://simpleds.local y en la IP
arranque ───┤
            └─ no hay red ────────► Aprovisionamiento:
                                     1. escaneo de redes (antes de levantar el AP)
                                     2. AP abierto "SimpleDS-XXXX" (XXXX = fin de la MAC)
                                     3. portal cautivo en http://192.168.4.1
                                     4. el usuario elige red y contraseña
                                     5. conexión de prueba con el AP aún activo
                                        ├─ falla ──► error en el portal; no se guarda nada
                                        └─ conecta ► se persiste en NVS y se reinicia
```

| Módulo                                  | Responsabilidad                                         |
| --------------------------------------- | ------------------------------------------------------- |
| `services/wifi/wifi_manager`            | Elige modo al arrancar; olvidar la red                  |
| `services/wifi/wifi_station`            | Conexión a la red configurada y reintentos              |
| `services/wifi/mdns_responder`          | Anuncio `simpleds.local` y servicio `_http._tcp`        |
| `services/panel/control_panel`          | Servidor web del panel en modo estación                 |
| `services/panel/device_status`          | Lectura y JSON del estado del dispositivo               |
| `services/provisioning/captive_portal`  | DNS comodín, rutas web y API del portal                 |
| `services/provisioning/connection_tester` | Conexión de prueba con las credenciales introducidas  |
| `services/provisioning/portal_json`     | Respuestas JSON de la API del portal                    |
| `services/provisioning/access_point`    | Punto de acceso abierto                                 |
| `services/provisioning/network_scanner` | Escaneo asíncrono de redes cercanas                     |
| `services/web/web_assets`               | Tabla de archivos web incrustados                       |
| `services/web/web_responses`            | Envío de archivos web y respuestas JSON                 |
| `services/storage/wifi_credentials_store` | Persistencia de credenciales en NVS                   |
| `services/system/restart_scheduler`     | Reinicio diferido (deja terminar la respuesta HTTP)     |
| `drivers/input/reset_button`            | Pulsación larga del botón BOOT                          |
| `core/network/wifi_credentials`         | Modelo y validación de credenciales (portable)          |
| `core/network/scanned_network`          | Ordenación, deduplicado y nivel de señal de redes (portable) |
| `core/network/connection_failure`       | Clasificación del motivo de fallo de conexión (portable) |
| `core/text/json_escape`                 | Escapado de cadenas JSON (portable)                     |

- **Portal cautivo:** el DNS responde a cualquier dominio con la IP del
  dispositivo y toda ruta desconocida redirige al formulario, de modo que los
  móviles lo abren solos al conectarse.
- **Cambiar de red:** mantener pulsado el botón BOOT 5 s borra la red guardada
  y reinicia en modo aprovisionamiento. Es la única vía si la contraseña
  guardada deja de ser válida.
- **Prueba antes de guardar:** las credenciales solo se guardan si la conexión
  de prueba funciona (hasta 2 intentos, 30 s como máximo). Al conectar, el AP
  pasa al canal del router y el móvil puede perder la conexión unos segundos;
  la interfaz reintenta la consulta del resultado, y el reinicio se retrasa
  10 s para que pueda leerlo.
- **API del portal:**

  | Ruta                   | Método | Respuesta |
  | ---------------------- | ------ | --------- |
  | `/api/networks`        | GET    | `{scanning, networks: [{ssid, bars, secured}]}` |
  | `/api/wifi`            | POST   | `ssid`, `password` → `202` (prueba iniciada), `400 {error}` (formato), `409 {error: "busy"}` |
  | `/api/wifi/status`     | GET    | `{status: idle\|testing\|connected\|failed, error?}` |

- **Pantalla final del portal:** muestra `http://simpleds.local` y la IP obtenida
  en la prueba, porque `.local` no funciona en todos los Android.
- **mDNS e IPv6:** al conectar se crea una dirección IPv6 de enlace local para
  que mDNS responda también a las consultas AAAA; sin ella macOS tarda ~5 s en
  resolver `simpleds.local`.
- **Escaneo:** con el AP activo la radio alterna canales y un escaneo tarda
  ~12 s, así que el primero se hace de forma síncrona antes de levantar el AP
  y los siguientes son asíncronos.

## Interfaz web

Dos aplicaciones web, nunca activas a la vez:

| Aplicación | Modo | Directorio | API |
| ---------- | ---- | ---------- | --- |
| Portal de aprovisionamiento | AP | `web/portal/` | ver *Conectividad WiFi* |
| Panel de control | Estación | `web/panel/` | `GET /api/status` → `{firmware, hostname, ssid, ip, rssi, bars, uptime, freeHeap}` |

- Los archivos de `web/` se comprimen con gzip y se incrustan en el firmware al
  compilar (`scripts/embed_web_assets.py` → `src/generated/`). Se sirven con
  `Content-Encoding: gzip` en la misma ruta que tienen en `web/`.
- Las páginas son estáticas: los datos llegan por una API JSON. El firmware
  devuelve códigos (`wrong_password`, `ssid_empty`…) y los textos para el
  usuario viven en el JavaScript.
- Sin herramientas de frontend (npm, bundlers): HTML, CSS y JS sin compilar.

## Memoria flash

Tabla de particiones `min_spiffs` (flash de 4 MB):

| Partición  | Tamaño  | Uso |
| ---------- | ------- | --- |
| `nvs`      | 20 KB   | Configuración persistente |
| `app0`/`app1` | 1,9 MB cada una | Firmware, dos ranuras para OTA |
| `spiffs`   | 128 KB  | Sin uso |
| `coredump` | 64 KB   | Volcado tras un fallo |

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
- Actualizaciones OTA (las particiones ya están preparadas).
- Seguridad del portal y del panel (hoy el AP es abierto y el panel no tiene
  autenticación).
- Nombre mDNS configurable desde el panel (hoy fijo: `simpleds`).

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
| 2026-10-05 | Particiones `min_spiffs`: 2 × 1,9 MB para la aplicación | La pila WiFi ocupa ~550 KB fijos; deja margen manteniendo OTA. El sistema de archivos no se usa. |
| 2026-10-05 | Interfaz web en `web/`, incrustada comprimida en el firmware en lugar de en un sistema de archivos | Fuentes ordenadas sin coste en flash; un único binario, sin desajustes entre firmware y archivos en OTA, y el portal siempre disponible. |
| 2026-10-05 | Páginas estáticas + API JSON; textos de usuario solo en el frontend | Permite servir los archivos comprimidos sin plantillas y separa datos de presentación. |
| 2026-10-05 | Las credenciales se prueban antes de guardarlas | Evita dejar el dispositivo con una contraseña errónea y sin portal. |
| 2026-10-05 | Acceso al panel por mDNS (`simpleds.local`) y por IP | `.local` es memorable pero no universal (Android); la IP es la alternativa. |
| 2026-10-05 | Panel de control y portal como servicios separados, cada uno con su servidor web, según el modo | Responsabilidades y rutas independientes; solo uno está activo. |
