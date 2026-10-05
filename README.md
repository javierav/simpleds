# SimpleDS

Firmware para un **derivador de excedentes solares** basado en un **ESP32-U**
(ESP32-WROOM-32U, con antena externa U.FL), escrito en C++ sobre el framework
Arduino y compilado con PlatformIO.

Un derivador desvía el excedente fotovoltaico a una carga resistiva (termo
eléctrico, radiador…) en lugar de verterlo a la red. SimpleDS es un derivador
**simple**: no se conecta al inversor ni decide cuándo derivar. Un controlador
externo (Home Assistant, Node-RED…) le envía por MQTT el porcentaje de potencia
y SimpleDS lo aplica con un SSR de paso por cero.

> **Estado:** en desarrollo, pendiente de validar en hardware real.

## Funciones

- Control de potencia por ciclos completos de red (0–100 %) sobre un SSR de
  paso por cero.
- Protección térmica del SSR con sonda DS18B20 y modo de emergencia.
- Ventilador de refrigeración automático.
- Sonda DS18B20 en el termo, publicada periódicamente por MQTT.
- Configuración WiFi por portal cautivo y panel web en `http://simpleds.local`
  con contraseña opcional.

Contrato MQTT y reglas de funcionamiento en [BUSINESS.md](BUSINESS.md);
esquema de conexiones en [ARCHITECTURE.md](ARCHITECTURE.md#montaje-del-hardware).

## ⚠️ Aviso de seguridad

Este proyecto controla cargas conectadas a la **tensión de red (230 V AC)**.
Una instalación incorrecta puede causar electrocución o incendio. Móntalo solo
si sabes lo que haces, respeta la normativa eléctrica local y usa protecciones
adecuadas. El software se distribuye sin garantía (ver [LICENSE](LICENSE)).

## Requisitos

- Placa con ESP32-WROOM-32U (o compatible ESP32 clásico).
- [PlatformIO Core](https://platformio.org/) (comando `pio`).
- Cable USB y, si la placa lo necesita, driver del conversor USB-serie
  (CP210x / CH340).

Instrucciones detalladas para macOS y Linux en [INSTALL.md](INSTALL.md).

## Uso rápido

```bash
pio run                     # compilar el firmware
pio run -t upload           # compilar y flashear
pio device monitor          # monitor serie (115200 baudios)
pio test -e native          # tests unitarios en el ordenador
```

## Estructura del proyecto

```
├── src/          Código dependiente de Arduino/ESP32 (app, drivers, servicios)
├── lib/core/     Lógica de dominio portable, sin dependencias de hardware
├── web/          Interfaz web (se incrusta en el firmware al compilar)
├── docs/         Diagramas (esquema de conexiones)
├── scripts/      Scripts de compilación
├── test/         Tests unitarios (Unity) que se ejecutan en el ordenador
└── platformio.ini
```

El detalle está en [ARCHITECTURE.md](ARCHITECTURE.md).

## Documentación

| Documento                            | Contenido                                        |
| ------------------------------------ | ------------------------------------------------ |
| [INSTALL.md](INSTALL.md)             | Preparación del entorno en macOS y Linux          |
| [ARCHITECTURE.md](ARCHITECTURE.md)   | Arquitectura, módulos, montaje del hardware y decisiones de diseño |
| [CLAUDE.md](CLAUDE.md)               | Convenciones, stack y decisiones técnicas         |
| [BUSINESS.md](BUSINESS.md)           | Contrato MQTT, reglas de funcionamiento y ajustes |

## Contribuir

1. Formatea con `clang-format` (configuración en `.clang-format`).
2. Usa [Conventional Commits](https://www.conventionalcommits.org/es/).
3. Añade tests para cualquier lógica nueva en `lib/core`.
4. Si tomas una decisión técnica o de arquitectura relevante, actualiza la
   documentación correspondiente en el mismo cambio.

## Licencia

[MIT](LICENSE) © 2026 Javier Aranda
