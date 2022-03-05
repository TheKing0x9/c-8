#!/bin/bash

cflags="-Wall -Wno-unused-result -O3 -g -std=gnu11 -fno-strict-aliasing -Isrc"
lflags="-lraylib -lm -lGL -lpthread -ldl -lrt -lX11"

platform="unix"
outfile="chip8"
compiler="gcc"
lflags="$lflags -o $outfile"

if command -v ccache >/dev/null; then
  compiler="ccache $compiler"
fi

echo "compiling ($platform)..."
for f in `find src -name "*.c"`; do
  $compiler -c $cflags $f -o "${f//\//_}.o"
  if [[ $? -ne 0 ]]; then
    got_error=true
  fi
done

if [[ ! $got_error ]]; then
  echo "linking..."
  $compiler *.o $lflags
fi

echo "cleaning up..."
rm *.o
echo "done"
