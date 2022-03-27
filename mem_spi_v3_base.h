/* BASE library version 1.0
# 
# Writed by Chansigaud Yann
# mem_spi_v3_base.h - ram library for 23LC512 from MicroShip
# 
*/

#ifndef MEM_SPI_V3_BASE_h
#define MEM_SPI_V3_BASE_h

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



class MEM_PROP16
{
  public:
    MEM_PROP16(){};
    MEM_PROP16( uint16_t w ){   _w = w;   };
    ~MEM_PROP16(){};

    MEM_PROP16 &operator=(  MEM_PROP16 w ){        _w =  w.get();     return( *this );  }
    MEM_PROP16 &operator+=( MEM_PROP16 w ){        _w += w.get();     return( *this );  }
    MEM_PROP16 &operator-=( MEM_PROP16 w ){        _w -= w.get();     return( *this );  }
    MEM_PROP16 &operator=(  const uint16_t &w ){   _w =  w ;          return( *this );  }
    MEM_PROP16 &operator+=( const uint16_t &w ){   _w += w ;          return( *this );  }
    MEM_PROP16 &operator-=( const uint16_t &w ){   _w -= w ;          return( *this );  }
    MEM_PROP16 &operator++( int               ){   _w++ ;             return( *this );  }
    MEM_PROP16 &operator--( int               ){   _w-- ;             return( *this );  }
    
    MEM_PROP16 &operator&=( const uint16_t &w ){   _w &= w ;          return( *this );  }
    MEM_PROP16 &operator|=( const uint16_t &w ){   _w |= w ;          return( *this );  }

    operator uint16_t();

    uint16_t get(){                return( _w );     }
    void     set( uint16_t w ){            _w = w;   }

    void printBIN(){
      uint8_t  n = 16;
      uint16_t w = _w;
      while( n-- ){
        Serial.print( (w & 0x8000) ? '1' : '0' );
        w <<= 1;
      }
      Serial.println();
    }
  
  protected:
    uint16_t _w = 0;
};

class MEM_PROP8
{
  public:
    MEM_PROP8(){};
    MEM_PROP8( uint8_t b ){   _b = b;   };
    ~MEM_PROP8(){};

    MEM_PROP8 &operator=(  MEM_PROP8 b ){       _b =  b.get();    return( *this );  }
    MEM_PROP8 &operator+=( MEM_PROP8 b ){       _b += b.get();    return( *this );  }
    MEM_PROP8 &operator-=( MEM_PROP8 b ){       _b -= b.get();    return( *this );  }
    MEM_PROP8 &operator=(  const uint8_t &b ){  _b =  b ;         return( *this );  }
    MEM_PROP8 &operator+=( const uint8_t &b ){  _b += b ;         return( *this );  }
    MEM_PROP8 &operator-=( const uint8_t &b ){  _b -= b ;         return( *this );  }
    MEM_PROP8 &operator++( int b            ){  _b++ ;            return( *this );  }

    operator uint8_t();

    uint8_t get(){              return( _b );     }
    void    set( uint8_t b ){           _b = b;   }

    void printBIN(){
      uint8_t n = 8;
      uint8_t b = _b;
      while( n-- ){
        Serial.print( (b & 0x80) ? '1' : '0' );
        b <<= 1;
      }
      Serial.println();
    }
  
  protected:
    uint8_t _b = 0;
};

#endif