#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>

#include "creds.h"

#include "base_functions.h"

WiFiMulti wifiMulti;
char nodename[20] = "";

/*
 * blink LED <amount> of times and on/off <duration> ms
 */
void blink(int amount, int duration) {
  for (byte i = 0; i < amount; ++i) {
    digitalWrite(LED, HIGH); // turn LED on
    delay(duration);
    digitalWrite(LED, LOW); // turn LED off
    if (amount > 1) delay(duration);
  }
}

/*
 * Connect to configured wifi's
 *
 * WIFI_CREDS must be defined in creds.h and always terminate with NULL:
 * static const char*  WIFI_CREDS[] = {"ssid1", "pw1", "ssid2", ..., NULL}
 */
int connect_to_wifi() {
  for (int i=0; WIFI_CREDS[i] != NULL; i+=2) {
    //DF("i: %i, ssid: %s, pw: %s\n", i, WIFI_CREDS[i], WIFI_CREDS[i+1]);
    wifiMulti.addAP(WIFI_CREDS[i], WIFI_CREDS[i+1]);
  }

  if (WiFi.status() == WL_CONNECTED) {
    D("Wifi already connected to: ");
    DL(WiFi.localIP());
    return 1;
  }

  uint64_t chipid = ESP.getEfuseMac();
  unsigned int l = sprintf(nodename, "ESP_%llx", chipid);
  nodename[l] = '\0';

  DL();
  DF("Hello from %s\n", nodename);
  D("Connecting");

  WiFi.mode(WIFI_STA); // default is WIFI_AP_STA
  WiFi.setHostname(nodename); // must be called as very 1st but with wifi on
  unsigned long startMillis = millis();

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(250);
    D(".");
    if (millis() - startMillis > WIFI_TIMEOUT) {
      DL("Timeout connecting to WiFi...");
      return 0;
    }
  }

  D(" connected to ");
  Serial.print(WiFi.SSID());

  D(". IP address: ");
  DL(WiFi.localIP());

  // setup MDNS
  if (MDNS.begin(nodename)) {
    DF("mdns server started on http://%s.local\n\r", nodename);
  }

  return 1;
}
