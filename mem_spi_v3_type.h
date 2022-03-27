/* RAM typedef library version 3.0
# 
# Writed by Chansigaud Yann
# mem_spi_v3_type.h - register library
# 
*/



#ifndef mem_vars_h
#define mem_vars_h


#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "mem_spi_v3_memory.h"

class mem_vars
{
  public:
    mem_vars( MEM_DEF *mem, uint8_t type, uint16_t q ){
      externalMemory = mem;
      alloc( type, q );
    }
    ~mem_vars(){
      del();
    }

// ==========================================================================
    ram_vars &operator[]( const uint16_t index ){
      if( add ){
        focus( index );
      }
      return( *this );
    }

    void del(){
      focus();
      externalMemory->del();
    }

  protected :
    uint16_t add=0;

    MEM_DEF *externalMemory;

    void focus(){
      externalMemory->focus( add );
    }
    void focus( uint16_t index ){
      externalMemory->focus( add, index );
    }
    bool alloc( uint8_t typeID, uint16_t count=1 ){
      if( add == 0 ){
        add = externalMemory->newVar( typeID, count );
        return( add > externalMemory->getMftSize() );
      }
      return( false );
    }
    bool allocRaw( uint16_t size, uint16_t count ){
      if( add == 0 ){
        add = externalMemory->newRaw( count, size );
        return( add > externalMemory->getMftSize() );
      }
      return( false );
    }
    void raw( uint16_t *tab, uint16_t q, uint8_t readWrite ){
      switch( readWrite ){
        case RamSPI_read  :{
          externalMemory->getVal( tab, q );
          break;
        }
        case RamSPI_write :{
          externalMemory->setVal( tab, q );
          break;
        }
      }
    }
};


#endif