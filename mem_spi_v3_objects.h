/* SECTOR library version 1.0
# 
# Writed by Chansigaud Yann
# mem_spi_v3_sec.h - ram library for 23LC512 from MicroShip
# 
*/


#ifndef MEM_SPI_V3_OBJECTS_h
#define MEM_SPI_V3_OBJECTS_h

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>
#include "mem_spi_v3_define.h"
#include "mem_spi_v3_base.h"


// ============================ SECTOR properties

class MEM_SEC_ADD : public MEM_PROP16
{
  public :
    using MEM_PROP16::MEM_PROP16;
    using MEM_PROP16::operator=;
    using MEM_PROP16::operator+=;
    using MEM_PROP16::operator-=;
    using MEM_PROP16::operator++;
    using MEM_PROP16::get;
};

class MEM_SEC_POS : public MEM_PROP8
{
  public :
    using MEM_PROP8::MEM_PROP8;
    using MEM_PROP8::operator=;
    using MEM_PROP8::operator++;
    using MEM_PROP8::get;
};

class MEM_SEC_REG : public MEM_PROP8
{
  public :
    using MEM_PROP8::operator=;
    using MEM_PROP8::operator+=;
    using MEM_PROP8::operator-=;
    using MEM_PROP8::get;

    bool isLock( uint8_t p ){   return( _b & ( 0x01 << p ) );   }
    bool isFull(){              return( _b == 0xFF );           }
    bool isEmpty(){             return( _b == 0x00 );           }
    void lock( uint8_t p ){     _b |=  ( 0x01 << p );           }
    void free( uint8_t p ){     _b &= ~( 0x01 << p );           }
    void lock( uint8_t bitPosition, uint8_t count, bool lock ){         // permet de verrouiller ou libérer count bit du registre
      uint8_t maskBitSet = 0;                                           //   mask des bits affectés
      while( (bitPosition ^ 0x08) && count ){                           //   temps que la position ne dépasse pas 7 et qu'il y a encore des bits à modifier
        maskBitSet |= ( 0x01 << bitPosition );                          //     on ajoute 1 au mask décalé de bitPosition
        bitPosition++;                                                  //     on incrémente la position de 1
        count--;                                                        //     on compte 1 bit de moins à modifier
      }                                                                 // 
      if( lock ){   _b |=  maskBitSet;     }                            //   si on lock, on ajoute le mask par un ou logique
      else{         _b &= ~maskBitSet;     }                            //   si on free, on garde la partie du registre définie par le complémentaire du mask
    }
};


// ============================ VARIABLE addresses

class MEM_VAR_ADD_DEF : public MEM_PROP16
{
  public:
    using MEM_PROP16::operator=;
    using MEM_PROP16::get;
  
};

class MEM_VAR_ADD_COUNT : public MEM_PROP16
{
  public:
    using MEM_PROP16::operator=;
    using MEM_PROP16::get;
  
};

class MEM_VAR_ADD_INDEX : public MEM_PROP16
{
  public:
    using MEM_PROP16::operator=;
    using MEM_PROP16::get;
  
};

// ============================ VARIABLE properties

class MEM_VAR_TYPE : public MEM_PROP16
{
  public:
    using MEM_PROP16::MEM_PROP16;
    using MEM_PROP16::operator=;
};

class MEM_VAR_SIZE : public MEM_PROP16
{
  public:
    using MEM_PROP16::MEM_PROP16;
    using MEM_PROP16::operator=;
    using MEM_PROP16::operator&=;
};

class MEM_VAR_DEF : public MEM_PROP16
{
  public:
    
    using MEM_PROP16::operator=;
    /*
    MEM_VAR_DEF &operator=( MEM_VAR_TYPE type ){      setType( type );                        return( *this );    }
    MEM_VAR_DEF &operator=( MEM_VAR_SIZE size ){      setSize( size );                        return( *this );    }
    */
    void set( uint16_t type, uint16_t size ){         setSize( size );      setType( type );                      }
    void erase(){
      _w = 0;
    }
    void setStd( uint8_t type ){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR_DEF : setStd" );
      #endif
      uint16_t size = ( 0x01 << ( type & RamSPI_size_mask ) );
      setType( type );
      setSize( size );
    }
    void setSize( uint16_t size ){
      _w   &= 0xF000;
      size &= 0x0FFF;
      _w   |= size;
      #ifdef VAR_debug
      Serial.print( "MEM_VAR_DEF : setSize to " );   Serial.println( size );
      Serial.print( "                  def is " );   printBIN();
      #endif
    }
    void setType( uint16_t type ){
      _w &= 0x0FFF;
      type <<= 12;
      _w |= type;
      #ifdef VAR_debug
      Serial.print( "MEM_VAR_DEF : setType to " );   Serial.println( type );
      Serial.print( "                  def is " );   printBIN();
      #endif
    }
    uint16_t getSize(){
      uint16_t size = _w & 0x0FFF;
      #ifdef VAR_debug
      Serial.print( "MEM_VAR_DEF : getSize " );   Serial.println( size );
      Serial.print( "               def is " );   printBIN();
      #endif
      return( size );
    }
    uint8_t  getType(){   return( _w >> 12    );    }
};

