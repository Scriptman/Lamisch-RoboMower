#include <EEPROM.h>

int addr = 0+sizeof(wifi_ssid)+sizeof(wifi_password); // First available byte.

void WriteEEPROM (int addrStart, char saveValue)
{
    EEPROM.put(addrStart, saveValue);
}

void SaveWiFiCredentials ()
{
    EEPROM.begin(512);
    EEPROM.put(0, wifi_ssid);
    EEPROM.put(0+sizeof(wifi_ssid), wifi_password);

    char ok[2+1] = "OK";
    EEPROM.put(0+sizeof(wifi_ssid)+sizeof(wifi_password), ok);
    EEPROM.commit();
    EEPROM.end();
}

void ReadWiFiCredentials ()
{
    EEPROM.begin(512);
    EEPROM.get(0, wifi_ssid);
    EEPROM.get(0+sizeof(wifi_ssid), wifi_password);

    char ok[2+1];
    EEPROM.get(0+sizeof(wifi_ssid)+sizeof(wifi_password), ok);
    EEPROM.end();

    if (String(ok) != String("OK"))
    {
        wifi_ssid[0] = 0;
        wifi_password[0] = 0;
    }
}
