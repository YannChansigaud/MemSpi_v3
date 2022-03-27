/* VAR library version 1.0
# 
# Writed by Chansigaud Yann
# mem_spi_v3_var.h - ram library for 23LC512 from MicroShip
# 
*/

#ifndef MEM_SPI_V3_VAR_h
#define MEM_SPI_V3_VAR_h

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "mem_spi_v3_define.h"
#include "mem_spi_v3_base.h"
#include "mem_spi_v3_objects.h"


// ======================================== //
//                                          //
//      address classes                     //
//                                          //
// ======================================== //

class MEM_VAR_ADD{
  public:
    uint16_t  baseAddress = 0;
    uint16_t countAddress = 0;
    uint16_t indexAddress = 0;
    uint16_t  dataAddress = 0;

    void erase(){
      baseAddress  = 0;
      countAddress = 0;
      indexAddress = 0;
      dataAddress  = 0;
    }

    void defineAddresses(){
      countAddress = baseAddress + 2;
      indexAddress = baseAddress + 4;
      dataAddress  = baseAddress + 6;
#ifdef VAR_debug
      Serial.println( "MEM_VAR_ADD : defineAddresses" );
      Serial.println( baseAddress  );
      Serial.println( countAddress );
      Serial.println( indexAddress );
      Serial.println( dataAddress  );
#endif
    }

  private:
};

// ======================================== //
//                                          //
//      definition classes                  //
//                                          //
// ======================================== //

class MEM_VAR{
  public:
    MEM_VAR(){}
    ~MEM_VAR(){}

/*
    MEM_VAR &operator=(  MEM_VAR_DEF   def       ){    definition =  def;         return( *this );  }
    MEM_VAR &operator=(  MEM_VAR_TYPE  type      ){    definition =  type;        return( *this );  }
    MEM_VAR &operator=(  MEM_VAR_SIZE  size      ){    definition =  size;        return( *this );  }
    MEM_VAR &operator=(  MEM_VAR_COUNT nbElement ){    count      =  nbElement;   return( *this );  }
    MEM_VAR &operator=(  MEM_VAR_INDEX position  ){    index      =  position;    return( *this );  }
    MEM_VAR &operator+=( MEM_VAR_INDEX position  ){    index      += position;    return( *this );  }
    MEM_VAR &operator-=( MEM_VAR_INDEX position  ){    index      -= position;    return( *this );  }
*/
    void begin( MEM_CHIP *chip ){
      memory = chip;
    }

    void setFirstAdd( uint16_t firstAdd ){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR : setFirstAdd" );
      #endif
      addresses.baseAddress = firstAdd;
      addresses.defineAddresses();
    }

    void setNewStd( uint8_t type, uint16_t c ){
      #ifdef VAR_debug
      Serial.print( "MEM_VAR : setNewStd " );
      #endif
      if( type != RamSPI_userDefined ){
        #ifdef VAR_debug
        Serial.println( "ok" );
        #endif
        definition.setStd( type );
        count = c ;
      }
      #ifdef VAR_debug
      else{
        Serial.println( "WRONG TYPE" );
      }
      #endif
    }

    void setNewRaw( uint16_t size, uint16_t c ){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR : setNewRaw ok" );
      #endif
      definition.set( RamSPI_userDefined, size );
      count = c ;
    }

    uint16_t getSize(){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR : getSize" );
      #endif
      return( definition.getSize() );
    }

    void setSize( uint16_t size ){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR : setSize" );
      #endif
      definition.setSize( size );
    }

    uint16_t overallSize(){
      uint16_t _dataSize = count ? dataSize() + 6 : 0;    // une variable contient au moins 1 élément. Si aucun, alors pas de variable déclarée, donc 0;
      #ifdef VAR_debug
      Serial.print( "MEM_VAR : overallSize = " ); Serial.println( _dataSize );
      #endif
      return( _dataSize );
    }

    uint16_t dataSize(){
      uint16_t s = definition.getSize() * count;
      #ifdef VAR_debug
      Serial.print( "MEM_VAR : dataSize = " ); Serial.println( s );
      #endif
      return( s );
    }
    void focus( uint16_t add ){
      addresses.baseAddress = add;
      load();
    }

    void focus( uint16_t add, uint16_t id ){
      focus( add );
      index = id;
      saveID();
    }

