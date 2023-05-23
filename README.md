# MyOS

A Custom 64 bit OS.

#### TODO List
- [x] Interrupts
- [x] PIT
- [x] PS2 Keyboard
- [x] Float support
- [x] Physical memory manager
- [ ] Virtual memory manager (Paging)
- [x] Heap
- [x] ACPI
- [ ] Framebuffer
- [ ] Filesystem
- [ ] User-Space

## Install & Build
#### Needed Packages
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

### Install
```bash
git clone https://github.com/Senc3951/MyOS.git
cd MyOS

make setup
```

## Run
``` bash
make run
```

## Debug
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
