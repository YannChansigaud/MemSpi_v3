// #define RAM_verif
// #define MEM_debug
// #define VAR_debug
// #define MFT_debug
// #define SEC_debug
// #define LOW_debug
/*
#ifndef RamSPI_cs
#define RamSPI_cs 10
#endif
*/

#ifndef MEM_SPI_V3_DEFINE_h
#define MEM_SPI_V3_DEFINE_h

//      MemSpi Type description :    TTMMMMMM
//                                   TT = Type of memory
//                                   00 = not defined
//                                   01 = Ram memory
//                                   10 = Rom memory
//                                   11 = not used
//                                   
//                                   MMMMMM = Chip Identifier
//                                   000001 = 23LC512
//                                   000010 = 25LC256
//                                   

#define MemSpi_Ram                  B00000001
#define MemSpi_Rom                  B00000010

#define MemSpi_23LC512              B00000100
#define MemSpi_23LC512_mftSize      0x0F00
#define MemSpi_23LC512_dataOffset   0x1000
#define MemSpi_23LC512_dataSize     0xF000
#define MemSpi_23LC512_maxAddress   0x9999
#define MemSpi_25LC256              B00001000
#define MemSpi_25LC256_mftSize      0x03C0
#define MemSpi_25LC256_dataOffset   0x0400
#define MemSpi_25LC256_dataSize     0x3C00
#define MemSpi_25LC256_maxAddress   0x4000

#define _MEM_FREQUENCY_             1000000



// =========================================== RAM CHIP

#define RamSPI_modeGet              B00000101
#define RamSPI_modeSet              B00000001

#define RamSPI_byteMode             B00000000
#define RamSPI_pageMode             B10000000
#define RamSPI_sequentialMode       B01000000

// #define _RAM_CONFIG_(cs)            SPI.beginTransaction( SPISettings( _MEM_FREQUENCY_, MSBFIRST, SPI_MODE0 ) );    digitalWrite( cs , LOW );    SPI.transfer( RamSPI_modeSet );    //   Serial.println( ">> config"  );
#define _RAM_CONFIG_(cs)            active( RamSPI_modeSet );


// =========================================== ROM CHIP

#define RomSPI_statusRead             B00000101
#define RomSPI_statusWrite            B00000001

#define RomSPI_noneProtected          B00000000
#define RomSPI_quarterProtected       B00000100
#define RomSPI_halfProtected          B00001000
#define RomSPI_fullProtected          B00001100

#define RomSPI_WPEnabled              B10000000
#define RomSPI_WPDisabled             B00000000

#define RomSPI_WriteLatch             B00000110

#define RomSPI_WriteInProgress        B00000001
#define RomSPI_WriteInProgressMask    B00000001
#define RomSPI_WriteLatchEnabled      B00000010
#define RomSPI_WriteLatchEnabledMask  B00000010

#define RomSPI_WEL_WIP_Mask           ( RomSPI_WriteInProgress | RomSPI_WriteLatchEnabled )


//#define _ROM_CONFIG_(cs)        SPI.beginTransaction( SPISettings( _MEM_FREQUENCY_, MSBFIRST, SPI_MODE0 ) );    digitalWrite( cs , LOW );    SPI.transfer( RomSPI_statusWrite );    // Serial.println( ">> config"  );
//#define _ROM_STATUS_(cs)        SPI.beginTransaction( SPISettings( _MEM_FREQUENCY_, MSBFIRST, SPI_MODE0 ) );    digitalWrite( cs , LOW );    SPI.transfer( RomSPI_statusRead  );    // Serial.println( ">> get status"  );
//#define _ROM_WRITELATCH(cs)     SPI.beginTransaction( SPISettings( _MEM_FREQUENCY_, MSBFIRST, SPI_MODE0 ) );    digitalWrite( cs , LOW );    SPI.transfer( RomSPI_WriteLatch  );    // Serial.println( ">> write latch"  );
#define _ROM_CONFIG_(cs)          active( RomSPI_statusWrite );
#define _ROM_STATUS_(cs)          active( RomSPI_statusRead  );
#define _ROM_WRITELATCH(cs)       active( RomSPI_WriteLatch  );



