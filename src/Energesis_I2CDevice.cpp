/**
 * @file Energesis_I2CDevice.cpp
 * @author José Guerra Carmenate <joseguerracarmenate@gmail.com>
 * @brief Implementación de la clase Energesis_I2CDevice
 * @version 0.1
 * @date 2022-05-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Energesis_I2CDevice.h"

const static char *TAG = "Energesis_I2CDevice";

//#define ENERGESIS_DEBUG Serial


#ifdef ENERGESIS_DEBUG
#include <HardwareSerial.h>

# define DEBUG_PRINT( x )  ENERGESIS_DEBUG.print( x )

# if defined(ESP32)
#  define DEBUG_PRINTF( format, ... )   ENERGESIS_DEBUG.printf( "[%s] ", TAG ); \
                                        ENERGESIS_DEBUG.printf( format, __VA_ARGS__ )
# else
#  define DEBUG_PRINTF( format, ... ) 
# endif

#else

# define DEBUG_PRINT( x ) 
# define DEBUG_PRINTF( format, ... ) 

#endif


Energesis_I2CDevice::Energesis_I2CDevice(uint8_t address, TwoWire *bus ){
  m_i2c = bus;
  m_address = address;
  m_scl = m_sda = -1;
  
}

#if defined(ESP32) || defined(ESP8266)
Energesis_I2CDevice::Energesis_I2CDevice( uint8_t address, int sda, int scl, TwoWire *bus ){
  m_i2c = bus;
  m_address = address;
  m_sda = sda;
  m_scl = scl;
}
#endif

bool Energesis_I2CDevice::begin(){

  if( !m_i2c->begin( m_sda, m_scl ) ){
    return false;
  }
  m_begun = true;
  m_timeout = 100;
  return true;
}

void Energesis_I2CDevice::end(){

#if !(defined(ESP8266) || (defined(ARDUINO_ARCH_AVR) && !defined(WIRE_HAS_END)) \
    || defined(ARDUINO_ARCH_ESP32) || defined(ESP32))
  m_i2c->end();
  m_begun = false;
#endif

}

bool Energesis_I2CDevice::isConnected(){

  if( !m_begun )
    return false;

  m_i2c->beginTransmission( m_address );

  if( m_i2c->endTransmission() == 0 )
    return true;
  
  return false;
}

bool Energesis_I2CDevice::read( uint8_t *buffer, size_t len, bool send_stop){
  size_t step_len = 0;
  for( size_t pos = 0; pos < len; pos += step_len ){
    step_len = (len-pos)>maxBufferSize()?maxBufferSize():(len-pos);
    bool step_stop = (pos < (len-step_len))?false: send_stop;

    if( !_read( buffer+pos, step_len, step_stop ) ) {
      return false;
    }

  }
  return true;
}

bool Energesis_I2CDevice::_read( uint8_t *buffer, size_t len, bool send_stop, uint32_t timeout){
  size_t recv;

  ulong start_millis = millis();

  do{
    m_i2c->requestFrom((uint8_t)m_address, (uint8_t)len, (uint8_t)send_stop);
    yield();
  }while (m_i2c->available() == 0 &&  millis() - start_millis < timeout );
  
  recv = m_i2c->available();

  if( recv != len ){
    DEBUG_PRINT( "[E] no se obtuvo la cantiad de bytes esperados." );
    DEBUG_PRINTF( "Se esperaban %d y se recibieron %d.", len, recv );
    DEBUG_PRINT( "\n" );
  
    return false;
  }

  for (size_t i = 0; i < len; i++) {
    buffer[i] = m_i2c->read();
  }
  return true;
}

size_t Energesis_I2CDevice::maxBufferSize(){
#if defined(I2C_BUFFER_LENGTH)
  return I2C_BUFFER_LENGTH;
#else
  return 32;
#endif
}

bool Energesis_I2CDevice::write( const uint8_t *buffer, size_t len, bool send_stop){

  m_i2c->beginTransmission(m_address);
  
  if( m_i2c->write( buffer, len ) != len ){
    DEBUG_PRINT( "[E] Error de escritura en bus I2C\n" );
    return false;
  }
  
  if( m_i2c->endTransmission(send_stop) != 0 ){
    DEBUG_PRINT("[E] Error al terminar la transmisión\n");
    return false;
  }

  return true;
}


bool Energesis_I2CDevice::setSpeedClock(uint32_t clkSpeed){

#if (ARDUINO >= 157) && !defined(ARDUINO_STM32_FEATHER) && !defined(TinyWireM_h)
  m_i2c->setClock( clkSpeed );
  return true;
#else
  (void)clkSpeed;
  return false;
#endif

}


