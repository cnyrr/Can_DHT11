#ifndef Can_DHT11_H
#define Can_DHT11_H

#include "Arduino.h"

class Can_DHT11 {
  public:
    Can_DHT11(uint8_t DATAPIN);
    ~Can_DHT11();
  
    void read();
    float getRelativeHumidity();
    float getTemperature();

  private:
    uint32_t last_poll;
    uint8_t _PIN;
    uint8_t data[5];

    void waitSignal(uint8_t state);
    void transferData();

};
#endif