// =========================================== MEM INSTRUCTION

#define MemSPI_read             B00000011
#define MemSPI_write            B00000010

//#define _MEM_READ(cs)           SPI.beginTransaction( SPISettings( _MEM_FREQUENCY_, MSBFIRST, SPI_MODE0 ) );    digitalWrite( cs , LOW );    SPI.transfer( MemSPI_read    );
//#define _MEM_WRITE(cs)          SPI.beginTransaction( SPISettings( _MEM_FREQUENCY_, MSBFIRST, SPI_MODE0 ) );    digitalWrite( cs , LOW );    SPI.transfer( MemSPI_write   );
//#define _MEM_RELEASE(cs)                                                                                        digitalWrite( cs , HIGH );   SPI.endTransaction();

#define _MEM_READ(cs)           active( MemSPI_read  );
#define _MEM_WRITE(cs)          active( MemSPI_write );
#define _MEM_RELEASE(cs)        release();

#define _MEM_ADD(a)             SPI.transfer16( a    );     // Serial.print( "  add 16 bits = " );    Serial.println( a );
#define _MEM_GET_B(a)       a = SPI.transfer(   0xFF );     // Serial.print( "  get  8 bits = " );    Serial.println( a );
#define _MEM_GET_W(a)       a = SPI.transfer16( 0xFF );     // Serial.print( "  get 16 bits = " );    Serial.println( a ); 
#define _MEM_SEND_B(a)          SPI.transfer(   a    );     // Serial.print( " send  8 bits = " );    Serial.println( a ); 
#define _MEM_SEND_W(a)          SPI.transfer16( a    );     // Serial.print( " send 16 bits = " );    Serial.println( a ); 

#define _MEM_ERASE_             SPI.transfer( 0 );





// =========================================== VAR DEFINITION

#define RamSPI_free                 B00000000
#define RamSPI_type_mask            B00001111
#define RamSPI_size_mask            B00000011
#define RamSPI_count_mask           0x0FFF

#define RamSPI_undefined            B00000000     // 1
#define RamSPI_uint16_t             B00000001     // 1
#define RamSPI_uint32_t             B00000010     // 2
#define RamSPI_uint64_t             B00000011     // 3
#define RamSPI_int16_t              B00000101     // 5
#define RamSPI_int32_t              B00000110     // 6
#define RamSPI_int64_t              B00000111     // 7
#define RamSPI_float                B00001010     // 10
#define RamSPI_double               B00001010     // 10
#define RamSPI_userDefined          B00001100     // 12

//      VarsDeclaration             0xTCCC    bin : TTTT SSSS SSSS SSSS
//                              T : type
//                              S : SIZE max 4096

//                        type    : FSWW    S = signed    WW = wide : 01 = 1 octet    10 = 2 octets   11 = 4 octets
//                                : 0000    free sector
//                                : 0001    uint16_t    // 2 octets
//                                : 0010    uint32_t    // 4 octets
//                                : 0011    uint64_t    // 8 octets
//                                : 0101    int16_t     // 2 octets
//                                : 0110    int32_t     // 4 octets
//                                : 0111    int64_t     // 8 octets
//                                : 
//                                : 1010    float       // 4 octets
//                                : 1110    double      // 4 octets
//                                : 

//      VarsContruction           : 0xTSSS      // type declaration
//                                : 0xCCCC      // count
//                                : 0xIIII      // index
//                                : 0xDDDD      // data
//                                : 0xDDDD      // data
//                                : 0xDDDD      // data
//                                : ....        // ...

#define setNewVar    true
#define setNewIndex  false

#define lockdownSector  true
#define releaseSector   false

#define sectorFull   255
#define sectorEmpty    0


#endif