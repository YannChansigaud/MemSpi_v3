/**********************************************************************
 * 
 * This is the C++ part of the mem_spi_v3_mft library.
 * See mem_spi_v3_mft.h for a full documentation.
 * 
 *********************************************************************/

#include "mem_spi_v3_mft.h"


// =================================================
// === MEM_SPI_V3_MFT class  ==========================
// =================================================

MEM_MFT::MEM_MFT(){}

MEM_MFT::~MEM_MFT(){}

void MEM_MFT::begin( MEM_CHIP *chip, MEM_VAR *var ){
  #ifdef MFT_debug
  Serial.println( "MEM_MFT : begin" );
  #endif
  memory   = chip;
  variable = var;
  sector.begin( chip );
}

void MEM_MFT::format(){
  #ifdef MFT_debug
  Serial.println( "MEM_MFT : format" );
  #endif
  memory->formatMFT();
}                                                           // 


uint16_t MEM_MFT::allocRaw( uint16_t qte, uint16_t size ){
  #ifdef MFT_debug
  Serial.println( "MEM_MFT : allocRaw" );
  #endif
  return( allocVar( qte, RamSPI_userDefined, size ) );
}

uint16_t MEM_MFT::allocStd( uint16_t qte, uint8_t type ){
  #ifdef MFT_debug
  Serial.println( "MEM_MFT : allocStd" );
  #endif
  return( allocVar( qte, type ) );
}

uint16_t MEM_MFT::allocVar( uint16_t count, uint8_t type, uint16_t size ){                  // routine d'allocation de 'count' variable 'type' de taille 'size'
  if( started() ){                                                                          //   si la mémoire a été initialisée
    #ifdef MFT_debug
    Serial.println( "MEM_MFT : allocVar " );
    #endif
    if( type == RamSPI_userDefined ){                                                       //     si c'est un type non standard
      variable->setNewRaw( size, count );                                                   //       la taille doit être définie par l'utilisateur
    }                                                                                       // 
    else{                                                                                   //     si ce n'est pas un type défini par l'utilisateur
      variable->setNewStd( type, count );                                                   //       alors la taille est conditionnée suivant le type
    }                                                                                       // 
    size = variable->overallSize();                                                         //     on récupère la taille totale nécessaire
    if( size > memory->freeSpace ){                                                         //     si la taille * quantité > espace libre alors retour 0
      #ifdef MFT_debug
      Serial.println( "not enough space" );
      #endif
      return( 0 );
    }
    if( searchFreeSpace( size ) ){                                                          //     si on trouve un emplacement adhéquat
      #ifdef MFT_debug
      Serial.println( "ok" );
      #endif
      sectorTable( lockdownSector, size );                                                  //       on verrouille la zone mft correspondante
      variable->save();                                                                     //       on enregistre la définition de la variable
      variable->format();                                                                   //       on format à 0 l'espace mémoire data
      #ifdef MFT_debug
      variable->printProperties();                                                          //       on imprime la définition complète de la variable
      #endif
      return( variable->addresses.baseAddress );                                            //       on retourne vrai
    }                                                                                       // 
  }                                                                                         // 
  return( 0 );                                                                              // si la ram SPI n'a pas été initialisée, on retourne zéro
}                                                                                           // 


void MEM_MFT::del(){                                                    // méthode d'effacement
  #ifdef MFT_debug
  Serial.println( "MEM_MFT : del" );
  #endif
  if( variable->addresses.baseAddress != 0 && started() ){              //   si la variable indiquée possède une adresse en mémoire
    sectorTable( releaseSector, variable->overallSize() );              //     on libère la taille totale en mft
  }                                                                     // 
}                                                                       // 

void MEM_MFT::setVarAdd_from_MftPos(){
  uint16_t varFirstAdd   = sector.varAdd();
  uint16_t dataAddOffset = memory->dataOffset;
  #ifdef MFT_debug
  Serial.print( "MEM_MFT : setVarAdd_from_MftPos " );
  Serial.print( varFirstAdd );
  Serial.print( " offset = " );
  Serial.println( dataAddOffset );
  #endif
  variable->setFirstAdd( varFirstAdd + dataAddOffset );
}

