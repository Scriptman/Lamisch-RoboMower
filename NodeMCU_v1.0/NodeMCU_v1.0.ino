#include <SoftwareSerial.h>
#include "SerialCom_non_blocking.h" // For fast serial comm.

// Settings NODEMCU
//----------------------------------------------------

// Node MCU Version
char  Version[8] = "V1.0";

bool Auto_Sync_Settings_Enabled = 1;        // Peridoically Syncs the settings tab on the APP with actual Mower Settings
bool MEGA_Watch_Enabled = 0;                // Detects if the Mower MEGA has forzen and resets it
bool Auto_Wire = 1;                         // Turns the wire on and off automatically (Requires PCB version of Wire transmitter)

// Software serial connection to Mower main board
SoftwareSerial MowerSerial(D2, D3); // RX D2 & TX D3
SerialCom MowerCom (MowerSerial, 40);

#define LED LED_BUILTIN
//#define LED D4      // on various NODEMCU boards the LED Pin is different.....

/* Set these to your desired softAP credentials. They are not configurable at runtime */
const char *softAP_ssid = "Lamisch Robomower Fallback AP";
const char *softAP_password = "12345678";

/* hostname for mDNS. Should work at least on windows. Try http://robomower.local */
const char *myHostname = "robomower";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char wifi_ssid[32] = "";
char wifi_password[32] = "";

void setup() {
    // setup serials..
    Serial.begin(115200);
    MowerSerial.begin(57600);
    MowerCom.begin();

    Serial.println();
    Serial.print("Lamisch Mower Wi-Fi Module Version:");
    Serial.print(Version);
    Serial.println("Setting up NodeMCU........");
    Serial.println("");

    pinMode(D2,INPUT);          // Serial TX pin
    pinMode(D3,OUTPUT);         // Serial RX pin
    pinMode(D5,OUTPUT);
    pinMode(D6,OUTPUT);
    digitalWrite(D5, HIGH);
    digitalWrite(LED, HIGH);    // Turn off LED Light

    // Start 
    SetupAccesspoint();
    SetupWebServer();

    // Try to load ssid and password from eeprom
    TryLoadWifiCredentials();

    pinMode(LED, OUTPUT); // Turn on LED so we know the setup is ready.
}

void loop() {
    TryConnectWifi();
    TryConnectMQTT();
    

    // Do work:
    //DNS
    dnsServer.processNextRequest();
    //HTTP
    server.handleClient();
}
