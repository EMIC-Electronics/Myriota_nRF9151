# Ejemplos para nRF9151 Feather / Myriota HyperPulse

Este directorio contiene ejemplos de codigo para el nRF9151 Feather y Myriota HyperPulse Dev Kit.

## Estructura

```
examples/
└── blinky/          # LED parpadeante basico
    ├── CMakeLists.txt
    ├── prj.conf
    └── src/
        └── main.c
```

## Como usar estos ejemplos

### Prerequisitos

1. Tener el entorno de desarrollo configurado (ver `GETTING_STARTED.md`)
2. nRF Connect SDK v3.1.1+ instalado
3. West workspace inicializado

### Compilar un ejemplo

```bash
# Desde el directorio del ejemplo
cd examples/blinky

# Compilar para nRF9151 Feather
west build -b circuitdojo_feather_nrf9151_ns --pristine
```

### Flashear

```bash
# Usando probe-rs
probe-rs download --chip nRF9151_xxAA --binary-format hex build/zephyr/merged.hex
probe-rs reset --chip nRF9151_xxAA

# O usando west
west flash
```

## Notas

- Estos ejemplos estan disenados para usarse dentro del workspace de nRF Connect SDK
- Para copiar un ejemplo a tu workspace, copia la carpeta completa a `nrf9151-workspace/app/`
