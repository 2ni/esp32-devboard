/*
 * gateway with my esp32 board
 * dependencies: pio lib install 92 (lowerpowerlab RFM69 (library)[https://github.com/LowPowerLab/RFM69])
 * some updates needed to be done to make it work though:
 * RFM69.cpp: ~120: _spi->begin(14, 12, 13, 15); // sck, miso, mosi, cs
 * RFM69.cpp: ~152: pinMode(_interruptPin, INPUT); // set as output before attachInterrupt command
 *
 * SCK       : e14
 * MISO      : e12
 * MOSI      : e13
 * RFM_CS    : e18
 * RFM_INT   : e35
 */


#include <Arduino.h>
#include "base_functions.h"

#include <RFM69.h>

#define NODEID 5
#define TONODEID 2
#define NETWORKID 33

RFM69 radio(RFM_CS, RFM_INT, true);
char reply[RF69_MAX_DATA_LEN+1];

void setup() {
  pinMode(LED, OUTPUT);

  // uart
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  DL("Hello there.");
  blink(1, 100);

  if (!radio.initialize(RF69_868MHZ, NODEID, NETWORKID)) { // frq, nodeID, networkID
    DL("init failed");
  }

  radio.setHighPower();
  radio.encrypt(0);
  radio.spyMode(true);

  DF("version: 0x%02x\n", radio.readReg(0x10));
}

byte ackCount = 0;
uint32_t packetCount = 0;

unsigned long now = millis();
void loop() {
  if (millis() - now > 1000) {
    blink(1);
    now = millis();
    // DF("node %u waiting on network %u...\n", NODEID, NETWORKID);

    /*
    radio.send(TONODEID, "Hello!", 6, 1);
    uint32_t sent_time = millis();
    while (millis()-sent_time < 1000) {
      if (radio.ACKReceived(TONODEID)) {
        DL("ack!");
      }
    }
    DL("no ack!");
    */
  }

  if (radio.receiveDone()) {
    /*
    char msg[radio.DATALEN];
    for (byte i=0; i<radio.DATALEN; i++) {
      msg[i] = radio.DATA[i];
    }
    */
    DF("%u->%u (%u): '%s'   [RX_RSSI: %d]", radio.SENDERID, NODEID, NETWORKID, radio.DATA, radio.RSSI);

    if (radio.ACKRequested()) {
      byte theNodeID = radio.SENDERID;
      // return an answer including the ack
      uint8_t len = sprintf(reply, "s:%u", (uint8_t)(random(1, 6)));
      radio.sendACK(reply, len);
      DF(" ...ack sent.");
      // also request an ack every 3rd ack request to check both sides
      /*
      if (ackCount++%3==0) {
        delay(3);
        DF("Pinging %u - ACK...", theNodeID);
        if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0)) { // 0 = only 1 attempt no retries
          DF("ok!\n");
        } else {
          DF("failed.\n");
        }
      }
      */
    }
    DL();
  }
}
