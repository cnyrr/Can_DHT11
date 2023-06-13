#include "Can_DHT11.h"

Can_DHT11::Can_DHT11(uint8_t DATAPIN): _PIN(DATAPIN) {
  // Power up DHT.
  pinMode(_PIN, OUTPUT);
  digitalWrite(_PIN, HIGH);

  // Get the initialization time.
  last_poll = millis();
}

Can_DHT11::~Can_DHT11() {}

void Can_DHT11::read() {
  // Handle millis() overflow.
  if (last_poll > millis()) { last_poll = 0; }

  // DHT needs 1 second between polls.
  if ((millis() > 1000) && (millis() - 1000 > last_poll)) {
    transferData();
    last_poll = millis();
  }
  else {
    delay(1001);
    read();
  }
}

float Can_DHT11::getRelativeHumidity() {
  // Calculate the relative humidity instead of storing it again.
  return (float) data[0] + data[1] * 0.1;
}

float Can_DHT11::getTemperature() {
  // Calculate the temperature instead of storing it again.
  return (float) data[2] + data[3] * 0.1;
}

void Can_DHT11::transferData() {
  // Prepare our data.
  uint8_t block = 0;
  uint8_t current_bit = 0;
  uint8_t checksum = 0;
  connection_fail = 0;
  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  // Set bus voltage LOW for at least 18ms to ensure DHT receives start signal.
  digitalWrite(_PIN, LOW);
  delay(20);

  // Start of time critical code.
  noInterrupts();

  // Pull-up and wait 20-40 microseconds for response.
  digitalWrite(_PIN, HIGH);
  delayMicroseconds(45);
  pinMode(_PIN, INPUT);

  // Wait for first part of DHT acknowledge signal to end.
  waitSignal(LOW);

  // Wait for second part of DHT acknowledge signal to end.
  waitSignal(HIGH);

  // Get one bit at a time.
  for (block = 0; block < 5; block++) {
    for (current_bit = 0; current_bit < 8; current_bit++) {
      // Wait until next bit is ready.
      waitSignal(LOW);

      // "1" will be represented as 70 microseconds of HIGH.
      // "0" will be represented as 26-28 microseconds of HIGH, we just ignore it.
      delayMicroseconds(30);

      // Set the received bit.
      if (digitalRead(_PIN)) {data[block] = data[block] + 1;}

      // Left shift unless we got the last bit.
      if (current_bit != 7) {data[block] = data[block] << 1;}

      // Wait the rest of the HIGH signal if we received "1".
      waitSignal(HIGH);
    }
  }

  // DHT will signal LOW after transmiting last bit.
  waitSignal(LOW);
 
  // End of time critical code.
  interrupts();

  // Clear the channel.
  pinMode(_PIN, OUTPUT);
  digitalWrite(_PIN, HIGH);

  // Calculate checksum.
  checksum = data[0] + data[1] + data[2] + data[3];

  // Dump data if checksum is wrong or connection fails.
  if(checksum != data[4] || connection_fail) {
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;
  }
}

void Can_DHT11::waitSignal(uint8_t state) {
  uint8_t counter = 100;

  // Stop waiting and raise connection fail flag if counter reachs 0.
  while(digitalRead(_PIN) ==  state) {
    if (counter == 0) {
      connection_fail = 1;
      break;
    }
    counter = counter - 1;
  }
}
