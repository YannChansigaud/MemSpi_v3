/**********************************************************************
 * 
 * This is the C++ part of the mem_spi_v3_base library.
 * See mem_spi_v3_base.h for a full documentation.
 * 
 *********************************************************************/

#include "mem_spi_v3_base.h"

MEM_PROP16::operator uint16_t(){  return( _w );  }

MEM_PROP8::operator uint8_t(){  return( _b );  }