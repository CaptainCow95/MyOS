#!/bin/bash

sudo apt-get install make nasm gcc g++ libgmp-dev libmpfr-dev libmpc-dev genisoimage grub

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

sudo apt-get install libgtk2.0-dev libsdl1.2-dev

cd toolchain-build

wget -O bochs-2.6.6.tar.gz http://downloads.sourceforge.net/project/bochs/bochs/2.6.6/bochs-2.6.6.tar.gz

tar -xf bochs-2.6.6.tar.gz

cd bochs-2.6.6

./configure --enable-smp --enable-all-optimizations --enable-x86-64 --enable-pci --enable-vmx --enable-logging --enable-fpu --enable-3dnow --enable-sb16=dummy --enable-cdrom --disable-plugins --disable-docbook --with-sdl

make

cd ..
