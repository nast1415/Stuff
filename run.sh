#!/bin/bash

cd initramfs/simple_elf
make
cd ../..
./make_initramfs.sh initramfs initrd
make clean && make && qemu-system-x86_64 -kernel kernel -m 4G -serial stdio -s -initrd initrd # -d int > log 2> log
