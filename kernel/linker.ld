ENTRY(_entry)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    . = 2M;
    __kernel_start__ = .;
    
    .text BLOCK(4K) : ALIGN(4K)
    {
        __write_protect_start__ = .;
        __text_start__ = .;
        *(.text)
        __text_end__ = .;
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        __rodata_start__ = .;
        *(.rodata)
        __rodata_end__ = .;
    }
    
    __write_protect_end__ = ALIGN(4K);
    
   .data BLOCK(4K) : ALIGN(4K)
    {
        __data_start__ = .;
        *(.data)
        __data_end__ = .;
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        __bss_start__  = .;
        *(.bss)
        __bss_end__ = .;
    }
    
    __kernel_end__ = .;
    
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}