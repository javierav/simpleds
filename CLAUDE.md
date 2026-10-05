# CLAUDE.md

Guía para trabajar en este repositorio: stack, convenciones y decisiones
técnicas de alto nivel. La arquitectura se documenta en
[ARCHITECTURE.md](ARCHITECTURE.md); las reglas de negocio, en `BUSINESS.md`.

## Proyecto

SimpleDS es el firmware de un derivador de excedentes solares para ESP32-U
(ESP32-WROOM-32U). Lee la potencia intercambiada con la red y regula una carga
resistiva para aprovechar el excedente fotovoltaico.

## Comandos

```bash
pio run                     # compilar (entorno por defecto: esp32u)
pio run -t upload           # flashear
pio device monitor          # monitor serie, 115200 baudios
pio test -e native          # tests unitarios de lib/core en el ordenador
pio check                   # análisis estático (cppcheck)
```

## Mantenimiento de la documentación

Obligatorio: cada decisión relevante se refleja en el mismo cambio que la
introduce.

| Tipo de decisión                                        | Dónde              |
| ------------------------------------------------------- | ------------------ |
| Stack, herramientas, convenciones, reglas técnicas      | `CLAUDE.md`        |
| Capas, módulos, interfaces, flujo de datos, concurrencia | `ARCHITECTURE.md`  |
| Reglas y decisiones del modelo de negocio               | `BUSINESS.md`      |
| Uso y visión general                                    | `README.md`        |
| Instalación del entorno de desarrollo                   | `INSTALL.md`       |

- Documentar decisiones de alto nivel, no casos particulares ni detalles de
  implementación que ya se leen en el código.
- Añadir una fila al registro de decisiones del documento afectado.
- Si una decisión deja obsoleta otra, actualizarla o marcarla como sustituida;
  no dejar documentación contradictoria.

## Decisiones técnicas

| Fecha      | Decisión | Motivo |
| ---------- | -------- | ------ |
| 2026-10-05 | **PlatformIO + framework Arduino** (`platformio/espressif32`, versión fijada en `platformio.ini`) | Builds reproducibles, varios entornos, gestión de dependencias y tests unitarios integrados, manteniendo el ecosistema Arduino. |
| 2026-10-05 | **C++17** (`-std=gnu++17`) | Namespaces anidados, `inline constexpr`, `std::optional`; soportado por el toolchain. |
| 2026-10-05 | **Lógica de dominio portable en `lib/core`**, sin `Arduino.h` ni ESP-IDF | Permite testearla en el ordenador (`env:native`) y desacoplarla del hardware. |
| 2026-10-05 | **Tests con Unity** en `env:native` | Framework por defecto de PlatformIO; rápido y sin hardware. |
| 2026-10-05 | **Bucle no bloqueante**: prohibido `delay()` y esperas activas fuera de `setup()` | El control y las comunicaciones deben convivir sin perder ciclos. |
| 2026-10-05 | **Fail-safe**: ante fallo o dato inválido, la salida de potencia pasa a 0 | Se controlan cargas de 230 V; el estado seguro es "apagado". |
| 2026-10-05 | **Configuración en dos niveles**: valores por defecto en compilación (`src/config/`) y ajustes de usuario en NVS (`Preferences`) | Separar valores de fábrica de ajustes persistentes. |
| 2026-10-05 | **Sin secretos en el repositorio**; `src/config/secrets.h` está en `.gitignore` | Evitar fugas de credenciales. |
| 2026-10-05 | **Idiomas**: documentación (`*.md`) en español; código, comentarios, ficheros de configuración y commits en inglés | Convención habitual del ecosistema; documentación en el idioma del equipo. |
| 2026-10-05 | **Conectividad con las librerías del core** (`WiFi`, `WebServer`, `DNSServer`, `Preferences`), sin librerías de terceros como WiFiManager | Sin dependencias externas, control del bucle no bloqueante y reutilización del servidor web para el futuro panel. |
| 2026-10-05 | **Textos de la interfaz web en español** | Idioma de los usuarios; los identificadores y comentarios siguen en inglés. |
| 2026-10-05 | **Frontend sin toolchain**: HTML, CSS y JS (ES2017+) planos en `web/`; un script de PlatformIO los comprime e incrusta | Sin Node ni dependencias; basta con `pio run`. |
| 2026-10-05 | **Versionado SemVer** (`config::kFirmwareVersion`) y **Conventional Commits** | Estándares de la comunidad. |

## Commits

- [Conventional Commits](https://www.conventionalcommits.org/es/), en inglés.
- **Sin atribución a herramientas de IA**: nada de `Co-Authored-By`, firmas,
  enlaces ni menciones a Claude u otros asistentes en commits ni PRs.

## Convenciones de código

### Organización

- Muchos ficheros pequeños mejor que pocos grandes: **una clase por pareja
  `.h`/`.cpp`**, orientativamente < 200 líneas por fichero.
- Un directorio por módulo/capa; ver estructura en `ARCHITECTURE.md`.
- `src/main.cpp` solo delega en `app::App`; no contiene lógica.
- Includes relativos a `src/` (`#include "app/app.h"`), cabecera propia
  primero, luego librerías del sistema/terceros, luego del proyecto.

### Estilo

- Formato con `clang-format` (`.clang-format`, base Google, 100 columnas).
- `#pragma once` en todas las cabeceras.
- Todo el código dentro de `namespace simpleds::<modulo>`.

| Elemento              | Convención         | Ejemplo               |
| --------------------- | ------------------ | --------------------- |
| Ficheros              | `snake_case`       | `power_meter.h`       |
| Tipos / clases        | `PascalCase`       | `PowerMeter`          |
| Funciones / métodos   | `camelCase`        | `readPower()`         |
| Variables             | `camelCase`        | `gridPower`           |
| Miembros privados     | `camelCase_`       | `lastSample_`         |
| Constantes            | `kPascalCase`      | `kSampleIntervalMs`   |
| Interfaces            | `I` + `PascalCase` | `IPowerMeter`         |

### Reglas técnicas

- **Unidades SI explícitas en el nombre** cuando no sean obvias
  (`intervalMs`, `powerW`).
- **Tiempo**: `millis()` con aritmética sin signo (`now - last >= interval`),
  segura ante desbordamiento.
- **Memoria**: sin `new`/`malloc` tras el arranque; evitar `String` de Arduino
  en rutas frecuentes (fragmentación del heap).
- **ISR**: `IRAM_ATTR`, mínimas, sin logging ni asignaciones; datos
  compartidos con `std::atomic` o secciones críticas.
- **Dependencias por inyección**: los módulos reciben interfaces en el
  constructor; nada de singletons ni globales salvo en `main.cpp`.
- **Logging**: macros de arduino-esp32 (`log_e`, `log_w`, `log_i`, `log_d`). El
  nivel visible se fija con `CORE_DEBUG_LEVEL` en `platformio.ini` (por defecto
  `3`, info).
- **Errores**: sin excepciones; devolver estado (`bool`, `enum class`,
  `std::optional`).
- Compilar sin warnings en `src/` (`-Wall -Wextra`).
- No editar `src/generated/`: se regenera en cada compilación a partir de
  `web/`.

## Hardware

- MCU: ESP32-WROOM-32U, placa genérica `esp32dev` en PlatformIO.
- La asignación de pines se centraliza en `src/config/pins.h`.
- Botón BOOT (GPIO0): pulsación larga para olvidar la red WiFi. No debe estar
  pulsado al arrancar, porque entra en modo de descarga.
