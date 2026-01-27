SECTIONS {
  /* Code section */
  .text ALIGN(4K) : {
    *(.text*)
  }

  /* Read-only data (e.g. vtables, const strings) */
  .rodata ALIGN(4K) : {
    *(.rodata*)
  }

  /* Initialized data */
  .data ALIGN(4K) : {
    *(.data*)
  }

  /* Uninitialized data (zeroed at runtime) */
  .bss ALIGN(4K) (NOLOAD) : {
    __bss_start = .;
    *(.bss*)
    __bss_end = .;
  }

  /* Optional: Module metadata */
  .modinfo ALIGN(1) : {
    KEEP(*(.modinfo))
  }

  /* Symbol tables and debugging */
  /DISCARD/ : {
    *(.comment)
  }
}