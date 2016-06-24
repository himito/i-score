#!/bin/bash

cd /image
NPROC=$(nproc)

mkdir gcc-build
(
  cd gcc-build
  ../combined/configure --enable-languages=c,c++,lto --enable-gold --enable-plugins --enable-plugin --disable-multilib --disable-nls --enable-werror=no
  make
  make install -j8
  cp -nrf /usr/local/* /usr/
)