    void saveID(){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR : saveID" );
      #endif
      memory->send16( addresses.indexAddress, index );
    }

    void save(){
      #ifdef VAR_debug
      Serial.print( "MEM_VAR : save " );   Serial.println( addresses.baseAddress );
      #endif
      uint16_t dataTab[3] = { definition.get(), count, index };
      memory->send16( addresses.baseAddress, dataTab, 3 );
    }

    void load(){
      addresses.defineAddresses();
      uint16_t dataTab[3];
      memory->get16( addresses.baseAddress, dataTab, 3 );
      definition = dataTab[0];
      count      = dataTab[1];
      index      = dataTab[2];
      #ifdef VAR_debug
      Serial.print( "MEM_VAR : load " );   Serial.println( addresses.baseAddress );
      Serial.println( definition );
      Serial.println( count );
      Serial.println( index );
      #endif
    }

    void format(){
      uint16_t size = dataSize();
     #ifdef VAR_debug
      Serial.print( "MEM_VAR : format from " );
      Serial.print( addresses.dataAddress );
      Serial.print( " for " );
      Serial.print( size );
      Serial.println( " octets" );
     #endif
      memory->formatVAR( addresses.dataAddress, size );
    }

    void erase(){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR : erase" );
      #endif
      format();
      definition.erase();
      count = 0;
      save();
      addresses.erase();
    }

    uint8_t getType(){
      return( definition.getType() );
    }

    void setRaw( uint8_t *tab, uint16_t q=0 ){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR : setRaw" );
      #endif
      q = ( ( q == 0 ) ? definition.getSize() : q );
      uint16_t dataAdd = getIndexedDataAdd();
      memory->send8( dataAdd, tab, q );
    }

    void getRaw( uint8_t *tab, uint16_t q=0 ){
      #ifdef VAR_debug
      Serial.println( "MEM_VAR : getRaw" );
      #endif
      q = ( ( q == 0 ) ? definition.getSize() : q );
      uint16_t dataAdd = getIndexedDataAdd();
      memory->get8( dataAdd, tab, q );
    }

    void setVal( uint16_t val ){
      uint16_t dataAdd = getIndexedDataAdd();
      memory->send16( dataAdd, val );
      #ifdef VAR_debug
      Serial.print( "MEM_VAR : setVal " );  Serial.print( dataAdd );  Serial.print( " : " );  Serial.println( val );
      #endif
    }
    
    uint16_t getVal(){
      uint16_t dataAdd = getIndexedDataAdd();
      uint16_t val = 0;
      val = memory->get16( dataAdd );
      #ifdef VAR_debug
      Serial.print( "MEM_VAR : getVal " );  Serial.print( dataAdd );  Serial.print( " : " );  Serial.println( val );
      #endif
      return( val );
    }

    void printProperties(){      
      Serial.print( "type  = " );    Serial.print( definition.getType(), BIN );   Serial.print( " at " );    Serial.println( addresses.baseAddress  );
      Serial.print( "size  = " );    Serial.print( definition.getSize(), DEC );   Serial.print( " at " );    Serial.println( addresses.baseAddress  );
      Serial.print( "count = " );    Serial.print( count,                DEC );   Serial.print( " at " );    Serial.println( addresses.countAddress );
      Serial.print( "index = " );    Serial.print( index,                DEC );   Serial.print( " at " );    Serial.println( addresses.indexAddress );
      Serial.print( "data start at " );                                                                      Serial.println( addresses.dataAddress  );
    }

    MEM_VAR_ADD addresses = MEM_VAR_ADD();

  private:
    MEM_CHIP     *memory;
    MEM_VAR_DEF   definition  = MEM_VAR_DEF();
    uint16_t      count = 0 ;
    uint16_t      index = 0 ;


    uint16_t getIndexedDataAdd(){
      uint16_t add = addresses.dataAddress + index * definition.getSize();
      #ifdef VAR_debug
      Serial.println( "compute indexed address : " );
      Serial.print(   "    variable defined at : " );   Serial.println( addresses.baseAddress );
      Serial.print(   "                size is : " );   Serial.println( definition.getSize()  );
      Serial.print(   "            starting at : " );   Serial.println( addresses.dataAddress );
      Serial.print(   "        index is set to : " );   Serial.println( index );
      Serial.print(   "            pointing at : " );   Serial.println( add );
      #endif
      return( add );
    }
};


#endif