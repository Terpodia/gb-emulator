# gb-emulator
A Gameboy emulator for Linux

## Prerequisites
- Build-Essential `sudo apt install build-essential`
- SDL2 library `sudo apt install libsdl2-dev`
- SDL2-ttf library `sudo apt install libsdl2-ttf-dev`
- Cmake `sudo apt install cmake`
- Check `sudo apt install check`

## Command Line Build

``` sh
mkdir build
cd build
cmake ..
make
gbemu rom_name.gb
```
