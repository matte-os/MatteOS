SECTIONS {
    /* Just group sections, let addresses start at 0 relative to section start */
    .module_header : { KEEP(*(.module_header)) }
    .text : { *(.text .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    .data : { *(.data .data.*) }
    .bss : { *(.bss .bss.*) }

    /* Keep the empty ksymtab if you want, or remove it */
    .ksymtab : { KEEP(*(.ksymtab)) }

    /DISCARD/ : { *(.eh_frame) *(.comment) }
}