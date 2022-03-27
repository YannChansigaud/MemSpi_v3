#include <SPI.h>
#include "mem_spi_v3_memory.h"

MEM_DEF ram;

void setup() {
  Serial.begin( 115200 );
  Serial.println( "debut setup" );

  pinMode( 11, INPUT );
  pinMode( 12, OUTPUT );
  pinMode( 13, OUTPUT );
  digitalWrite( 12, HIGH );
  digitalWrite( 13, HIGH );
  
  SPI.begin();
  ram = MEM_DEF();
  
  if( ram.begin( MemSpi_25LC256, 12 ) ){
//  if( ram.begin( MemSpi_23LC512, 13 ) ){
//    ram.formatMFT();
    Serial.println( "ram ok" );
  }

  uint16_t tabAdd = ram.newVar( RamSPI_uint16_t, 10 );
  Serial.println( tabAdd );
  
  ram.focus( tabAdd, 5 );
  ram.setVal( 17 );
  ram.focus( tabAdd, 3 );
  ram.setVal( 12 );

  ram.focus( tabAdd, 5 );
  Serial.println( ram.getVal() );
  ram.focus( tabAdd, 3 );
  Serial.println( ram.getVal() );

  
  ram.readRom();
  
  Serial.println( "==========================================" );
  
  uint16_t partial_area = 512;
  Serial.println();
  Serial.println( "mft partial extract : " );
  ram.extractMFT( partial_area >> 4 );
  Serial.println();
  Serial.println( "data partial extract : " );
  ram.extractData( partial_area );
  
  Serial.println( "==========================================" );
  
  Serial.println();
  Serial.println( "fin setup" );
}

void loop() {
  if( digitalRead( 11 ) == LOW ){
    ram.format();
    delay(1000);
  }
}
