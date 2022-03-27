/* RAM mft handler library version 1.0
# 
# Writed by Chansigaud Yann
# ram_spi_mft.h - ram library for 23LC512 from MicroShip
# 
*/

#ifndef MEM_SPI_V3_MFT_h
#define MEM_SPI_V3_MFT_h

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>
#include "mem_spi_v3_define.h"
#include "mem_spi_v3_objects.h"
#include "mem_spi_v3_var.h"
#include "mem_spi_v3_sec.h"


// =================================================
// === MFT CLASS  ==================================
// =================================================

class MEM_MFT{
  public:
    MEM_MFT();
    ~MEM_MFT();

    void begin( MEM_CHIP *chip, MEM_VAR *var );
    void format();

    uint16_t allocRaw( uint16_t qte, uint16_t size );
    uint16_t allocStd( uint16_t qte, uint8_t  type );
    uint16_t allocVar( uint16_t qte, uint8_t  type, uint16_t size=2 );
    void del();

    void printStatus();
    bool started();
    uint16_t size(){  return( memory->mftSize );  }

    void analyseData();
    
  private:
    MEM_SEC   sector   = MEM_SEC();
    MEM_VAR   *variable;
    MEM_CHIP  *memory;
    
    void setVarAdd_from_MftPos();
    void setMftPos_from_VarAdd();

    bool searchFreeSpace( uint16_t size );
    uint16_t freeSpace();

    void sectorTable( bool lock, uint16_t size );

};

#endif