class MEM_VAR_COUNT : public MEM_PROP16
{
  public:
    using MEM_PROP16::MEM_PROP16;
    using MEM_PROP16::operator=;
    using MEM_PROP16::set;
};

class MEM_VAR_INDEX : public MEM_PROP16
{
  public:
    using MEM_PROP16::MEM_PROP16;
    using MEM_PROP16::operator=;
    using MEM_PROP16::operator+=;
    using MEM_PROP16::operator-=;
    using MEM_PROP16::operator++;
    using MEM_PROP16::operator--;
};

// ============================ MEMORY properties

typedef struct{
  uint16_t  chipID     : 5,
            memType    : 2,
            started    : 1,
            fastIO     : 1,
            wip        : 1,
            cs         : 6;
 } CHIP_PROP;

// ========================================================================================================
// ==============================================            ==============================================
// ==============================================  MEM_CHIP  ==============================================
// ==============================================            ==============================================
// ========================================================================================================

class MEM_CHIP{
  public:
    MEM_CHIP(){};
    ~MEM_CHIP(){};
    
    CHIP_PROP chip       = { 0 } ;
    uint16_t  mftSize    = 0;
    uint16_t  dataSize   = 0;
    uint16_t  freeSpace  = 0;
    uint16_t  maxAddress = 0;
    uint16_t  dataOffset = 0;

    bool begin(){
      if( chip.cs > 0 ){
        #ifdef MEM_debug
        Serial.print( "config " );
        #endif
        switch( chip.chipID ){
          case MemSpi_23LC512 : {
            #ifdef MEM_debug
            Serial.println( "23LC512 ram" );
            #endif
            _RAM_CONFIG_( chip.cs )
            _MEM_SEND_B( RamSPI_byteMode | RamSPI_sequentialMode )
            _MEM_RELEASE( chip.cs )
            break;
          }
          case MemSpi_25LC256 : {
            #ifdef MEM_debug
            Serial.println( "25LC256 rom" );
            #endif
            romWriteProcess();
            _ROM_CONFIG_( chip.cs )
            _MEM_SEND_B( RomSPI_WPDisabled | RomSPI_noneProtected )
            _MEM_RELEASE( chip.cs )
            getStatusRegister();
            break;
          }
        }
        if( chip.fastIO ){
          Serial.println( "fast IO actived" );
          uint8_t port   = digitalPinToPort(    chip.cs );
          outputPortMask = digitalPinToBitMask( chip.cs );
          outputPortPTR  = portOutputRegister(  port    );
          Serial.print( "portID  = " );   Serial.println( port );
          Serial.print( "bitmask = " );   printBIN( outputPortMask, 8 );
        }
        return( true );
      }
      return( false );
    }

    void formatVAR( uint16_t firstAdd, uint16_t q ){
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP : format VAR " );
      Serial.print( firstAdd );
      Serial.print( " : " );
      Serial.print( q );
      Serial.println( " octet(s)" );
      #endif
      format( firstAdd, q );
    }
    void formatMFT(){
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP : format MFT " );
      Serial.print( mftSize );
      Serial.println( " octet(s)" );
      #endif
      format( 0, mftSize );
    }
    void format(){
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP : format ALL " );
      Serial.print( maxAddress );
      Serial.println( " octet(s)" );
      #endif
      format( 0, maxAddress );
    }

// ============================================================================== //
//                                                                                // 
// ROUTINES unitaire                                                              //
//                                                                                // 
// ============================================================================== //

    void send8( uint16_t add, uint8_t b ){
      romWriteProcess();
      _MEM_WRITE( chip.cs )
      _MEM_ADD( add )
      _MEM_SEND_B( b )
      _MEM_RELEASE( chip.cs )
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP -  send8 " );   Serial.print( add );  Serial.print( " : " );    printBIN( b, 8 );
      #endif
    }

    uint8_t get8( uint16_t add ){
      uint8_t b = 0;
      romReadProcess();
      _MEM_READ( chip.cs )
      _MEM_ADD( add )
      _MEM_GET_B( b )
      _MEM_RELEASE( chip.cs )
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP -   get8 " );   Serial.print( add );  Serial.print( " : " );    printBIN( b, 8 );
      #endif
      return( b );
    }

