/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example to change UID of changeable MIFARE card.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * This sample shows how to set the UID on a UID changeable MIFARE card.
 * 
 * @author Tom Clement
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   0     // Configurable, see typical pin layout above
#define SS_PIN    5    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

/* Set your new UID here! */
// #define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF} 
// D2 E7 E7 30
#define NEW_UID {0x65, 0xE6, 0xD2, 0x3C}
byte nuid_new[4] = {0, 0, 0, 0};
byte storage_nuid[4] = {0, 0, 0, 0};
bool state = 0; // state 0 ->read mode

MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  Serial.println(F("Warning: this example overwrites the UID of your UID changeable card, use with care!"));
  
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

// Setting the UID can be as simple as this:
//void loop() {
//  byte newUid[] = NEW_UID;
//  if ( mfrc522.MIFARE_SetUid(newUid, (byte)4, true) ) {
//    Serial.println("Wrote new UID to card.");
//  }
//  delay(1000);
//}
int count = 0;
bool card_same = 0;
// But of course this is a more proper approach
void loop() {
  // Serial.println("Reading...");
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle. And if present, select one.
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  
  // Now a card is selected. The UID and SAK is in mfrc522.uid.
  
  // Dump UID
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
  Serial.println();
  if(!state){
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      storage_nuid[i] = mfrc522.uid.uidByte[i];
    } 
    Serial.println();
    Serial.println("It read state of module");
    
    Serial.print(F("Storaging Card UID :"));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(storage_nuid[i] < 0x10 ? " 0" : " ");
      Serial.print(storage_nuid[i], HEX);
    }
    Serial.println();
    
    Serial.println("=====READ MODE DONE=====");
    mfrc522.PICC_HaltA();
    state = 1;
    return;
  }
  
  {
      
    /*
    // Dump PICC type
    //  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    //  Serial.print(F("PICC type: "));
    //  Serial.print(mfrc522.PICC_GetTypeName(piccType));
    //  Serial.print(F(" (SAK "));
    //  Serial.print(mfrc522.uid.sak);
    //  Serial.print(")\r\n");
    //  if (  piccType != MFRC522::PICC_TYPE_MIFARE_MINI 
    //    &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
    //    &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    //    Serial.println(F("This sample only works with MIFARE Classic cards."));
    //    return;
    //  }

    // Set new UID

    // byte newUid[] = NEW_UID;
    */
  }

  card_same = 1;
  Serial.print(F("Storage Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(storage_nuid[i] < 0x10 ? " 0" : " ");
    Serial.print(storage_nuid[i], HEX);
  }
  Serial.println();



  if(state){
    // if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    //   return;
    // }
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      nuid_new[i] = mfrc522.uid.uidByte[i];
    }
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if(nuid_new[i] != storage_nuid[i]){
        card_same = 0;
        break;
      }        
    }

    
    if(card_same && count <5){
        state = 1;
        count++;
        Serial.println("Try new one UID card of Mifare card, card ID is same with Storage ID.");
        Serial.print("Write Fail times: ");
        Serial.print(count);
        Serial.println(", max time = 5");
        Serial.println();
    } else if (count >=5 ){
        Serial.println("Try new one UID card of Mifare card.");
        Serial.println("Back into read mode");
        count = 0;
        state = 0;
    } else{
      if (!mfrc522.MIFARE_SetUid(storage_nuid, (byte)4, true) ) {
        if(count<=5 | card_same){
          state = 1;
          count++;
          Serial.println("Try new one UID card of Mifare card.");
          Serial.print("Write Fail times: ");
          Serial.print(count);
          Serial.println(", max time = 5");
          Serial.println();
          return;
        }else{
          Serial.println("Try new one UID card of Mifare card.");
          Serial.println("Back into read mode");
          count = 0;
          state = 0;
        }
      }else{
        Serial.println(F("Wrote new UID to card."));
        Serial.println();
        state = 0;
        // return;
      }
    }   
    // Halt PICC and re-select it so DumpToSerial doesn't get confused
    // if(card_same == 1){
    //   // Serial.println();
    //   Serial.println("Same card, back into read mode, try new one card.");
    //   // Serial.println();
    //   state = 0;
    //   return;
    // }



    mfrc522.PICC_HaltA();
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      return;
    }
    
    // Dump the new memory contents
    Serial.println(F("New UID and contents:"));
    // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
  }
  delay(2000);
}
