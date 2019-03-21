#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>

#include "creds.h"

#include "base_functions.h"

WiFiMulti wifiMulti;

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
  char nodename[20] = "";

  uint64_t chipid = ESP.getEfuseMac();
  unsigned int l = sprintf(nodename, "ESP_%llx", chipid);
  nodename[l] = '\0';

  return connect_to_wifi(nodename);
}

/*
 * setting the hostname requires formatting the memory
 * run "make fst" when hostname is change
 */
int connect_to_wifi(const char *givenNodename) {
  DL("Connecting... ");
  char nodename[20] = "";
  strcpy(nodename, givenNodename);

  for (int i=0; WIFI_CREDS[i] != NULL; i+=2) {
    // DF("i: %i, ssid: %s, pw: %s\n", i, WIFI_CREDS[i], WIFI_CREDS[i+1]);
    wifiMulti.addAP(WIFI_CREDS[i], WIFI_CREDS[i+1]);
  }

  if (WiFi.status() == WL_CONNECTED) {
    D("Wifi already connected to: ");
    DL(WiFi.localIP());
    return 1;
  }

  WiFi.mode(WIFI_STA); // default is WIFI_AP_STA
  // must be called as very 1st but with wifi on
  if (!WiFi.setHostname(nodename)) {
    DL("Setting hostname failed");
  }

  if (wifiMulti.run() == WL_CONNECTED) {
    DF("Connected to %s with %s (%s)\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(), WiFi.getHostname());

    // setup MDNS
    if (MDNS.begin(nodename)) {
      DF("MDNS server started on http://%s.local\n\r", nodename);
    }
    return 1;
  } else {
    return 0;
  }
}
