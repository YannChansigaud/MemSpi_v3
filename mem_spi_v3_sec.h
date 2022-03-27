/* SECTOR library version 1.0
# 
# Writed by Chansigaud Yann
# mem_spi_v3_sec.h - ram library for 23LC512 from MicroShip
# 
*/

#ifndef MEM_SPI_V3_SEC_h
#define MEM_SPI_V3_SEC_h

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
      #endif

#include <SPI.h>
#include "mem_spi_v3_base.h"
#include "mem_spi_v3_objects.h"
#include "mem_spi_v3_define.h"
#include "mem_spi_v3_var.h"


class MEM_SEC{
  public:
    MEM_SEC(){}
    ~MEM_SEC(){}

    void begin( MEM_CHIP *chip ){
      memory = chip;
    }   

    // passe à la position suivante
    void nextPos(){   _pos++;   }

    // le bit n du registre en cours est-il libre ?
    bool isLock(){
      #ifdef SEC_debug
        bool resultat = _reg.isLock( _pos );
        Serial.print( "MEM_SEC : isLock = " );
        Serial.println( resultat ? "true" : "false" );
        return( resultat );
      #else
        return( _reg.isLock( _pos ) );
      #endif
      
    }

    // le registre en cours est-il plein ?
    bool isFull(){
      #ifdef SEC_debug
        bool resultat = _reg.isFull();
        Serial.print( "MEM_SEC : isFull = " );
        Serial.println( resultat ? "true" : "false" );
      return( resultat );
      #else
        return( _reg.isFull() );
      #endif
      
    }
    
    // le registre en cours est-il vide ?
    bool isEmpty(){
      #ifdef SEC_debug
        bool resultat = _reg.isEmpty();
        Serial.print( "MEM_SEC : isEmpty = " );
        Serial.println( resultat ? "true" : "false" );
        return( resultat );
      #else
        return( _reg.isEmpty() );
      #endif
      
    }

    // bloque le bit n du registre en cours
    void lockBit(){
      #ifdef SEC_debug
      Serial.print( "MEM_SEC : lockBit " );
      Serial.println( _pos );
      #endif
      _reg.lock( _pos );
    }

    // libère le bit n du registre en cours
    void freeBit(){
      #ifdef SEC_debug
      Serial.print( "MEM_SEC : freeBit " );
      Serial.println( _pos );
      #endif
      _reg.free( _pos );
    }

    uint8_t setBit( bool lock, uint16_t count ){
      count = min( count, 8 );
      _reg.lock( _pos, count, lock );
      if( count > ( 7 - _pos ) ){
        uint8_t nbBitSet = 7 - _pos;
        _pos = 8 ;
        return( nbBitSet );
      }
      else{
        _pos += count;
        return( count );
      }
    }

    uint8_t freeSpace(){
      uint8_t bitFree  = ~_reg;    // on prends l'inverse du registre, les 1 indiques un secteur libre
      uint8_t bitRead  = 0xFF;
      uint8_t bitCount = 0;
      while( bitRead ){
        bitCount += (bitFree & 0x01);
        bitFree >>= 1;
        bitRead <<= 1;
      }
      return( bitCount << 1 );
    }

    // redéfini l'addresse de registre à 0
    void reset(){
      #ifdef SEC_debug
      Serial.println( "MEM_SEC : reset" );
      #endif
      _add = 0 ;
      _reg = 0 ;
      _pos = 0 ;
    }

    // passe au registre suivant (pensez à MEM_SEC::load())
    void nextReg(){
      _add++;
      #ifdef SEC_debug
      Serial.print( "MEM_SEC : nextReg " );    Serial.println( _add );
      #endif
    }

    // lecture du registre pointé par _add
    void loadReg(){
      #ifdef SEC_debug
      Serial.print( "MEM_SEC : loadReg " );    Serial.println( _add );
      #endif
      load();
    }

    // sauvegarde du registre pointé par _add avec vérification
    void saveReg(){
      #ifdef SEC_debug
      Serial.println( "MEM_SEC : saveReg" );
      #endif
      if( started() ){
        uint8_t nbTry  = 0; 
        uint8_t backup = _reg.get();
        save();
        load();
        while( ( backup != _reg.get() ) && (nbTry < 10) ){
          #ifdef SEC_debug
          Serial.print( "try " );
          Serial.println( nbTry );
          #endif
          nbTry++;
          _reg.set( backup );
          save();
          load();
        }
      }
    }

    void set( uint16_t add ){
      uint16_t a = (   add >> 4            );
      uint16_t p = ( ( add & 0x000F ) >> 1 );
      #ifdef SEC_debug
      Serial.print( "MEM_SEC : set : add = " );
      Serial.print( a );
      Serial.print( " pos = " );
      Serial.println( p );
      #endif
      _add = a ;
      load();
      _pos = p ;
    }
    uint8_t get(){
      return( _reg.get() );
    }

    uint16_t varAdd(){
      uint16_t a = ( _add << 4 ) | ( _pos << 1 );
      #ifdef SEC_debug
      Serial.print( "MEM_SEC : varAdd : add = " );
      Serial.println( a );
      #endif
      return( a );
    }
    uint16_t regAdd(){    return( _add );    }
    uint16_t secPos(){    return( _pos );    }

    bool started(){
      #ifdef SEC_debug
      bool result = memory->chip.started;
      Serial.print( "MEM_SEC : started " );
      Serial.println( result ? "true" : "false" );
      return( result );
      #else
      return( memory->chip.started );
      #endif
    }

  private:
    uint16_t     _add = 0 ;
    MEM_SEC_REG  _reg = MEM_SEC_REG();
    uint16_t     _pos = 0 ;

    MEM_CHIP     *memory;

   
    void load(){
      _reg.set( memory->get8( _add ) );
      _pos = 0 ;
      #if ( defined SEC_debug || defined LOW_debug ) 
      Serial.print( "MEM_SEC : load from " );
      Serial.print( _add );
      Serial.print( " : reg = " );
      _reg.printBIN();
      #endif
    }

    void save(){
      #if ( defined SEC_debug || defined LOW_debug ) 
      Serial.print( "MEM_SEC : save to " );
      Serial.print( _add );
      Serial.print( " : reg = " );
      Serial.println( _reg );
      #endif
      memory->send8( _add, _reg.get() );
    }

};

#endif