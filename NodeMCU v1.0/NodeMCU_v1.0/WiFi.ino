#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

/* Soft AP network parameters */
IPAddress apIP(192, 168, 5, 1);
IPAddress netMsk(255, 255, 255, 0);

// DNS server
const byte DNS_PORT = 53;

// WiFi Client
WiFiClient wifiClient;
DNSServer dnsServer;
ESP8266WebServer server(80);

/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
long lastConnectTry = 0;

/** Current WLAN status */
int status = WL_IDLE_STATUS;

void SetupAccesspoint ()
{
    // Check if there are some credentials stored.
    Serial.println("Configuring access point...");
    
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(softAP_ssid, softAP_password);
    delay(500); // Without delay I've seen the IP address blank
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    /* Setup the DNS server redirecting all the domains to the apIP */  
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", apIP);
}

void TryConnectWifi ()
{
    if (connect) {
        Serial.println ( "Connect requested" );
        connect = false;
        ConnectWifi();
        lastConnectTry = millis();
    }

    int s = WiFi.status();
    
    if (s == 0 && millis() > (lastConnectTry + 60000) ) {
      /* If WLAN disconnected and idle try to connect */
      /* Don't set retry time too low as retry interfere the softAP operation */
      connect = true;
    }
    
    if (status != s) { // WLAN status change
      Serial.print ( "Status: " );
      Serial.println ( s );
      status = s;
      
      if (s == WL_CONNECTED) {
        /* Just connected to WLAN */
        Serial.println ( "" );
        Serial.print ( "Connected to " );
        Serial.println ( wifi_ssid );
        Serial.print ( "IP address: " );
        Serial.println ( WiFi.localIP() );

        // Setup MDNS responder
        if (!MDNS.begin(myHostname)) {
          Serial.println("Error setting up MDNS responder!");
        } else {
          Serial.println("mDNS responder started");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }

        // shutdown Fallback AP if active
        WiFi.softAPdisconnect (true);
         
      } else if (s == WL_NO_SSID_AVAIL) {
        WiFi.disconnect();
      }
    }
}

void ConnectWifi ()
{
    Serial.println("Connecting as wifi client...");
    WiFi.disconnect();
    WiFi.begin ( wifi_ssid, wifi_password );
    int connRes = WiFi.waitForConnectResult();
    Serial.print ( "connRes: " );
    Serial.println ( connRes );
}

void SetupWebServer ()
{
    /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
    server.on("/", handleRoot);
    server.on("/wifi", handleWifi);
    server.on("/wifisave", handleWifiSave);
    server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
    server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
    server.onNotFound ( handleNotFound );
    server.begin(); // Web server start
    
    Serial.println("HTTP server started");
}

void TryLoadWifiCredentials ()
{
    // Load WLAN credentials from network
    ReadWiFiCredentials(); // Read from EEPROM.
    connect = strlen(wifi_ssid) > 0; // Request WLAN connect if there is a SSID
}
