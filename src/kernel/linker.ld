ENTRY(_start)

SECTIONS
{
	. = 1M;
	_kernelStart = .;
    
	.multiboot2 :
	{
		*(.multiboot2.header)
		*(.multiboot2.gdt64)
		*(.multiboot2.text)
		*(.multiboot2.bss)
	}
	
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}
	
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
	
	.kernelEnd = .;
	
	/DISCARD/ : { *(.comment) }
}