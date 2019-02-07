#include <avr/power.h>
#include <RFM69.h>
#include <JeeLib.h>

#include "config.h"

RFM69 radio;

ISR(WDT_vect) {
  Sleepy::watchdogEvent();
}

void setup() {
  ADCSRA &= ~(1 << 7);
  power_adc_disable();
  power_twi_disable();

  Serial.begin(9600);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.encrypt(ENCRYPTKEY);
}

void loop() {
  if (radio.receiveDone()) {
    uint8_t sender_id = radio.SENDERID;
    uint8_t sender_rssi = radio.RSSI;
    String data_recv = "";
    
    for (byte i = 0; i < radio.DATALEN; i++) {
      data_recv += (char)radio.DATA[i];
    }

    if (radio.ACKRequested()) {
      radio.sendACK();
    }

    Serial.print('[');
    Serial.print(sender_id);
    Serial.print("] ");
    Serial.print(data_recv);
    Serial.print("[RX_RSSI:");
    Serial.print(sender_rssi);
    Serial.println("]");

    Serial.flush();
  }
  radio.receiveDone();

  Sleepy::powerDown();
}
