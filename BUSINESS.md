# Reglas de negocio

Qué hace SimpleDS y con qué reglas. La implementación está en
[ARCHITECTURE.md](ARCHITECTURE.md).

## Alcance

SimpleDS **ejecuta** la derivación de excedentes, pero **no decide** cuándo
derivar. Un controlador externo (Home Assistant, Node-RED, un script…) calcula
el excedente a partir de su inversor o contador y envía por MQTT el porcentaje
de potencia que debe recibir la carga. SimpleDS:

1. Aplica ese porcentaje a la carga (normalmente un termo eléctrico) mediante
   un SSR de paso por cero.
2. Protege el SSR: lo apaga si se sobrecalienta y gobierna su ventilador.
3. Publica su estado, la potencia aplicada y las temperaturas del SSR y del
   termo para que el controlador decida.

## Contrato MQTT

Todos los nombres de topic se configuran en el panel. Por defecto, lo que recibe
SimpleDS cuelga de `simpleds/in/` y lo que publica, de `simpleds/out/`:

| Topic por defecto | Sentido | Contenido | Retenido |
| ----------------- | ------- | --------- | -------- |
| `simpleds/in/power` | Controlador → SimpleDS | Potencia en %: número decimal de `0` a `100` (`0`, `42`, `42.5`) | **No** |
| `simpleds/out/power` | SimpleDS → controlador | Potencia aplicada en %, un decimal (`42.5`) | Sí |
| `simpleds/out/state` | SimpleDS → controlador | `active`, `emergency`, `starting` u `offline` | Sí |
| `simpleds/out/ssr/temperature` | SimpleDS → controlador | °C con un decimal, o `error` | Sí |
| `simpleds/out/tank/temperature` | SimpleDS → controlador | °C con un decimal, o `error` | Sí |
| `simpleds/out/fan` | SimpleDS → controlador | `on` u `off` | Sí |

- **Estado:** `active` = funcionamiento normal; `emergency` = carga cortada por
  protección térmica; `starting` = arrancando (dura ~1 s, rara vez visible);
  `offline` = última voluntad publicada por el broker cuando pierde el
  dispositivo.
- **Publicación:** el estado, la potencia y el ventilador se publican al
  cambiar; las temperaturas, cada intervalo de telemetría. Tras cada
  (re)conexión se publica todo.
- **Potencia aplicada ≠ orden:** `power` refleja lo que realmente recibe la
  carga (0 en emergencia, sin orden u orden caducada).

## Órdenes de potencia

- Una orden vale hasta que llega otra o hasta que **caduca** (ajustable,
  60 s por defecto). El controlador debe **repetir la orden periódicamente**
  (por ejemplo cada 10–30 s) aunque no cambie.
- Al caducar, la carga se apaga hasta la siguiente orden.
- Una orden **inválida** (no numérica, fuera de 0–100, con signo, exponente…)
  apaga la carga.
- Una orden **retenida** se ignora: podría ser antigua y encender el termo tras
  un reinicio. El controlador debe publicar sin la marca de retención.
- Si se pierde la conexión MQTT, la carga se apaga.
- Sin servidor MQTT configurado no hay órdenes, así que la carga no se enciende.

## Protección del SSR

La sonda del disipador del SSR se lee cada 2 s.

| Situación | Efecto |
| --------- | ------ |
| Temperatura ≥ umbral de emergencia (70 °C) | Estado `emergency`: carga apagada, ventilador encendido |
| En emergencia, temperatura ≤ umbral de rearme (55 °C) | Vuelve a `active`; si hay orden vigente, se reanuda |
| 3 lecturas fallidas seguidas (sonda desconectada o averiada) | Estado `emergency` por fallo de sonda |
| La sonda vuelve a leer tras un fallo | Se reanuda solo si la temperatura está ≤ rearme |

Entre ambos umbrales hay histéresis para no alternar continuamente.

## Ventilador

- Se enciende al alcanzar su umbral de encendido (45 °C) y se apaga al bajar al
  de apagado (40 °C).
