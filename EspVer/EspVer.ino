#include <Wire.h>
#include <Adafruit_PN532.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

#define PN532_SDA 8
#define PN532_SCL 9
#define TARGET_ID "12345678"  // id карты РЕДАКТИРУЕТСЯ В 4 БЛОКЕ mifare classic
#define PASSWORD "113224"   // свой пароль

uint8_t keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // ключи карты

Adafruit_PN532 nfc(PN532_SDA, PN532_SCL);
USBHIDKeyboard Keyboard;

unsigned long lastCardRead = 0;
const unsigned long cardCooldown = 3000;

void setup() {
  USB.begin();
  Keyboard.begin();
  
  Wire.begin(PN532_SDA, PN532_SCL);
  nfc.begin();
  nfc.SAMConfig();
  
  delay(2000);
}

void loop() {
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;

  if (millis() - lastCardRead < cardCooldown) {
    delay(100);
    return;
  }

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
    if (uidLength == 4) {
      if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keyA)) {
        uint8_t data[16];
        
        if (nfc.mifareclassic_ReadDataBlock(4, data)) {
          bool match = true;
          for (int i = 0; i < 8; i++) {
            if (data[i] != TARGET_ID[i]) {
              match = false;
              break;
            }
          }
          
          if (match) {
            Keyboard.write('KEY_DOWN_ARROW'); // открывает окно ввода
            delay(1000);  // время на открытие окна ввода пароля
           
            Keyboard.print(PASSWORD);
            delay(50);
            
            Keyboard.write(KEY_RETURN); 
            
            lastCardRead = millis();
          }
        }
      }
    }
  }
  
  delay(100);
}