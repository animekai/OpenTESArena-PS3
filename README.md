# OpenTESArena-PS3

Proyecto experimental para llevar **[OpenTESArena](https://github.com/afritz1/OpenTESArena)**, un motor de código abierto para *The Elder Scrolls: Arena* (1994), a la **PlayStation 3** usando el SDK homebrew [PSL1GHT](https://github.com/ps3dev/PSL1GHT).

> **Estado: etapa inicial.** Todavía no se puede jugar. Por ahora se está comprobando, pieza por pieza, qué partes del motor funcionan en la arquitectura de la PS3.

## Objetivo

Que Arena corra de forma nativa en una PS3 con firmware personalizado, usando el motor de OpenTESArena (renderer por software + SDL2).

## Avance

| Etapa | Descripción | Estado |
|---|---|---|
| 0 | Toolchain PSL1GHT en WSL, "Hello World" y gráficos de prueba en el emulador RPCS3 | Hecho |
| 1 | Cargar con las clases del motor (VFS, BSA, IMG, COL) la pantalla de título de Arena y mostrarla con SDL2 | En curso |
| 2 | Renderer por software del motor sobre SDL2 en PS3 | Pendiente |
| 3 | Sustituir Jolt Physics por un sistema de colisiones simple | Pendiente |
| 4 | Entrada con mando, audio y menús | Pendiente |
| 5 | Empaquetado (.pkg) y pruebas en PS3 real | Pendiente |

### Lo aprendido hasta ahora

- Los datos de Arena se leen con helpers little-endian (`Bytes::getLE*`), así que el *byte-swap* para la PPU (big-endian) casi no requiere parches.
- El C++20 del motor compila con `ppu-g++ 13.2` usando `-std=gnu++20 -D_GNU_SOURCE`.
- El `libstdc++` del toolchain se compila sin hilos (`std::thread` y `std::mutex` no funcionan), y `std::snprintf` y varias funciones POSIX no existen: ver `ps3/compat/`.
- Referenciar `std::cerr` o `std::cout` arrastra el inicializador de `iostream`, que bloquea el arranque en este entorno. `ps3/compat/Debug.cpp` es una copia de `Debug.cpp` sin `iostream`.
- En RPCS3 hay que arrancar el `.fake.self`; el `.elf` crudo falla.

## Estructura
ps3/
source/ programa de prueba (main.cpp)
compat/ shims y parches para PSL1GHT/newlib
Makefile basado en ppu_rules del SDK
juego-original/ clon de OpenTESArena (no incluido en este repositorio)


## Compilar

Requisitos: Linux o WSL con [ps3toolchain](https://github.com/ps3dev/ps3toolchain), PSL1GHT y SDL2 para PS3 (portlibs).

```bash
git clone https://github.com/afritz1/OpenTESArena.git juego-original
export PS3DEV=/usr/local/ps3dev
export PSL1GHT=$PS3DEV
cd ps3
make
```

Resultado: `etapa1.fake.self`, que se puede abrir en RPCS3 (*File → Boot SELF/ELF*).

## Datos del juego

**Este repositorio no incluye ningún archivo de Arena.** Necesitas tu propia copia del juego (por ejemplo, la versión de GOG) y copiarla a `/dev_hdd0/ARENA` en la PS3 (o en la carpeta `dev_hdd0` de RPCS3).

## Créditos

- [OpenTESArena](https://github.com/afritz1/OpenTESArena) por Aaron Fritz y colaboradores (licencia MIT).
- [PSL1GHT](https://github.com/ps3dev/PSL1GHT) y [ps3toolchain](https://github.com/ps3dev/ps3toolchain).
- Este proyecto cuenta con el apoyo de **Claude** (Anthropic) como asistente de desarrollo para el análisis, la depuración y la escritura de código.

*The Elder Scrolls* es marca de Bethesda Softworks. Este es un proyecto de aficionados, sin relación con Bethesda.
