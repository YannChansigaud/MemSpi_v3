# MemSpi_v3
Third release of ram/rom manager with automatic addressing

Memory space is split in two area : MFT and DATA

CHIP_PROP struct
regroup memory chip settings

MEM_CHIP class
class handling communication process with SPI chip and memory space setting

MEM_MFT class
working in MFT area
class handling function to manage mft area
get an instanciation of MEM_SEC class
get pointer to MEM_CHIP instanciation
get pointer to MEM_VAR  instanciation

MEM_SEC class
working in MFT area
class handling function to lock, release and looking for free bit.

MEM_VAR class
working in DATA area
class handling function and declaration of variable in data space

MEM_DEF class
get an instanciation of MEM_CHIP class
get an instanciation of MEM_VAR  class
get an instanciation of MEM_MFT  class
send MEM_CHIP address object to MEM_MFT and MEM_VAR instanciation to allow MEM_MFT and MEM_VAR to access to MEM_CHIP definition
send MEM_VAR  address object to MEM_MFT instanciation to allow MEM_MFT function to set MEM_VAR definition
