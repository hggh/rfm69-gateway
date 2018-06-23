#include <avr/power.h>
#include <RFM69.h>

#include "config.h"

#define ACK_TIME      15

RFM69 radio;
String inData;
String serial_data = "";
bool serial_process = false;
bool rfm69_send_burst = false;

void setup() {
  ADCSRA &= ~(1 << 7);
  power_adc_disable();
  power_twi_disable();

  Serial.begin(9600);
  Serial.println("Starting Gateway System...");
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
    rfm69_send_burst = false;
    if (serial_data.startsWith("B")) {
      rfm69_send_burst = true;
      serial_data.remove(0, 1);
    }
    uint8_t index = serial_data.indexOf(';');
    String rfm_receiver_id = serial_data.substring(0, index);
    String rfm_receiver_payload = serial_data.substring(index + 1, -1);

    char buffer[rfm_receiver_payload.length()];
    
    rfm_receiver_payload.toCharArray(buffer, rfm_receiver_payload.length());
    
    Serial.print("Index: ");
    Serial.println(index);
    Serial.println(rfm_receiver_id);
    Serial.println(rfm_receiver_payload);

    if (rfm69_send_burst == true) {
      radio.listenModeSendBurst(rfm_receiver_id.toInt(), buffer, strlen(buffer));
    }
    else {
      radio.sendWithRetry(rfm_receiver_id.toInt(), buffer, strlen(buffer), 5, 10);
    }

    serial_data = "";
    serial_process = false;
  }

  if (radio.receiveDone()) {
    Serial.print('[');
    Serial.print(radio.SENDERID, DEC);
    Serial.print("] ");
    
    for (byte i = 0; i < radio.DATALEN; i++) {
      Serial.print((char)radio.DATA[i]);
    }
    Serial.print("[RX_RSSI:");
    Serial.print(radio.RSSI);
    Serial.println("]");
    
     if (radio.ACKRequested()) {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
    }
  }
}
