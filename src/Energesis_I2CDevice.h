/**
 * @file Energesis_I2CDevice.h
 * @author José Guerra Carmenate <joseguerracarmenate@gmail.com>
 * @brief 
 * @version 0.1
 * @date 2022-05-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _ENERGESIS_I2CDEVICE_H_
#define _ENERGESIS_I2CDEVICE_H_

#include <Wire.h>

/**
 * @brief Esta clase que especifica cómo nos comunicaremos con un dispositivo usando el bus I2C.
 * 
 * @note Inspirado en la clase Adafruit_I2CDevice (https://github.com/adafruit/Adafruit_BusIO/blob/master/Adafruit_I2CDevice.h)
 */
class Energesis_I2CDevice{

public:
  Energesis_I2CDevice( uint8_t address, TwoWire *bus = &Wire );

#if defined(ESP32)
  Energesis_I2CDevice( uint8_t address, int sda, int scl, TwoWire *bus = &Wire );
#endif

  bool begin();
  void end();

  void setBus( TwoWire *bus ){ if( bus ) m_i2c = bus;  }

  void setTimeout( uint16_t timeout ){ m_i2c->setTimeOut(timeout); }

  uint16_t getTimeout( ){ return m_i2c->getTimeOut(); }

  uint8_t getAddress(){ return m_address; }
  bool isConnected();

  bool read( uint8_t *buffer, size_t len, bool send_stop = true );

  bool write( const uint8_t *buffer, size_t len, bool send_stop = true );

  bool setSpeedClock(uint32_t clkSpeed);
  size_t maxBufferSize();

private:
  TwoWire *m_i2c;     //!< Bus I2C utilizado
  uint8_t m_address;  //!< Dirección I2C del dispositivo
  int m_sda;          //!< Pin SDA del bus I2C
  int m_scl;          //!< Pin SCL del bus I2C
  bool m_begun;       //!< Indica si se ha inicializado el bus
  int m_timeout;      //!< Tiempo de espera en la lectura en ms

  bool _read( uint8_t *buffer, size_t len, bool send_stop = true, uint32_t timeout = 100 );

};


#endif
