# Can_DHT11
A simple non-blocking DHT11 library with every step documented according to the datasheet. Written for Arduino Nano, should be compatible with most boards.
## Usage

``` c++
#include "Can_DHT11.h"

Can_DHT11 dht11(8);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Poll sensor.
  dht11.read();
  
  // Getting relative humidity.
  Serial.print("Relative Humidity: ");
  Serial.println(dht11.getRelativeHumidity());
  
  // Getting temperature.
  Serial.print("Temperature: "); 
  Serial.println(dht11.getTemperature());
}
```
## Troubleshooting
### Values are always 0
Something is causing checksums to not match, connection is bad or you simply forget to poll the sensor, either way data is set to all zeros.
## License
Public Domain, feel free to do anything you want with the code.