    void send16( uint16_t add, uint16_t w ){
      romWriteProcess();
      _MEM_WRITE( chip.cs )
      _MEM_ADD( add )
      _MEM_SEND_W( w )
      _MEM_RELEASE( chip.cs )
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP - send16 " );   Serial.print( add );  Serial.print( " : " );    printBIN( w, 16 );
      #endif
    }

    uint16_t get16( uint16_t add ){
      uint16_t w = 0;
      romReadProcess();
      _MEM_READ( chip.cs )
      _MEM_ADD( add )
      _MEM_GET_W( w )
      _MEM_RELEASE( chip.cs )
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP -  get16 " );   Serial.print( add );  Serial.print( " : " );    printBIN( w, 16 );
      #endif
      return( w );
    }


// ============================================================================== //
//                                                                                // 
// ROUTINES d'envoi et réception multiples                                        //
//                                                                                // 
// ============================================================================== //

    void send8( uint16_t firstAdd, uint8_t *data, uint16_t count, bool erase=false ){
      uint8_t tramSize = 0;
      while( count ){
        tramSize = ( count > 32 ) ? 32 : count;
        sendTrameHelper( firstAdd, data, tramSize );
        firstAdd += tramSize;
        count    -= tramSize;
        data     += erase ? 0 : tramSize;
      }
    }
/*
    void send8( uint16_t firstAdd, uint8_t *data, uint16_t count, bool erase=false ){
      while( count-- ){
        send8( firstAdd, (*data) );
        firstAdd++;
        data += erase ? 0 : 1;
      }
    }
*/
    void get8( uint16_t firstAdd, uint8_t *data, uint16_t count, bool erase=false ){
      uint8_t tramSize = 0;
      while( count ){
        tramSize = ( count > 32 ) ? 32 : count;
        getTrameHelper( firstAdd, data, tramSize );
        firstAdd += tramSize;
        count    -= tramSize;
        data     += erase ? 0 : tramSize;
      }
    }
/*    
    void get8( uint16_t firstAdd, uint8_t *data, uint16_t count ){
      uint8_t tramSize = 0;
      while( count-- ){
        (*data) = get8( firstAdd );
        firstAdd++;
        data++;
      }
    }
*/
    void send16( uint16_t firstAdd, uint16_t *data, uint16_t count ){
      send8( firstAdd, (uint8_t*)data, count<<1 );
    }

    void get16( uint16_t firstAdd, uint16_t *data, uint16_t count ){
      get8( firstAdd, (uint8_t*)data, count<<1 );
    }

// ============================================================================== //
//                                                                                // 
// ROUTINES debug                                                                 //
//                                                                                // 
// ============================================================================== //

    void extract( uint16_t add, uint16_t count ){
      romReadProcess();
      uint8_t b = 0;
      uint16_t index = 0;
      _MEM_READ( chip.cs )
      _MEM_ADD( add )
      while( count-- ){
        _MEM_GET_B( b )
        printDEC( index, 4, false );
        Serial.print( " : " );
        printBIN( b, 8, false );
        Serial.print( " (" );
        Serial.print( b );
        Serial.println( ")" );
        index++;
      }
      _MEM_RELEASE( chip.cs )
    }
  
  private:
    volatile uint8_t *outputPortPTR;
             uint8_t  outputPortMask = 0;

