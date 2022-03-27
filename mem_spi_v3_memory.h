// file structure :
// 
//  type  :     4 bits
//  size  :    12 bits (means size per element)
//  count :    16 bits (how many element, max 65535)
//  index :    16 bits (element pointed)
//  data  :  n*16 bits


#ifndef MEM_SPI_V3_MEMORY_h
#define MEM_SPI_V3_MEMORY_h

#include "mem_spi_v3_define.h"
#include "mem_spi_v3_objects.h"
#include "mem_spi_v3_mft.h"
#include "mem_spi_v3_var.h"

class MEM_DEF{
  public:
    MEM_DEF(){};
    ~MEM_DEF(){};

    // initialisation du chip mémoire.
    //   - chipID : type de mémoire
    //   - cs     : pin chipSelect
    //   - fastIO : not implemented yet !
    bool begin( uint8_t chipID, uint8_t cs, bool fastIO=false ){                      // initialisation de la mémoire
      if( cs > 0 ){                                                                   //   si la pin a été définie
        mem.chip.fastIO = fastIO;                                                     //     on mémorise le mode de transfert
        mem.chip.cs = cs;                                                             //     ........... la pin
        mem.chip.chipID  = chipID;                                                    //     ........... l'ID du chip
        pinMode( cs, OUTPUT );                                                        //     on passe la pin de control en sortie
        digitalWrite( cs, HIGH );                                                     //     on passe en high
        init();                                                                       //     on initialise les paramètres de mémoire
        return( true );                                                               // 
      }                                                                               // 
      return( false );                                                                // 
    }                                                                                 // 

    void focus( uint16_t add ){                         var.focus(  add        );   }
    void focus( uint16_t add, uint16_t index ){         var.focus(  add, index );   }
    void getVal( uint8_t *tab, uint16_t q ){            var.getRaw( tab, q     );   }
    void setVal( uint8_t *tab, uint16_t q ){            var.setRaw( tab, q     );   }
    
    uint16_t getVal(){                return( var.getVal()    );   }
    void     setVal( uint16_t val ){          var.setVal( val );   }

    uint16_t newVar( uint8_t type, uint16_t qte=1 ){    return( mft.allocStd( qte, type ) ? var.addresses.baseAddress : 0 );        }
    uint16_t newRaw( uint16_t qte, uint16_t size ){     return( mft.allocRaw( qte, size ) ? var.addresses.baseAddress : 0 );        }
    uint16_t getMftSize(){                              return( mft.size() );                                                       }

    void del(){
      mft.del();
      var.erase();
    }

    void formatMFT(){
      Serial.print( "fast formating... " );
      mem.formatMFT();
      Serial.println( "done" );
      delay(100);
    }

    void format(){
      Serial.print( "formating... " );
      mem.format();
      Serial.println( "done" );
      delay(100);
    }
    
    void extractMFT( uint16_t nbOctet=0 ){
      nbOctet = ( nbOctet == 0 ) ? mem.mftSize : nbOctet;
      mem.extract( 0, nbOctet );
    }

    void extractData( uint16_t count ){
      mem.extract( mem.dataOffset, count );
    }

    void readRom(){
      if( mem.chip.memType == MemSpi_Rom ){
        mft.analyseData();
      }
      else{
        Serial.println( "not a ROM !" );
      }
    }

  private:
    MEM_CHIP  mem = MEM_CHIP();
    MEM_MFT   mft = MEM_MFT();
    MEM_VAR   var = MEM_VAR();

    void init(){
      switch( mem.chip.chipID ){                                                      //     suivant l'ID chip
          case MemSpi_23LC512 : {                                                     //       si c'est un 23LC512
            Serial.println( "MemSpi_23LC512" );                                       //         sortie moniteur série
            mem.chip.memType = MemSpi_Ram;                                            //         c'est de type RAM
            mem.mftSize      = MemSpi_23LC512_mftSize;                                //         on défini la taille max de la mft
            mem.maxAddress   = MemSpi_23LC512_maxAddress;                             //         on défini l'adresse max
            mem.freeSpace    = MemSpi_23LC512_dataSize;                               //         on défini la taille libre
            mem.dataSize     = MemSpi_23LC512_dataSize;                               //         on défini la quantité maximal de data
            mem.dataOffset   = MemSpi_23LC512_dataOffset;                             //         on défini l'adresse de départ des données
            break;                                                                    // 
          }                                                                           // 
          case MemSpi_25LC256 : {                                                     // 
            Serial.println( "MemSpi_25LC256" );                                       //         sortie moniteur série
            mem.chip.memType = MemSpi_Rom;                                            //         c'est de type ROM
            mem.mftSize      = MemSpi_25LC256_mftSize;                                //         on défini la taille max de la mft
            mem.maxAddress   = MemSpi_25LC256_maxAddress;                             //         on défini l'adresse max
            mem.freeSpace    = MemSpi_25LC256_dataSize;                               //         on défini la taille libre
            mem.dataSize     = MemSpi_25LC256_dataSize;                               //         on défini la quantité maximal de data
            mem.dataOffset   = MemSpi_25LC256_dataOffset;                             //         on défini l'adresse de départ des données
            break;                                                                    // 
          }                                                                           // 
        }                                                                             // 
        mem.begin();                                                                  //     on initialise la mémoire
        var.begin( &mem );                                                            //     on initialise le conteneur de variable
        mft.begin( &mem, &var );                                                      //     on initialise la mft
        switch( mem.chip.chipID ){                                                    //     suivant l'ID chip
          case MemSpi_23LC512 : {                                                     //       si c'est un 23LC512, c'est de la ram, donc on formate
            format();                                                                 //         on format la mft
            break;                                                                    // 
          }                                                                           // 
        }                                                                             // 
        mem.chip.started = true;                                                      // 
    }

};

#endif