void MEM_MFT::setMftPos_from_VarAdd(){
  uint16_t varFirstAdd = variable->addresses.baseAddress ;
  uint16_t dataAddOffset = memory->dataOffset;
  #ifdef MFT_debug
  Serial.print( "MEM_MFT : setMftPos_from_VarAdd " );
  Serial.print( varFirstAdd );
  Serial.print( " offset = " );
  Serial.println( dataAddOffset );
  #endif
  sector.set( varFirstAdd - dataAddOffset );                                        // L
}

bool MEM_MFT::searchFreeSpace( uint16_t size ){                                       // on recherche n espace libre.
  #ifdef MFT_debug
  Serial.println( "MEM_MFT : searchFreeSpace" );
  #endif
  if( started() ){                                                                    //   si la ram est initialisée
    variable->addresses.baseAddress = 0;                                              //     on défini l'addresse de la variable à 0
    sector.reset();                                                                   //     on reposition le pointeur sector à 0
    size += ((( size >> 1 ) << 1) == size ) ? 0 : 1 ;                                 //     si size /2 puis *2 égale size, alors c'est un pair, sinon c'est un impair et on lui rajoute 1 pour retrouver un pair
    size >>= 1;                                                                       //     attention chaque bit dans MFT désigne un emplacement de 16 bits donc division par 2
    uint16_t count = 0;                                                               //     compteur d'espace contigus
    while( (count < size) && (sector.regAdd() < memory->mftSize) ){                   //     temps qu'on a pas trouvé suffisament de place (et qu'on reste dans la table mft)
      sector.loadReg();                                                               // L     on récupère le registre en cours
      if( count == 0 ){                                                               //         si le compteur est à zéro 0
        setVarAdd_from_MftPos();                                                      //           on défini l'adresse de base de la variable
      }                                                                               // 
      switch( sector.get() ){                                                         //         suivant l'état du secteur
        case sectorEmpty :{                                                           //           0 correspond à l'état secteur vide
          count += 8;                                                                 //             on rajoute 8 au compteur
          break;                                                                      // 
        }                                                                             // 
        case sectorFull : {                                                           //           255 correspond à l'état secteur plein
          count = 0;                                                                  //             on remet le compteur à 0
          break;                                                                      // 
        }                                                                             // 
        default :{                                                                    //           si le secteur n'est ni plein ni vide
          while( ( sector.secPos() < 8 ) && ( count < size ) ){                       //           pour chaque bit du secteur récupéré
            count = sector.isLock() ? 0 : count + 1;                                  //             si on tombe sur un bit 1 on met à O sinon on rajoute 1
            #ifdef MFT_debug
            Serial.println( count<<1 );
            #endif
            sector.nextPos();                                                         //               on passe au bit suivant
            if( count == 0 ){                                                         //             si le compteur d'espace vide contigu est repassé à 0
              #ifdef MFT_debug
              Serial.println( "count reset to 0" );
              #endif
              setVarAdd_from_MftPos();                                                //               on défini la nouvelle position en mémoire
            }                                                                         // 
          }                                                                           // 
          break;                                                                      //          
        }                                                                             // 
      }
      sector.nextReg();                                                               //         on passe au secteur suivant (attention : ne charge PAS le secteur suivant)
    }                                                                                 // 
    return( count >= size );                                                          // 
  }                                                                                   //    
  return( false );                                                                    //    
}                                                                                     // 

uint16_t MEM_MFT::freeSpace(){
  return( memory->freeSpace );
}

