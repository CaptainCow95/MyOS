#!/bin/bash

# install necessary packages
sudo apt-get install make nasm gcc g++ libgmp-dev libmpfr-dev libmpc-dev genisoimage grub libgtk2.0-dev libsdl1.2-dev bochs bochs-sdl

_dir="$(pwd)"

# create folder for build
mkdir toolchain-build
# go into the build folder
cd toolchain-build

# download all the source packages we need
wget -N ftp://ftp.gnu.org/gnu/binutils/binutils-2.24.tar.gz
wget -N ftp://ftp.gnu.org/gnu/gcc/gcc-4.9.0/gcc-4.9.0.tar.gz
wget -N http://downloads.sourceforge.net/project/bochs/bochs/2.6.6/bochs-2.6.6.tar.gz

# untar all the source packages
tar -xf binutils-2.24.tar.gz
tar -xf gcc-4.9.0.tar.gz
tar -xf bochs-2.6.6.tar.gz

# build the cross-compiler starting with binutils
export PREFIX="$_dir/compiler"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# build binutils
mkdir build-binutils
# go into binutils build folder
cd build-binutils
../binutils-2.24/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror
make -j 4
sudo make install

# out of binutils build folder
cd ..

# now gcc
mkdir build-gcc
# go into gcc build folder
cd build-gcc
../gcc-4.9.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j 4 all-gcc
sudo make install-gcc

# out of gcc build folder
cd ..

# and finally build bochs
# go into bochs folder
cd bochs-2.6.6
./configure --enable-smp --enable-all-optimizations --enable-x86-64 --enable-pci --enable-vmx --enable-logging --enable-fpu --enable-3dnow --enable-sb16=dummy --enable-cdrom --disable-plugins --disable-docbook --with-sdl
make -j 4

# out of bochs folder
cd ..

# out of build folder
cd ..

# reset ownership of the compiler output folder
sudo chown -R $USER:$USER compiler
