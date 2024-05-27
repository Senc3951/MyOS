# MyOS
x86 Operating System.

## Building
> [!NOTE]
> I do all development on Ubuntu 22.04, therefore, other Linux environments should work but the project was never tested on Mac / Windows.

### Installation
Install `grub`, `gcc`, `nasm`, `ld`, `makefile`, `mtools` and `qemu`.

### Setup
Enter the directory and run ```make setup```

### Running
run `make`.

### Debugging
First, make sure that debugging is enabled in config.mk, if it wasn't, enable it and clear the project by running ```make clean```.
Run ```make debug```.

On a seperate terminal, run the following commands
```bash
gdb
target remote localhost:1234
symbol-file output/kernel.bin
```

## Contribute
This project is open source, feel free to study and contribute.