# Guia de Inicio: nRF9151 Feather + Myriota HyperPulse

Esta guia te llevara paso a paso desde la instalacion del entorno hasta ejecutar tu primer proyecto (Blink LED).

> **IMPORTANTE:** Esta guia fue validada con el proceso de compilacion real. Los comandos y configuraciones han sido probados.

---

## Tabla de Contenidos

1. [Requisitos Previos](#1-requisitos-previos)
2. [Instalacion del Entorno](#2-instalacion-del-entorno)
3. [Configuracion del Proyecto](#3-configuracion-del-proyecto)
4. [Primer Proyecto: Blink LED](#4-primer-proyecto-blink-led)
5. [Compilar y Flashear](#5-compilar-y-flashear)
6. [Siguientes Pasos](#6-siguientes-pasos)

---

## 1. Requisitos Previos

### Hardware
- **nRF9151 Feather** (Circuit Dojo) o **Myriota HyperPulse Dev Kit**
- Cable USB-C (que soporte datos, no solo carga)
- (Opcional) Antena LTE/GPS para proyectos de conectividad

### Software Base
| Software | Version | Descarga |
|----------|---------|----------|
| VS Code | Latest | https://code.visualstudio.com/ |
| Git | Latest | https://git-scm.com/downloads |
| Python | 3.10+ | https://www.python.org/downloads/ |

> **Nota:** Asegurate de marcar "Add Python to PATH" durante la instalacion de Python.

### Verificar Instalacion

```powershell
code --version
git --version
python --version
```

---

## 2. Instalacion del Entorno

### Paso 2.1: Instalar nRF Connect for VS Code

1. Abre VS Code
2. Ve a **Extensions** (Ctrl+Shift+X)
3. Busca **"nRF Connect for VS Code Extension Pack"**
4. Instala el pack completo (incluye multiples extensiones)

### Paso 2.2: Instalar Toolchain y SDK

1. Abre la **Command Palette** (Ctrl+Shift+P)
2. Ejecuta: `nRF Connect: Manage Toolchains`
3. Instala **Toolchain v3.1.1** o superior
4. Espera a que complete la descarga (~2-3 GB)

El toolchain se instalara en: `C:\ncs\toolchains\<hash>\`

### Paso 2.3: Inicializar West Workspace

> **IMPORTANTE:** Usa una ruta corta como `C:\ncs\ws` para evitar errores de Windows por rutas demasiado largas (limite 250 caracteres).

1. Abre la **Command Palette** (Ctrl+Shift+P)
2. Ejecuta: `nRF Connect: West Init`
3. Selecciona **"Custom repository"**
4. Ingresa los siguientes datos:

```
Repository URL: https://github.com/circuitdojo/nrf9160-feather-examples-and-drivers.git
Version: v3.1.x
```

5. Selecciona carpeta de destino: **`C:\ncs\ws`** (ruta corta!)

### Paso 2.4: Ejecutar West Update

**IMPORTANTE:** Debes ejecutar `west update` desde DENTRO del directorio del workspace.

Abre el **nRF Connect Terminal** en VS Code y ejecuta:

```powershell
cd C:\ncs\ws
west update
```

Esto descargara Zephyr RTOS y todas las dependencias (~10-20 minutos).

### Paso 2.5: Instalar probe-rs (Para Flashear)

Abre **PowerShell como Administrador** y ejecuta:

```powershell
powershell -ExecutionPolicy ByPass -c "irm https://github.com/probe-rs/probe-rs/releases/latest/download/probe-rs-tools-installer.ps1 | iex"
```

Agrega al PATH (si no se agrego automaticamente):

```powershell
[Environment]::SetEnvironmentVariable('Path', [Environment]::GetEnvironmentVariable('Path', 'User') + ';C:\Users\<TU_USUARIO>\.cargo\bin', 'User')
```

Verifica la instalacion (reinicia la terminal primero):
```powershell
probe-rs --version
```

---

## 3. Configuracion del Proyecto

### Estructura del Workspace

Despues de `west update`, tendras esta estructura:

```
C:\ncs\ws\
├── .west/
│   └── config              # Configuracion de west
├── bootloader/
├── modules/
├── nrf/                    # Nordic SDK
├── nfed/                   # Circuit Dojo Examples (manifest)
│   ├── boards/
│   │   └── circuitdojo/
│   │       └── feather_nrf9151/   # Definicion del board
│   └── samples/
│       ├── blinky/         # <-- Proyecto Blink
│       ├── tracker/
│       ├── gps/
│       └── ...
├── zephyr/                 # Zephyr RTOS
└── ...
```

### Configurar Variable de Entorno

Antes de compilar, configura `ZEPHYR_BASE` en el **nRF Connect Terminal**:

```powershell
$env:ZEPHYR_BASE = "C:\ncs\ws\zephyr"
```

---

## 4. Primer Proyecto: Blink LED

### Paso 4.1: Navegar al Proyecto Blinky

```powershell
cd C:\ncs\ws\nfed\samples\blinky
```

### Paso 4.2: Entender la Estructura

```
blinky/
├── CMakeLists.txt      # Configuracion de build
├── prj.conf            # Configuracion de Kconfig
└── src/
    └── main.c          # Codigo principal
```

### Paso 4.3: Configuracion Kconfig (prj.conf)

El archivo `prj.conf` debe contener:

```ini
CONFIG_GPIO=y
CONFIG_LED=y
CONFIG_MFD=y
CONFIG_I2C=y
CONFIG_LOG=y
```

### Paso 4.4: Codigo Principal (src/main.c)

El ejemplo de Circuit Dojo usa el LED RGB controlado por el nPM1300 PMIC:

```c
/*
 * Copyright (c) 2024 Circuit Dojo LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

static const struct device *leds = DEVICE_DT_GET(DT_NODELABEL(npm1300_leds));

int main(void)
{
    LOG_INF("Blinky Sample");

    while (1)
    {
        led_on(leds, 2U);      /* LED index 2 = LED azul/verde */
        k_sleep(K_MSEC(500));
        led_off(leds, 2U);
        k_sleep(K_MSEC(500));
    }

    return 0;
}
```

> **Nota:** El nRF9151 Feather usa LEDs controlados por el PMIC nPM1300 via I2C, no GPIOs directos.

---

## 5. Compilar y Flashear

### Paso 5.1: Compilar desde Terminal

Abre el **nRF Connect Terminal** y ejecuta:

```powershell
# Navegar al proyecto
cd C:\ncs\ws\nfed\samples\blinky

# Configurar Zephyr base
$env:ZEPHYR_BASE = "C:\ncs\ws\zephyr"

# Compilar
west build -b circuitdojo_feather_nrf9151 --pristine --no-sysbuild -- "-DBOARD_ROOT=C:\ncs\ws\nfed"
```

**Parametros importantes:**
| Parametro | Descripcion |
|-----------|-------------|
| `-b circuitdojo_feather_nrf9151` | Nombre del board target |
| `--pristine` | Limpia build anterior |
| `--no-sysbuild` | Deshabilita sysbuild (evita conflictos con SDK global) |
| `-DBOARD_ROOT=...` | Ruta a las definiciones del board |

### Paso 5.2: Verificar Compilacion Exitosa

Deberas ver algo como:

```
[169/169] Linking C executable zephyr\zephyr.elf
Memory region         Used Size  Region Size  %age Used
           FLASH:       38116 B         1 MB      3.64%
             RAM:        9728 B        88 KB     10.80%
Generating files from .../build/zephyr/zephyr.elf for board: circuitdojo_feather_nrf9151
```

Los archivos generados estaran en: `build/zephyr/`
- `zephyr.hex` - Firmware en formato HEX
- `zephyr.elf` - Firmware con simbolos de debug
- `zephyr.bin` - Firmware binario

### Paso 5.3: Conectar el Hardware

1. Conecta el **nRF9151 Feather** via USB-C a tu PC
2. Windows deberia reconocer el dispositivo CMSIS-DAP integrado
3. Verifica en **Device Manager** que aparezca el dispositivo

### Paso 5.4: Flashear

**Opcion A - probe-rs (Recomendado):**

```powershell
# Usar ruta completa si probe-rs no esta en PATH del nRF Connect Terminal
C:\Users\<TU_USUARIO>\.cargo\bin\probe-rs.exe download --chip nRF9151_xxAA build/zephyr/zephyr.elf
C:\Users\<TU_USUARIO>\.cargo\bin\probe-rs.exe reset --chip nRF9151_xxAA
```

> **IMPORTANTE:** Usar archivo `.elf`, no `.hex`. El formato hex puede dar error "Unknown file magic".

**Opcion B - west flash (requiere nrfjprog):**

```powershell
west flash
```

> Nota: Requiere instalar nRF Command Line Tools para que `nrfjprog` este disponible.

### Paso 5.5: Verificar

- El **LED RGB** del nRF9151 Feather deberia alternar entre **azul y verde** cada 500ms
- El LED es controlado por el PMIC nPM1300 via I2C (LED index 2)
- Si no parpadea, revisa la conexion USB y los pasos anteriores

---

## 6. Siguientes Pasos

### 6.1 Otros Ejemplos Disponibles

El repositorio de Circuit Dojo (`nfed/samples/`) incluye:

| Ejemplo | Descripcion |
|---------|-------------|
| `blinky` | LED parpadeante basico |
| `gps` | Lectura de GPS integrado |
| `tracker` | GPS tracking application |
| `at_client` | Cliente AT para modem LTE |
| `serial_lte_modem` | Modem serial LTE |
| `battery` | Monitoreo de bateria |
| `accelerometer` | Lectura de acelerometro |
| `deep_sleep` | Modo de bajo consumo |

### 6.2 Para Myriota HyperPulse

Si tienes el **Myriota HyperPulse Dev Kit**, necesitaras:

1. **Contactar a Myriota** para acceso al HyperPulse Firmware Library
2. Obtener la **SIM NTN** incluida en el kit
3. Registrarte en el **Early Access Program** para soporte tecnico

Recursos Myriota:
- Portal de Desarrolladores: https://support.myriota.com/
- FAQ: https://myriota.com/faqs/
- GitHub SDK (para productos UltraLite): https://github.com/Myriota/SDK

### 6.3 Depuracion con VS Code

Para depurar paso a paso:

1. Instala la extension **"Debugger for probe-rs"** en VS Code
2. Crea `.vscode/launch.json`:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "probe-rs-debug",
            "request": "launch",
            "name": "Debug nRF9151",
            "chip": "nRF9151_xxAA",
            "coreConfigs": [
                {
                    "programBinary": "${workspaceFolder}/build/zephyr/zephyr.elf"
                }
            ]
        }
    ]
}
```

3. Presiona **F5** para iniciar depuracion

---

## Troubleshooting

### Error: Path too long (Windows)

**Sintoma:**
```
The object file directory has 291 characters. The maximum is 250.
```

**Solucion:** Mueve el workspace a una ruta mas corta como `C:\ncs\ws`

### Error: "No board named 'xxx' found"

**Solucion:** Verifica el nombre exacto del board:
```powershell
west boards | Select-String "circuitdojo"
```
El nombre correcto es: `circuitdojo_feather_nrf9151`

### Error: "west: unknown command build"

**Causa:** West no encuentra Zephyr en el workspace.

**Solucion:** Ejecuta `west update` desde el directorio del workspace:
```powershell
cd C:\ncs\ws
west update
```

### Error: Missing header files (npm1300.h, etc)

**Causa:** El build usa el SDK global en vez del workspace local.

**Solucion:** Configura `ZEPHYR_BASE`:
```powershell
$env:ZEPHYR_BASE = "C:\ncs\ws\zephyr"
```

### Error: "probe-rs: No probe found"

**Soluciones:**
- Verifica la conexion USB-C
- Prueba otro cable USB-C (algunos solo cargan, no transmiten datos)
- Reinstala drivers CMSIS-DAP
- Verifica en Device Manager que aparezca el dispositivo

### Error: "Unknown file magic" al flashear

**Causa:** Intentaste flashear un archivo `.hex` en vez de `.elf`.

**Solucion:** Usa el archivo `.elf`:
```powershell
probe-rs download --chip nRF9151_xxAA build/zephyr/zephyr.elf
```

### Error: "probe-rs no se reconoce como comando"

**Causa:** probe-rs no esta en el PATH del nRF Connect Terminal.

**Solucion:** Usa la ruta completa:
```powershell
C:\Users\<TU_USUARIO>\.cargo\bin\probe-rs.exe download --chip nRF9151_xxAA build/zephyr/zephyr.elf
```

### Error: undefined reference to `__device_dts_ord_xxx`

**Causa:** Falta configuracion en `prj.conf`.

**Solucion:** Agrega las dependencias necesarias:
```ini
CONFIG_LED=y
CONFIG_MFD=y
CONFIG_I2C=y
```

---

## Comandos Rapidos

### Compilar

```powershell
cd C:\ncs\ws\nfed\samples\blinky
$env:ZEPHYR_BASE = "C:\ncs\ws\zephyr"
west build -b circuitdojo_feather_nrf9151 --pristine --no-sysbuild -- "-DBOARD_ROOT=C:\ncs\ws\nfed"
```

### Flashear

```powershell
C:\Users\<TU_USUARIO>\.cargo\bin\probe-rs.exe download --chip nRF9151_xxAA build/zephyr/zephyr.elf
C:\Users\<TU_USUARIO>\.cargo\bin\probe-rs.exe reset --chip nRF9151_xxAA
```

### Compilar y Flashear (todo junto)

```powershell
cd C:\ncs\ws\nfed\samples\blinky
$env:ZEPHYR_BASE = "C:\ncs\ws\zephyr"
west build -b circuitdojo_feather_nrf9151 --pristine --no-sysbuild -- "-DBOARD_ROOT=C:\ncs\ws\nfed"
C:\Users\<TU_USUARIO>\.cargo\bin\probe-rs.exe download --chip nRF9151_xxAA build/zephyr/zephyr.elf
C:\Users\<TU_USUARIO>\.cargo\bin\probe-rs.exe reset --chip nRF9151_xxAA
```

---

## Referencias

- [Circuit Dojo - nRF9151 Feather Documentation](https://docs.circuitdojo.com/nrf9151-feather/overview.html)
- [Circuit Dojo - Using VS Code](https://docs.circuitdojo.com/nrf9151-feather/using-vscode.html)
- [GitHub - Examples and Drivers](https://github.com/circuitdojo/nrf9160-feather-examples-and-drivers)
- [probe-rs Official Site](https://probe.rs/)
- [Myriota HyperPulse Dev Kit](https://myriota.com/hyperpulse-dev-kit/)
- [Nordic nRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/)
- [Zephyr Project Documentation](https://docs.zephyrproject.org/latest/)

---

## Informacion de Compilacion Validada

Esta guia fue validada con:
- **Toolchain:** nRF Connect SDK Toolchain v3.1.1
- **Zephyr:** v4.0.99-ncs1-1
- **Board:** circuitdojo_feather_nrf9151
- **probe-rs:** v0.30.0
- **Resultado Build:** Exitoso (38116 B FLASH, 9728 B RAM)
- **Resultado Flash:** Exitoso (40 KiB en 2.21s)
- **Resultado LED:** Alterna azul/verde cada 500ms