void MEM_MFT::analyseData(){
  uint16_t memAdd = 0;
  uint16_t freeSpace = 0;
  sector.reset();
  while( memAdd < memory->mftSize ){
    sector.loadReg();
    freeSpace += sector.freeSpace();
    memAdd++;
    sector.nextReg();
  }
  memory->freeSpace = freeSpace;
  Serial.print( freeSpace );
  Serial.print( " / " );
  Serial.print( memory->dataSize );
  Serial.print( " octet(s) free (");
  Serial.print( 100.0*(1.0*freeSpace / memory->dataSize) );
  Serial.println( " %)" );
  sector.reset();
  uint16_t dataSize = 1;
  while( (memAdd < memory->maxAddress) && dataSize){
    variable->focus( memAdd );
    dataSize = variable->overallSize();
    if( dataSize ){
      Serial.println();
      Serial.print( memAdd );
      Serial.print( " : " );
      Serial.print( dataSize );
      Serial.println( " octet(s)" );
      variable->printProperties();
      memAdd += dataSize;
    }
  }
}

void MEM_MFT::sectorTable( bool lock, uint16_t count ){                               // 
  #ifdef MFT_debug
  Serial.print( "MEM_MFT : sectorTable " );
  Serial.println( lock ? "lock" : "unlock" );
  #endif
  if( started() ){                                                                    //   si la mft est initialisée
    memory->freeSpace += lock ? -count : count ;                                      //     on actualise la quantité de mémoire disponible
    uint16_t nbBit = count >> 1;                                                      //     on défini le nombre de bit à modifier dans la mft
    setMftPos_from_VarAdd();                                                          //     on positionne la mft suivant l'adresse de la variable
    /*
    while( nbBit ){                                                                   //       temps qu'il reste des bits à modifier
      nbBit -= sector.setBit( lock, nbBit );                                          //       setBit renvoi le nombre de bit modifié, que l'on soustrait à nbBit
      if( (sector.secPos() > 7) || nbBit == 0 ){                                      //       si la position dans le registre est > 7 OU si le nombre de bit à modifier est 0
        sector.saveReg();                                                             //         alors on enregistre le registre
        if( nbBit ){                                                                  //         s'il reste des bits à modifier
          sector.nextReg();                                                           //           on passe au registre suivant
          sector.loadReg();                                                           //           on le charge
        }                                                                             //         
      }                                                                               //       
      nbBit = (nbBit > count) ? 0 : nbBit ;                                           //    sécurité : si nbBit > à count alors il est devenu négatif, mais pour un uint16_t il devrait être > à count
    }
    */
    
    while( nbBit-- ){                                                                 // 
      #ifdef MFT_debug
      Serial.print( "MEM_MFT : sectorTable, nbBit = " );
      Serial.println( nbBit );
      #endif
      if( lock ){     sector.lockBit();  }                                            // 
      else{           sector.freeBit();  }                                            // 
      sector.nextPos();                                                               // 
      if( (sector.secPos() > 7) || nbBit == 0 ){                                      // 
        #ifdef MFT_debug
        Serial.println( "MEM_MFT : sectorTable, saveReg " );
        #endif
        sector.saveReg();                                                             // 
        if( nbBit ){                                                                  // 
          #ifdef MFT_debug
          Serial.println( "MEM_MFT : sectorTable, loadNextReg " );
          #endif
          sector.nextReg();                                                           // 
          sector.loadReg();                                                           // 
        }                                                                             // 
      }                                                                               // 
    }                                                                                 // 
    
  }                                                                                   // 
}                                                                                     // 


void MEM_MFT::printStatus(){
  switch( memory->chip.memType ){
    case MemSpi_23LC512 : {
      Serial.print( "SRAM  23LC512 " );
      break;
    }
    case MemSpi_25LC256 : {
      Serial.print( "FLASH 25LC256 " );
      break;
    }
  }
  Serial.println( memory->chip.started ? "ready" : "waiting" );
  Serial.print(   memory->freeSpace );
  Serial.print(   " / " );
  Serial.print(   memory->dataSize );
  Serial.println( " byte(s)" );
  Serial.print(   "control pin : " );
  Serial.println( memory->chip.cs );
  Serial.println();
}

bool MEM_MFT::started(){
  #ifdef MFT_debug
  Serial.print( "MEM_MFT : started = " );
  Serial.println( memory->chip.started ? "true" : "false" );
  #endif
  return( memory->chip.started  );
}