- En emergencia está siempre encendido, también si la sonda falla.
- El umbral de encendido debe ser menor que el de emergencia, para que el
  ventilador actúe antes de cortar la carga.

## Sonda del termo

- Se lee y publica cada intervalo de telemetría (30 s por defecto, de 5 a
  3600 s).
- Es solo informativa: un fallo publica `error` pero **no** corta la carga. La
  protección del agua corresponde al termostato del propio termo y la decisión
  de calentar, al controlador.

## Ajustes

Todos se modifican en el panel. Valores de fábrica y rangos:

| Ajuste | Por defecto | Rango |
| ------ | ----------- | ----- |
| Emergencia del SSR | 70 °C | 20–100 °C, mayor que el rearme |
| Rearme del SSR | 55 °C | 20–100 °C |
| Encendido del ventilador | 45 °C | 20–100 °C, menor que la emergencia |
| Apagado del ventilador | 40 °C | 20–100 °C, menor que el encendido |
| Intervalo de telemetría | 30 s | 5–3600 s |
| Caducidad de la orden | 60 s | 10–3600 s |
| Servidor MQTT | vacío (MQTT desactivado) | nombre o IP |
| Puerto MQTT | 1883 | 1–65535 |
| Identificador de cliente | `simpleds` | letras, números, `-` y `_` |
| Topics | ver *Contrato MQTT* | sin comodines (`+`, `#`), distintos entre sí |

- Los cambios de protección y tiempos se aplican al instante; los de MQTT
  reinician el dispositivo.

## Acceso al panel

- Sin contraseña por defecto: cualquiera en la red local puede acceder.
- Se puede fijar una contraseña (8–64 caracteres) desde el propio panel; a
  partir de entonces se pide al entrar. También se puede quitar.
- Tras 5 intentos fallidos, el acceso se bloquea 1 minuto.
- Si se olvida, la pulsación larga (5 s) del botón BOOT borra la red WiFi y la
  contraseña del panel.

## Registro de decisiones

| Fecha      | Decisión | Motivo |
| ---------- | -------- | ------ |
| 2026-10-05 | SimpleDS no decide cuándo derivar: ejecuta órdenes de potencia en % recibidas por MQTT | Independencia del inversor y del contador; la lógica vive en el controlador. |
| 2026-10-05 | Un topic de órdenes y cinco de publicación, todos configurables y retenidos salvo el de órdenes | Cada valor en su topic, fácil de consumir en Home Assistant y similares; los nuevos suscriptores reciben el último valor. |
| 2026-10-05 | Las órdenes caducan (60 s por defecto) y se ignoran las retenidas | Si el controlador cae o publica algo antiguo, la carga no queda encendida. |
| 2026-10-05 | Orden inválida o pérdida de MQTT ⇒ carga apagada | Principio de fallo seguro. |
| 2026-10-05 | El estado de emergencia comparte topic con la disponibilidad (`offline` como última voluntad) | Un único topic indica si el derivador está operativo. |
| 2026-10-05 | Emergencia por temperatura del SSR con histéresis (70/55 °C) y por 3 fallos seguidos de su sonda | Protege el SSR sin oscilaciones ni falsos disparos por una lectura aislada. |
| 2026-10-05 | Ventilador con histéresis propia (45/40 °C) y forzado en emergencia | Refrigera antes de llegar al corte y siempre que haya riesgo. |
| 2026-10-05 | La sonda del termo es informativa y no corta la carga | El termostato del termo protege el agua; el controlador decide. |
| 2026-10-05 | Panel sin contraseña por defecto, con contraseña opcional | Puesta en marcha sencilla; protección cuando el usuario la quiera. |
| 2026-10-05 | El botón BOOT también borra la contraseña del panel | Vía de recuperación con acceso físico. |
| 2026-10-05 | Topics por defecto agrupados en `simpleds/in/…` (entrada) y `simpleds/out/…` (salida) | El sentido de cada topic se ve en su nombre y permite suscribirse a toda la salida con `simpleds/out/#`. |