    void format( uint16_t firstAdd, uint16_t q ){
      uint32_t eraser[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
      send8( firstAdd, (uint8_t*)eraser, q, true );
    }

// ============================================================================== //
//                                                                                //
// ROUTINES d'envoi et réception de TRAMES (32 octets max)                        //
//                                                                                //
// ============================================================================== //
    
    void sendTrameHelper( uint16_t firstAdd, uint8_t *data, uint8_t count ){
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP - sendTrameHelper " );   Serial.print( firstAdd );  Serial.print( " : " );    Serial.print( count );    Serial.println( " octet(s)" );    
      #endif
      romWriteProcess();
      _MEM_WRITE( chip.cs )
      _MEM_ADD( firstAdd )
      while( count-- ){
        #ifdef MEM_debug
        printBIN( (*data), 8 );
        #endif
        _MEM_SEND_B( *data )
        data++;
      }
      _MEM_RELEASE( chip.cs )
    }

    void getTrameHelper( uint16_t firstAdd, uint8_t *data, uint8_t count ){
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP -  getTrameHelper " );   Serial.print( firstAdd );  Serial.print( " : " );    Serial.print( count );    Serial.println( " octet(s)" );    
      #endif
      romReadProcess();
      _MEM_READ( chip.cs )
      _MEM_ADD( firstAdd )
      while( count-- ){
        _MEM_GET_B( *data )
        #ifdef MEM_debug
        printBIN( (*data), 8 );
        #endif
        data++;
      }
      _MEM_RELEASE( chip.cs )
    }

// ============================================================================== //
//                                                                                //
// ROUTINES de prise en charge des EEPROM avec sécurité WriteLatchEnabler         //
//                                                                                //
// ============================================================================== //

    void romWriteProcess(){
      if( chip.memType == MemSpi_Rom ){
        #ifdef MEM_debug
        Serial.println( "MEM_CHIP : romWriteProcess" );
        #endif
        WIP_Waiting();
        WEL_Send();
        WIP_WEL_Waiting();
        chip.wip = 1;
      }
    }

    void romReadProcess(){
      #ifdef MEM_debug
      Serial.println( "MEM_CHIP : romReadProcess" );
      #endif
      if( chip.memType == MemSpi_Rom ){
        WIP_Waiting();
      }
    }

    void WEL_Send(){
      #ifdef MEM_debug
      Serial.println( "MEM_CHIP : WEL_Send" );
      #endif
      _ROM_WRITELATCH( chip.cs )
      _MEM_RELEASE( chip.cs )
      delay(1);
    }

    void WIP_Waiting(){
      if( chip.wip ){
        #ifdef MEM_debug
        Serial.print( "MEM_CHIP : WIP_Waiting... " );
        #endif
        uint8_t statusRegister = 0;
        do{
          statusRegister = getStatusRegister();
          statusRegister &= RomSPI_WriteInProgressMask;
          delay(1);
        } while( statusRegister & RomSPI_WriteInProgress );
        chip.wip = 0;
        #ifdef MEM_debug
        Serial.println( "ok" );
        #endif
      }
    }

    void WIP_WEL_Waiting(){
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP : WIP_WEL_Waiting... " );
      #endif
      uint8_t statusRegister = 0;
      do{
        statusRegister = getStatusRegister();
        statusRegister &= RomSPI_WEL_WIP_Mask;
        delay(1);
      } while( ((statusRegister ^ RomSPI_WriteLatchEnabled)>>1) & (statusRegister & RomSPI_WriteInProgress) );
      chip.wip = 0;
      #ifdef MEM_debug
      Serial.println( "ok" );
      #endif
    }

// ============================================================================== //
//                                                                                //
// ROUTINES d'acquisition du registre de status                                   //
//                                                                                //
// ============================================================================== //

    uint8_t getStatusRegister(){
      #ifdef MEM_debug
      Serial.print( "MEM_CHIP : getStatusRegister " );
      #endif
      uint8_t statusRegister = 0;
      _ROM_STATUS_( chip.cs )
      _MEM_GET_B( statusRegister )
      _MEM_RELEASE( chip.cs )
      #ifdef MEM_debug
      printBIN( statusRegister, 8 );
      #endif
      return( statusRegister );
    }

// ============================================================================== //
//                                                                                //
// ROUTINES debug                                                                 //
//                                                                                //
// ============================================================================== //

    void printBIN( uint16_t b, uint8_t q, bool ln=true ){
      uint8_t n = q;
      uint16_t mask = ( q==8 ) ? 0x0080 : 0x8000 ;
      while( n-- ){
        Serial.print( ( b & mask) ? '1' : '0' );
        b <<= 1;
        if( (n % 8) == 0 ){
          Serial.print( " " );
        }
      }
      if( ln ){
        Serial.println();
      }
    }

    void printDEC( uint16_t d, uint8_t q, bool ln = true ){
      q -= uint8_t( log10( d + (( d == 0 ) ? 1 : 0) ) );    // "log10(d) + 1" = nombre de chiffre composant d. q = nombre de chiffre à afficher. on calcul donc le nombre de zero à compléter
      while( q-- ){
        Serial.print( '0' );
      }
      Serial.print( d );
      if( ln ){
        Serial.println();
      }
    }


// ============================================================================== //
//                                                                                //
// ROUTINES fast IO                                                               //
//                                                                                //
// ============================================================================== //

    void active( uint8_t cmd ){
      SPI.beginTransaction( SPISettings( _MEM_FREQUENCY_, MSBFIRST, SPI_MODE0 ) );
      if( chip.fastIO ){
        (*outputPortPTR) &= ~(outputPortMask);
        yield();
      }
      else{
        digitalWrite( chip.cs , LOW );
      }
      SPI.transfer( cmd );
    }
    
    void release(){
      if( chip.fastIO ){
        (*outputPortPTR) |= outputPortMask;
      }
      else{
        digitalWrite( chip.cs , HIGH );
        delayMicroseconds(1);
      }
      SPI.endTransaction();
    }

};


#endif

