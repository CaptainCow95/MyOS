#!/bin/bash

sudo apt-get install make nasm gcc g++ libgmp-dev libmpfr-dev libmpc-dev qemu genisoimage grub

mkdir toolchain-build
cd toolchain-build

wget -O binutils-2.9.1.tar.gz ftp://ftp.gnu.org/gnu/binutils/binutils-2.24.tar.gz
wget -O gcc-4.9.0.tar.gz ftp://ftp.gnu.org/gnu/gcc/gcc-4.9.0/gcc-4.9.0.tar.gz

tar -xf binutils-2.24.tar.gz
tar -xf gcc-4.9.0.tar.gz

export PREFIX="/usr/local/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir build-binutils
cd build-binutils
../binutils-2.24/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror
make
make install

cd ..
mkdir build-gcc
cd build-gcc
../gcc-4.9.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make install-gcc

cd ..
cd ..
