# MyOS

A Custom 64 bit OS.

## Needed Packages:
* git
* grub
* qemu-system
* xorriso
* mtools
* make
* nasm
* gcc
* ld

```bash
# To install on Ubuntu
sudo apt install git qemu-system xorriso mtools make nasm gcc binutils
```

## Build Instructions
```bash
git clone https://github.com/Senc3951/MyOS.git
cd MyOS

make build  # To compile the OS
make run    # To run the OS
```

## Debug Instructions
```bash
make debug

# On a seperate terminal
gdb
(gdb) target remote localhost:1234
(gdb) symbol-file <Output .Bin file>
(gdb) layout src
(gdb) break kmain
(gdb) c
```
