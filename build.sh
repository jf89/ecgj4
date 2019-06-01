#!/bin/sh

set -e

LIBS="`sdl2-config --libs`"
CFLAGS="`sdl2-config --cflags` -Wall -ggdb -I src -include prelude.h"

mkdir -p bin
mkdir -p build

glslangValidator assets/tile.vert
glslangValidator assets/tile.frag
glslangValidator assets/font.vert
glslangValidator assets/font.frag

gcc $CFLAGS src/asset_builder.c -o asset_builder $LIBS
./asset_builder
gcc $CFLAGS src/main.c -o bin/main $LIBS
