# Instalación del entorno de desarrollo

Guía para preparar el entorno en **macOS** y **Linux**. El proyecto se compila
con [PlatformIO Core](https://docs.platformio.org/page/core/index.html), una
herramienta de línea de comandos que proporciona el comando `pio`.

## macOS

### 1. Herramientas de desarrollo y Homebrew

```bash
xcode-select --install
```

Instala `clang`, `make` y `git`, necesarios para compilar los tests en el
ordenador. Si no tienes [Homebrew](https://brew.sh/), instálalo siguiendo las
instrucciones de su web.

### 2. PlatformIO

```bash
brew install platformio clang-format
```

`clang-format` formatea el código C++ según `.clang-format`.

Verifica:

```bash
pio --version
```

### 3. Puerto USB

Las placas con chip **CP210x** (Silicon Labs) y **CH340/CH9102** (WCH)
funcionan sin drivers adicionales en las versiones recientes de macOS.
Conecta la placa y comprueba que aparece un puerto del tipo
`/dev/cu.usbserial-*` o `/dev/cu.SLAB_USBtoUART`:

```bash
pio device list
```

Si solo aparecen `/dev/cu.debug-console` y `/dev/cu.Bluetooth-Incoming-Port`,
comprueba si macOS ve la placa a nivel USB:

```bash
system_profiler SPUSBDataType | grep -iE "cp210|ch34|ch910|uart|serial|espressif"
```

- **No aparece nada:** el problema es físico, no de drivers. Lo más habitual
  es un cable USB que solo carga (sin líneas de datos); prueba otro cable y
  conecta directamente al Mac, sin hubs ni monitores.
- **Aparece pero no hay puerto `/dev/cu.*`:** instala el driver del fabricante
  del chip USB-serie (Silicon Labs para CP210x, WCH para CH340/CH9102).

## Linux

Ejemplos para Debian/Ubuntu; en otras distribuciones usa los paquetes
equivalentes.

### 1. Requisitos previos

```bash
sudo apt update
sudo apt install -y python3 python3-venv build-essential git curl clang-format
```

### 2. PlatformIO

Instálalo con el instalador oficial, que crea un entorno aislado en
`~/.platformio/penv`:

```bash
curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
python3 get-platformio.py
rm get-platformio.py
```

Ese directorio no está en el `PATH`; enlaza los ejecutables en `~/.local/bin`:

```bash
mkdir -p ~/.local/bin
ln -sf ~/.platformio/penv/bin/platformio ~/.local/bin/platformio
ln -sf ~/.platformio/penv/bin/pio ~/.local/bin/pio
ln -sf ~/.platformio/penv/bin/piodebuggdb ~/.local/bin/piodebuggdb
```

Si `~/.local/bin` no está en tu `PATH`, añade esta línea a `~/.bashrc` (o
`~/.zshrc`) y abre una terminal nueva:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Verifica:

```bash
pio --version
```

### 3. Puerto USB

1. Instala las reglas `udev` de PlatformIO:

```bash
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

2. Añade tu usuario al grupo con acceso a puertos serie (`dialout` en
   Debian/Ubuntu, `uucp` en Arch):

```bash
sudo usermod -aG dialout $USER
```

3. Cierra sesión y vuelve a entrar para que el cambio de grupo tenga efecto.
4. Conecta la placa y comprueba que aparece `/dev/ttyUSB0` o `/dev/ttyACM0`:

```bash
pio device list
```

> En Ubuntu, si la placa se conecta y desconecta enseguida, desinstala
> `brltty`, que captura los conversores CH340: `sudo apt remove brltty`.

## Verificar la instalación

Desde la raíz del repositorio:

```bash
pio run
```

La primera compilación descarga la plataforma ESP32 y el toolchain (varios
cientos de MB), por lo que tarda unos minutos. Después:

```bash
pio run -t upload
pio device monitor
```

Si la subida no arranca sola, mantén pulsado el botón **BOOT** de la placa
mientras empieza el flasheo.

## Formatear el código

Antes de hacer commit, desde la raíz del repositorio:

```bash
clang-format -i $(git ls-files '*.cpp' '*.h' | grep -v generated)
```
