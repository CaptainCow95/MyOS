#!/bin/bash

# install necessary packages
sudo apt-get install make nasm gcc g++ libgmp-dev libmpfr-dev libmpc-dev genisoimage grub libgtk2.0-dev libsdl1.2-dev

# create folder for build
mkdir toolchain-build
cd toolchain-build

# download all the source packages we need
wget -O binutils-2.9.1.tar.gz ftp://ftp.gnu.org/gnu/binutils/binutils-2.24.tar.gz
wget -O gcc-4.9.0.tar.gz ftp://ftp.gnu.org/gnu/gcc/gcc-4.9.0/gcc-4.9.0.tar.gz
wget -O bochs-2.6.6.tar.gz http://downloads.sourceforge.net/project/bochs/bochs/2.6.6/bochs-2.6.6.tar.gz

# untar all the source packages
tar -xf binutils-2.24.tar.gz
tar -xf gcc-4.9.0.tar.gz
tar -xf bochs-2.6.6.tar.gz

# build the cross-compiler starting with binutils
export PREFIX="/usr/local/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir build-binutils
cd build-binutils
../binutils-2.24/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror
make
make install

cd ..

# now gcc
mkdir build-gcc
cd build-gcc
../gcc-4.9.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make install-gcc

cd ..

# and finally build bochs
mkdir bochs-2.6.6
cd bochs-2.6.6
./configure --enable-smp --enable-all-optimizations --enable-x86-64 --enable-pci --enable-vmx --enable-logging --enable-fpu --enable-3dnow --enable-sb16=dummy --enable-cdrom --disable-plugins --disable-docbook --with-sdl
make

cd ..
