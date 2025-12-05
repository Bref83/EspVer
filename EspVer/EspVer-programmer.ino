#include <Wire.h>
#include <Adafruit_PN532.h>

#define PN532_SDA 8
#define PN532_SCL 9

const char* CARD_ID = "12345678"; // id карты

Adafruit_PN532 nfc(PN532_SDA, PN532_SCL);

void setup(void) {
  Serial.begin(115200);
  
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Ошибка подключения pn532");
    while (1);
  }
  
  nfc.SAMConfig();
  Serial.print("Поднесите карту для записи");
}

void loop(void) {
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("UID карты: ");
    nfc.PrintHex(uid, uidLength);
    
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya)) {
      uint8_t data[16];
      memset(data, 0, 16);
      strncpy((char*)data, CARD_ID, 8);
      
      if (nfc.mifareclassic_WriteDataBlock(4, data)) {
        Serial.print("Карта записана");
      } else {
        Serial.println("Ошибка записи");
      }
    } else {
      Serial.println("Ошибка формата");
    }
    
    delay(1000);
  }
  
  delay(500);
}