#include <avr/power.h>
#include <RFM69.h>

#include "config.h"

#define ACK_TIME      15

RFM69 radio;
String inData;
String serial_data = "";
bool serial_process = false;

void setup() {
  ADCSRA &= ~(1 << 7);
  power_adc_disable();
  power_twi_disable();

  Serial.begin(9600);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.encrypt(ENCRYPTKEY);
}

void loop() {
  if (Serial.available()) {
    char recieved = Serial.read();
    inData += recieved;
    if (recieved == '\n') {
      serial_data = inData;
      serial_process = true;
      inData = "";
    }
    
  }
  if (serial_process == true) {
    uint8_t index = serial_data.indexOf(';');
    String rfm_receiver_id = serial_data.substring(0, index);
    String rfm_receiver_payload = serial_data.substring(index + 1, -1);

    char buffer[rfm_receiver_payload.length()];
    
    rfm_receiver_payload.toCharArray(buffer, rfm_receiver_payload.length());
    
    Serial.print("Index: ");
    Serial.println(index);
    Serial.println(rfm_receiver_id);
    Serial.println(rfm_receiver_payload);

    radio.sendWithRetry(rfm_receiver_id.toInt(), buffer, strlen(buffer), 5, 50);

    serial_data = "";
    serial_process = false;
  }

  if (radio.receiveDone()) {
     if (radio.ACKRequested()) {
      radio.sendACK();
    }
    Serial.print('[');
    Serial.print(radio.SENDERID, DEC);
    Serial.print("] ");
    
    for (byte i = 0; i < radio.DATALEN; i++) {
      Serial.print((char)radio.DATA[i]);
    }
    Serial.print("[RX_RSSI:");
    Serial.print(radio.RSSI);
    Serial.println("]");
    
  }
}
