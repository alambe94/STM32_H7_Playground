/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x2009FFFF;    /* end of SB_SRAM1 : SRAM2+SRAM3 */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x4000;      /* required amount of heap  */
_Min_Stack_Size = 0x8000; /* required amount of stack */

INCLUDE mapping_fwimg.ld
INCLUDE mapping_sbsfu.ld

/* Specific SBSFU definition */
__ICFEDIT_intvec_start__    = 0x08000000;
SE_Binary_region_ROM_Length = __ICFEDIT_SE_Code_region_ROM_end__ - __ICFEDIT_SE_CallGate_region_ROM_start__ + 1;

/* Specify the memory areas */
MEMORY
{
 ISR_VECTOR (rx)               : ORIGIN = __ICFEDIT_intvec_start__, LENGTH = VECTOR_SIZE
 SE_Binary_region_ROM (rx)     : ORIGIN = __ICFEDIT_SE_CallGate_region_ROM_start__, LENGTH = SE_Binary_region_ROM_Length
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(8);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(8);
  } >ISR_VECTOR
  SE_CORE_Bin :
  {
    SE_CORE_Bin_start = .;
    KEEP(*(SE_CORE_Bin));
    SE_CORE_Bin_end = .;
  } > SE_Binary_region_ROM
  .SE_IF_Code : { KEEP(*se_interface_application.o (.text .text*))
                  KEEP(*se_interface_bootloader.o (.text .text*))
                  KEEP(*se_interface_common.o (.text .text*))
                  KEEP(*se_interface_kms.o (.text .text*))
                  KEEP(*t_kms.o (.text .text*))
  } >SE_IF_region_ROM
  .text :
  {
    . = ALIGN(8);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(8);
    _etext = .;        /* define a global symbols at end of code */
  } >SB_ROM_region

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >SB_ROM_region

  .ARM.extab   :
  {
  . = ALIGN(8);
  *(.ARM.extab* .gnu.linkonce.armextab.*)
  . = ALIGN(8);
  } >SB_ROM_region
  .ARM : {
	. = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
	. = ALIGN(8);
  } >SB_ROM_region

  .preinit_array     :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
	. = ALIGN(8);
  } >SB_ROM_region

  .init_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
	. = ALIGN(8);
  } >SB_ROM_region
  .fini_array :
  {
	. = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
	. = ALIGN(8);
  } >SB_ROM_region

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >SB_SRAM1_region AT>SB_ROM_region


  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >SB_SRAM1_region

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >SB_SRAM1_region



  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}


