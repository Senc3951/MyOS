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
# Install on Ubuntu
sudo apt install git qemu-system xorriso mtools make nasm gcc binutils
```

## Build Instructions
```bash
git clone https://github.com/Senc3951/MyOS.git
cd MyOS

make build
```

## Run Instructions
``` bash
make run
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
