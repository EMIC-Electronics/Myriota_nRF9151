# Guia de Inicio: nRF9151 Feather + Myriota HyperPulse

Esta guia te llevara paso a paso desde la instalacion del entorno hasta ejecutar tu primer proyecto (Blink LED).

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
- Cable USB-C
- (Opcional) Antena LTE/GPS para proyectos de conectividad

### Software Base
| Software | Version | Descarga |
|----------|---------|----------|
| VS Code | Latest | https://code.visualstudio.com/ |
| Git | Latest | https://git-scm.com/downloads |
| nRF Command Line Tools | Latest | https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools |

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
3. Instala **Toolchain v3.0.1** (o la version mas reciente disponible)
4. Espera a que complete la descarga (~2-3 GB)

### Paso 2.3: Inicializar West con el Repositorio de Circuit Dojo

1. Abre la **Command Palette** (Ctrl+Shift+P)
2. Ejecuta: `nRF Connect: West Init`
3. Selecciona **"Custom repository"**
4. Ingresa los siguientes datos:

```
Repository URL: https://github.com/circuitdojo/nrf9160-feather-examples-and-drivers.git
Version: v3.0.x
```

5. Selecciona una carpeta de destino (ej: `C:\ncs\nrf9151-workspace`)
6. Ejecuta: `nRF Connect: West Update`
   - Esto descargara Zephyr RTOS y todas las dependencias del NCS
   - Puede tomar 10-20 minutos dependiendo de tu conexion

### Paso 2.4: Instalar probe-rs (Para Flashear)

Abre **PowerShell como Administrador** y ejecuta:

```powershell
powershell -ExecutionPolicy ByPass -c "irm https://github.com/probe-rs/probe-rs/releases/latest/download/probe-rs-tools-installer.ps1 | iex"
```

Verifica la instalacion:
```powershell
probe-rs --version
```

---

## 3. Configuracion del Proyecto

### Estructura del Workspace

Despues de `west update`, tendras esta estructura:

```
nrf9151-workspace/
├── .west/
├── bootloader/
├── modules/
├── nrf/
├── nrf9160-feather-examples-and-drivers/   <-- Ejemplos de Circuit Dojo
│   ├── samples/
│   │   ├── blinky/                          <-- Proyecto Blink
│   │   ├── tracker/
│   │   └── ...
│   └── ...
├── zephyr/
└── ...
```

---

## 4. Primer Proyecto: Blink LED

### Paso 4.1: Abrir el Proyecto Blinky

1. En VS Code, ve a **File > Open Folder**
2. Navega a: `nrf9151-workspace/nrf9160-feather-examples-and-drivers/samples/blinky`
3. Abre esa carpeta

### Paso 4.2: Entender el Codigo

El proyecto blinky tiene esta estructura:

```
blinky/
├── CMakeLists.txt      # Configuracion de build
├── prj.conf            # Configuracion de Kconfig
└── src/
    └── main.c          # Codigo principal
```

**src/main.c** (ejemplo tipico):

```c
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* LED en el nRF9151 Feather esta en D7 (P0.03) */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&led)) {
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return 0;
    }

    while (1) {
        gpio_pin_toggle_dt(&led);
        k_msleep(1000);  /* Parpadea cada 1 segundo */
    }

    return 0;
}
```

### Paso 4.3: Configurar Build en VS Code

1. En la barra lateral de **nRF Connect**, haz clic en **"Add Build Configuration"**
2. Configura:
   - **Board**: `circuitdojo/nrf9151-feather/ns`
   - **Build directory**: `build/circuitdojo_feather_nrf9151`
   - **Enable Sysbuild**: Activado
3. Haz clic en **"Build Configuration"**

---

## 5. Compilar y Flashear

### Paso 5.1: Compilar

**Opcion A - VS Code:**
- En la barra lateral de nRF Connect, haz clic en el icono de **Build** (martillo)
- Espera a que termine (primera compilacion puede tomar varios minutos)

**Opcion B - Terminal:**
```bash
cd nrf9151-workspace/nrf9160-feather-examples-and-drivers/samples/blinky
west build -b circuitdojo_feather_nrf9151_ns --pristine
```

### Paso 5.2: Conectar el Hardware

1. Conecta el **nRF9151 Feather** via USB-C a tu PC
2. Windows deberia reconocer el dispositivo CMSIS-DAP integrado
3. Verifica en **Device Manager** que aparezca el dispositivo

### Paso 5.3: Flashear

**Opcion A - probe-rs (Recomendado):**

```bash
cd samples/blinky
probe-rs download --chip nRF9151_xxAA --binary-format hex build/circuitdojo_feather_nrf9151/merged.hex
probe-rs reset --chip nRF9151_xxAA
```

**Opcion B - west flash:**
```bash
west flash
```

### Paso 5.4: Verificar

- El **LED RGB** del nRF9151 Feather deberia empezar a parpadear cada segundo
- Si no parpadea, revisa la conexion USB y los pasos anteriores

---

## 6. Siguientes Pasos

### 6.1 Otros Ejemplos Disponibles

El repositorio de Circuit Dojo incluye mas ejemplos:

| Ejemplo | Descripcion |
|---------|-------------|
| `samples/blinky` | LED parpadeante basico |
| `samples/tracker` | GPS tracking application |
| `samples/at_client` | Cliente AT para modem LTE |
| `samples/serial_lte_modem` | Modem serial LTE |

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
                    "programBinary": "${workspaceFolder}/build/circuitdojo_feather_nrf9151/zephyr/zephyr.elf"
                }
            ]
        }
    ]
}
```

3. Presiona **F5** para iniciar depuracion

---

## Troubleshooting

### Error: "No boards found"
- Asegurate de haber ejecutado `West Update` completamente
- Verifica que el SDK version sea v3.0.1 o superior

### Error: "probe-rs: No probe found"
- Verifica la conexion USB-C
- Instala drivers CMSIS-DAP si Windows no los reconoce
- Prueba otro cable USB-C (algunos solo cargan, no transmiten datos)

### Error de compilacion
- Ejecuta `west build --pristine` para limpiar y recompilar
- Verifica que el board target sea exactamente: `circuitdojo/nrf9151-feather/ns`

---

## Referencias

- [Circuit Dojo - nRF9151 Feather Documentation](https://docs.circuitdojo.com/nrf9151-feather/overview.html)
- [Circuit Dojo - Using VS Code](https://docs.circuitdojo.com/nrf9151-feather/using-vscode.html)
- [GitHub - Examples and Drivers](https://github.com/circuitdojo/nrf9160-feather-examples-and-drivers)
- [probe-rs Official Site](https://probe.rs/)
- [Myriota HyperPulse Dev Kit](https://myriota.com/hyperpulse-dev-kit/)
- [Nordic nRF Connect SDK](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/)
- [Zephyr Project - Blinky Sample](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/samples/basic/blinky/README.html)
