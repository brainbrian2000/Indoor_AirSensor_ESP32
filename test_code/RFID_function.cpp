// #include <Arduino.h>
// #include <SPI.h>
// #include <SPI.h>
#ifndef _RFID_FUNCTION_CPP
#define _RFID_FUNCTION_CPP

    #include <MFRC522.h>
    #define RST_PIN   0     // Configurable, see typical pin layout above
    #define SS_PIN    5    // Configurable, see typical pin layout above


    /* Set your new UID here! */
    // #define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF} 
    // D2 E7 E7 30
    #define NEW_UID {0x65, 0xE6, 0xD2, 0x3C}

    // MFRC522::MIFARE_Key key;

    class RFID_module{
        private:
            byte buffer[18]={0};
            byte size = sizeof(buffer);
            MFRC522::StatusCode status;
            MFRC522::MIFARE_Key A_key;
            MFRC522::MIFARE_Key B_key;
            MFRC522::MIFARE_Key update_key;
            // byte trailerBlock = 7;


        public:
            MFRC522 mfrc522 = MFRC522(SS_PIN, RST_PIN);   // Create MFRC522 instance
            byte storage_nuid[4] = {0, 0, 0, 0};
            byte rfid_dump_arr[16][4][18] = {0};
            RFID_module(){};
            ~RFID_module(){};
            void start(){
                // SPI.begin();         // Init SPI bus
                mfrc522.PCD_Init();  // Init MFRC522 card
                // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
                for (byte i = 0; i < 6; i++) {
                    // key.keyByte[i] = 0xFF;
                    A_key.keyByte[i] = 0xFF;
                    B_key.keyByte[i] = 0xFF;
                    update_key.keyByte[i] = 0xFF;
                }
            };
            void powerup(){
                mfrc522.PCD_SoftPowerUp();
                mfrc522.PCD_AntennaOn();
            }
            void powerdown(){
                mfrc522.PCD_SoftPowerDown();
                mfrc522.PCD_AntennaOff();
            }


            bool scan(){
                if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
                    delay(50);
                    return false;
                }
                return true;
            };
            void halt(){
                mfrc522.PCD_StopCrypto1();
                mfrc522.PICC_HaltA();
                return;
            };
            void dump(){
                mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
                halt();
            };
            bool Authenticate_A( byte trailerBlock){
                status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &A_key, &(mfrc522.uid));
                if (status != MFRC522::STATUS_OK) {
                    Serial.print(F("PCD_Authenticate_Type_A() failed: "));
                    Serial.println(mfrc522.GetStatusCodeName(status));
                    return false;
                }
                return true;
            };
            bool Authenticate_B( byte trailerBlock){
                status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &B_key, &(mfrc522.uid));
                if (status != MFRC522::STATUS_OK) {
                    Serial.print(F("PCD_Authenticate_Type_B() failed: "));
                    Serial.println(mfrc522.GetStatusCodeName(status));
                    return false;
                }
                return true;
            };
            bool write( byte blockNum, byte *_blockData, byte size=16){
                status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockNum, _blockData, size);
                // 若寫入不成功…
                if (status != MFRC522::STATUS_OK) {
                    // 顯示錯誤訊息
                    Serial.print(F("MIFARE_Write() failed: "));
                    Serial.println(mfrc522.GetStatusCodeName(status));
                    return false;
                }
                return true;
            };
            void writeBlock_safe( byte _sector, byte _block, byte _blockData[]) {
                if (_sector < 0 || _sector > 15 || _block < 0 || _block > 3) {
                    // 顯示「區段或區塊碼錯誤」，然後結束函式。
                    Serial.println(F("Wrong sector or block number."));
                    return;
                }
                if (_block == 3){
                    Serial.println(F("Write not safe ->try to write Key A/B/access bit by using writeBlock_unsafe."));
                    return;
                }
                if (_sector == 0 && _block == 0) {
                    // 顯示「第一個區塊只能讀取」，然後結束函式。
                    Serial.println(F("First block is read-only for normal UID card. \n-> CUID need another function to write it."));
                    return;
                }

                byte blockNum = _sector * 4 + _block;  // 計算區塊的實際編號（0~63）
                byte trailerBlock = _sector * 4 + 3;   // 控制區塊編號
                bool Authenticated_state = false;
                bool write_state = false;
                
                Authenticated_state = Authenticate_B(trailerBlock);//(MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
                if(!Authenticated_state){
                    Authenticated_state = Authenticate_A(trailerBlock);
                    if(!Authenticated_state){
                        Serial.println("Authenticated_state is not correct. End writeSector_safe.");
                        return;
                    }
                }
                // 在指定區塊寫入16位元組資料
                write_state = write(blockNum,_blockData);
                if(!write_state){
                    Serial.println("Write_state is not correct. End writeBlock_safe.");
                    return;
                }
                // 顯示「寫入成功！」
                Serial.println("Data was written @ sector "+String(_sector)+" block "+String(_block));
            }
            void writeSector_safe( byte sector, byte **data){
                if(sector<1||sector >15){
                    Serial.println("Sector error.");
                    return;
                }
                byte blockNum = sector*4;
                byte trailerBlock = sector*4+3;
                bool Authenticated_state = false;
                bool write_state = false;
                
                Authenticated_state = Authenticate_B(trailerBlock);//(MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
                if(!Authenticated_state){
                    Authenticated_state = Authenticate_A(trailerBlock);
                    if(!Authenticated_state){
                        Serial.println("Authenticated_state is not correct. End writeSector_safe.");
                        return;
                    }
                }
                // 在指定區塊寫入16位元組資料
                write_state = write(blockNum,data[0]);
                write_state = write(blockNum+1,data[1]) | write_state;
                write_state = write(blockNum+2,data[2]) | write_state;
                if(!write_state){
                    Serial.println("Write_state is not correct. End writeBlock_safe.");
                    return;
                }
                // 顯示「寫入成功！」
                Serial.println("Data was written @ sector "+String(sector));

                
            };


            void writeBlock_unsafe( byte _sector, byte _block, byte _blockData[]) {
                if (_sector < 0 || _sector > 15 || _block < 0 || _block > 3) {
                    // 顯示「區段或區塊碼錯誤」，然後結束函式。
                    Serial.println(F("Wrong sector or block number."));
                    return;
                }
                if (_sector == 0 && _block == 0) {
                    // 顯示「第一個區塊只能讀取」，然後結束函式。
                    Serial.println(F("First block is read-only for normal UID card. \n-> CUID need another function to write it."));
                    return;
                }

                byte blockNum = _sector * 4 + _block;  // 計算區塊的實際編號（0~63）
                byte trailerBlock = _sector * 4 + 3;   // 控制區塊編號
                bool Authenticated_state = false;
                bool write_state = false;
                
                Authenticated_state = Authenticate_A(trailerBlock);//(MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
                if(!Authenticated_state){
                    Serial.println("Authenticated_state is not correct. End writeBlock_safe.");
                    return;
                }
                // 在指定區塊寫入16位元組資料
                write_state = write(blockNum,_blockData);
                if(!write_state){
                    Serial.println("Write_state is not correct. End writeBlock_safe.");
                    return;
                }
                // 顯示「寫入成功！」
                Serial.println("Data was written @ sector "+String(_sector)+" block "+String(_block));
            }
            void set_key_A( byte *arr){
                /** 
                 * input array of len = 6 and using type byte
                */
                for (byte i = 0; i < 6 ;i++){
                    A_key.keyByte[i] = arr[i];
                }
            };
            void set_key_B( byte *arr){
                /** 
                 * input array of len = 6 and using type byte
                */
                for (byte i = 0; i < 6 ;i++){
                    B_key.keyByte[i] = arr[i];
                }
            };
            void set_key_update( byte  *arr){
                /** 
                 * input array of len = 6 and using type byte
                */
                for (byte i = 0; i < 6 ;i++){
                    update_key.keyByte[i] = arr[i];
                }
            };
            bool read( byte blockNum){
                status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum,buffer,&size);
                if (status != MFRC522::STATUS_OK) {
                    // 顯示錯誤訊息
                    Serial.print(F("MIFARE_Read() failed: "));
                    Serial.println(mfrc522.GetStatusCodeName(status));
                    return false;
                }
                return true;
            };
            void readBlock( byte _sector, byte _block){
                if (_sector < 0 || _sector > 15 || _block < 0 || _block > 3) {
                    // 顯示「區段或區塊碼錯誤」，然後結束函式。
                    Serial.println(F("Wrong sector or block number."));
                    return;
                }

                byte blockNum = _sector * 4 + _block;  // 計算區塊的實際編號（0~63）
                byte trailerBlock = _sector * 4 + 3;   // 控制區塊編號
                bool Authenticated_state = false;
                bool read_state = false;
                
                Authenticated_state = Authenticate_A(trailerBlock);//(MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
                if(!Authenticated_state){
                    Authenticated_state = Authenticate_B(trailerBlock);// Authenticate with key B.
                }
                read_state = read(blockNum);
                if(!read_state){
                    Serial.println("read_state is not correct. End readBlock.");
                    return;
                }
                Serial.println(F("Data was read. print out at here"));
                for(int i = 0; i< 16;i++){
                    Serial.print(buffer[i]<0x10?" 0": " ");
                    Serial.print(buffer[i],HEX);
                    Serial.print(" ");
                }
                Serial.println();
                return;
            }
            void readSector( byte _sector, byte arr[4][18]){
                    bool Authenticated_state = false;
                    bool read_state = false;
                    byte trailerBlock = _sector * 4 + 3;   // 控制區塊編號
                    Authenticated_state = Authenticate_A(trailerBlock);//(MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
                    if(!Authenticated_state){
                        Authenticated_state = Authenticate_B(trailerBlock);// Authenticate with key B.
                    }
                    byte blockNum = _sector * 4;  // 計算區塊的實際編號（0~63）
                    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum,arr[0],&size);
                    if (status != MFRC522::STATUS_OK) {
                        // 顯示錯誤訊息
                        Serial.print(F("MIFARE_Read() failed: "));
                        Serial.println(mfrc522.GetStatusCodeName(status));
                    }
                    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum+1,arr[1],&size);
                    if (status != MFRC522::STATUS_OK) {
                        // 顯示錯誤訊息
                        Serial.print(F("MIFARE_Read() failed: "));
                        Serial.println(mfrc522.GetStatusCodeName(status));
                    }
                    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum+2,arr[2],&size);
                    if (status != MFRC522::STATUS_OK) {
                        // 顯示錯誤訊息
                        Serial.print(F("MIFARE_Read() failed: "));
                        Serial.println(mfrc522.GetStatusCodeName(status));
                    }
                    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum+3,arr[3],&size);
                    if (status != MFRC522::STATUS_OK) {
                        // 顯示錯誤訊息
                        Serial.print(F("MIFARE_Read() failed: "));
                        Serial.println(mfrc522.GetStatusCodeName(status));
                    }
            };
            void readAllBlock(){
                for (int _sector=0; _sector<16;_sector++){
                    bool Authenticated_state = false;
                    bool read_state = false;
                    byte trailerBlock = _sector * 4 + 3;   // 控制區塊編號
                    Authenticated_state = Authenticate_A(trailerBlock);//(MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
                    if(!Authenticated_state){
                        Authenticated_state = Authenticate_B(trailerBlock);// Authenticate with key B.
                    }
                    byte blockNum = _sector * 4;  // 計算區塊的實際編號（0~63）
                    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum,rfid_dump_arr[_sector][0],&size);
                    if (status != MFRC522::STATUS_OK) {
                        // 顯示錯誤訊息
                        Serial.print(F("MIFARE_Read() failed: "));
                        Serial.println(mfrc522.GetStatusCodeName(status));
                    }
                    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum+1,rfid_dump_arr[_sector][1],&size);
                    if (status != MFRC522::STATUS_OK) {
                        // 顯示錯誤訊息
                        Serial.print(F("MIFARE_Read() failed: "));
                        Serial.println(mfrc522.GetStatusCodeName(status));
                    }
                    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum+2,rfid_dump_arr[_sector][2],&size);
                    if (status != MFRC522::STATUS_OK) {
                        // 顯示錯誤訊息
                        Serial.print(F("MIFARE_Read() failed: "));
                        Serial.println(mfrc522.GetStatusCodeName(status));
                    }
                    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum+3,rfid_dump_arr[_sector][3],&size);
                    if (status != MFRC522::STATUS_OK) {
                        // 顯示錯誤訊息
                        Serial.print(F("MIFARE_Read() failed: "));
                        Serial.println(mfrc522.GetStatusCodeName(status));
                    }
                    // for (int _block = 0;_block<4;_block++){
                    //     // read_state = read(blockNum);
                    //     status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum,rfid_dump_arr[_sector][_block],&size);
                    // if (status != MFRC522::STATUS_OK) {
                    //     // 顯示錯誤訊息
                    //     Serial.print(F("MIFARE_Read() failed: "));
                    //     Serial.println(mfrc522.GetStatusCodeName(status));
                    // }
                    // }
                }
                return;
            }
            void RFIDDump(bool char_output = false){
                unsigned int time;
                time = millis();
                readAllBlock();
                Serial.println("Time cost :"+String(millis()-time));
                if(!char_output){
                    for (int _sector=0; _sector<16;_sector++){
                        for (int _block = 0;_block<4;_block++){
                            for(int i = 0; i< 18;i++){
                                Serial.print(rfid_dump_arr[_sector][_block][i]<0x10?" 0": " ");
                                Serial.print(rfid_dump_arr[_sector][_block][i],HEX);
                            }
                            Serial.print("\n");
                        }
                    } 
                }else{
                    for (int _sector=0; _sector<16;_sector++){
                        for (int _block = 0;_block<3;_block++){
                            for(int i = 0; i< 16;i++){
                                if(rfid_dump_arr[_sector][_block][i]!=0x00){
                                    Serial.print(char(rfid_dump_arr[_sector][_block][i]));
                                }else{
                                    Serial.print(' ');
                                }
                            }
                            Serial.print("\n");
                        }
                    } 
                }
            };
            void sectorDump(byte _sector,bool char_output=true){
                byte arr[4][18];
                readSector(_sector,arr);
                Serial.println("sectorDump:");
                if(!char_output){
                    for(int _block = 0;_block<4;_block++){
                        for(int i = 0; i< 18;i++){
                            Serial.print(arr[_block][i]<0x10?" 0": " ");
                            Serial.print(arr[_block][i],HEX);
                        }
                        Serial.print("\n");
                    }
                }
                else if(char_output){
                    for(int _block = 0;_block<3;_block++){
                        for(int i = 0; i< 16;i++){
                            if(arr[_block][i] != 0x00){
                                Serial.print(char(arr[_block][i]));
                            }else{
                                Serial.print(' ');
                            }
                        }
                        Serial.print("\n");
                    }
                    Serial.println(F("Do not print block 3[key A & B]"));
                };
            }
            /**
             * @brief Write c-str to RFID card, the maximum length of string is 48(include '\0'). 
             * 
             * @param _sector the target sector, recommand to avoid using 0/1/2(for UID, WiFi, WiFi)
             * @param str Maximum 48
             */
            void WriteStrToSector(byte _sector, const char *str){
                int len = strlen(str);
                if(len>48){
                    Serial.println("Too long string. Return");
                }

                byte data_sec_1[3][16] = {0};
                for(int i=0;i<16;i++){
                    if(!(i<len)){
                        break;  
                    }
                    data_sec_1[0][i] = byte(str[i]);
                }
                for(int i=0;i<16;i++){
                    if(!(i<len)){
                        break;  
                    }
                    data_sec_1[1][i] = byte(str[i+16]);
                }
                for(int i=0;i<16;i++){
                    if(!(i<len)){
                        break;  
                    }
                    data_sec_1[2][i] = byte(str[i+32]);
                }
                byte *temp_p1[3] = {data_sec_1[0],data_sec_1[1],data_sec_1[2]};
                writeSector_safe(_sector,temp_p1);
                return;
            }
            char* ReadStrFromSector(byte _sector){
                byte data[4][18]={0};
                char str[48];
                readSector(_sector,data);
                for(int j = 0;j<3;j++){
                    for(int i = 0;i<16;i++){
                        str[j*16+i] = char(data[j][i]);
                    }
                }
                Serial.println("Read "+String(str));
                return str;
            }
            // WiFi Setting 
            /**
             * @brief Set Wifi setting into RFID 
             * 
             * @param ssid 31byte
             * @param passwd 32byte
             * @return * void 
             * for ssid   set in sector 1 block 0[1:15], block1[0:15] ->31byte
             * for passwd set in sector 2 block 1[0:15], block2[0:15] ->32byte
             */
            void writeWiFi_RFID(char  * ssid,char  * passwd){
                int ssid_len = strlen(ssid);
                int passwd_len = strlen(passwd);
                if(ssid_len>31 || passwd_len >32){
                    Serial.println(F("Too Long cannot set in RFID, return"));
                }
                byte data_sec_1[3][16] = {0};
                byte data_sec_2[3][16] = {0};
                //B key update->storage part, using setting wifi rfid section key
                for(int i=0;i<16;i++){
                    if(!(i<ssid_len)){
                        break;  
                    }
                    data_sec_1[0][i] = byte(ssid[i]);
                }
                for(int i=0;i<15;i++){
                    if(!(i+15<ssid_len)){
                        break;
                    }
                    data_sec_1[1][i] = byte(ssid[i+16]);
                }
                data_sec_1[1][15] = '1'; // This is the tag for WiFi type
                //userid part
                for(int i=0;i<16;i++){
                    data_sec_1[2][i] = 0x00;
                }
                for(int i=0;i<16;i++){
                    data_sec_2[0][i] = 0x00;
                }


                for(int i=0;i<16;i++){
                    if(!(i<passwd_len)){
                        break;
                    }
                    data_sec_2[1][i] = byte(passwd[i]);
                }
                for(int i=0;i<16;i++){
                    if(!(i+16<passwd_len)){
                        break;
                    }
                    data_sec_2[2][i] = byte(passwd[i+16]);
                }
                byte *temp_p1[3] = {data_sec_1[0],data_sec_1[1],data_sec_1[2]};
                byte *temp_p2[3] = {data_sec_2[0],data_sec_2[1],data_sec_2[2]};
                writeSector_safe(1,temp_p1);
                writeSector_safe(2,temp_p2);
                //B key set back
            
            };
            /**
             * @brief Set Wifi setting into RFID 
             * 
             * @param ssid 31byte
             * @param userid 32byte
             * @param passwd 32byte
             * for ssid   set in sector 1 block 0[1:15], block1[0:15] ->31byte
             * for userid set in sector 1 block 2[0:15], sector 2 block 0[1:15]->32byte
             * for passwd set in sector 2 block 1[0:15], block2[0:15] ->32byte
             */
            void writeWiFi_RFID(char  * ssid,char  * userid,char  * passwd){
                int ssid_len = strlen(ssid);
                int userid_len = strlen(userid);
                int passwd_len = strlen(passwd);
                if(ssid_len>=31 || passwd_len >=32 || userid_len >=32){
                    Serial.println(F("Too Long cannot set in RFID, return"));
                }
                byte data_sec_1[3][16] = {0};
                byte data_sec_2[3][16] = {0};
                //B key update->storage part, using setting wifi rfid section key
                //ssid part
                for(int i=0;i<16;i++){
                    if(!(i<ssid_len)){
                        break;  
                    }
                    data_sec_1[0][i] = byte(ssid[i]);
                }
                for(int i=0;i<15;i++){
                    if(!(i+15<ssid_len)){
                        break;
                    }
                    data_sec_1[1][i] = byte(ssid[i+16]);
                }
                data_sec_1[1][15] = '2'; // This is the tag for WiFi connection type
                //userid part
                for(int i=0;i<16;i++){
                    if(!(i<userid_len)){
                        break;  
                    }
                    data_sec_1[2][i] = byte(userid[i]);
                }
                for(int i=0;i<16;i++){
                    if(!(i+16<userid_len)){
                        break;  
                    }
                    data_sec_2[0][i] = byte(userid[i+16]);
                }
                // passwd part
                for(int i=0;i<16;i++){
                    if(!(i<passwd_len)){
                        break;
                    }
                    data_sec_2[1][i] = byte(passwd[i]);
                }
                for(int i=0;i<16;i++){
                    if(!(i+16<passwd_len)){
                        break;
                    }
                    data_sec_2[2][i] = byte(passwd[i+16]);
                }
                byte *temp_p1[3] = {data_sec_1[0],data_sec_1[1],data_sec_1[2]};
                byte *temp_p2[3] = {data_sec_2[0],data_sec_2[1],data_sec_2[2]};
                writeSector_safe(1,temp_p1);
                writeSector_safe(2,temp_p2);

                //B key set back
            };

            void ReadWiFi_RFID(){
                char ssid[32]="";
                char userid[32]="";
                char passwd[32]="";
                char WiFitype;
                byte sector1[4][18];
                byte sector2[4][18];
                readSector(1,sector1);
                readSector(2,sector2);
                WiFitype = char(sector1[1][15]);
                // SSID part
                for(int i = 0; i<16;i++){
                    ssid[i] = char(sector1[0][i]);
                }
                for(int i = 0; i<15;i++){
                    ssid[i+16] = char(sector1[1][i]);
                }
                // USERID part
                for(int i = 0; i<16;i++){
                    userid[i] = char(sector1[2][i]);
                }
                for(int i = 0; i<16;i++){
                    userid[i+16] = char(sector2[0][i]);
                }
                // SSID part
                for(int i = 0; i<16;i++){
                    passwd[i] = char(sector2[1][i]);
                }
                for(int i = 0; i<16;i++){
                    passwd[i+16] = char(sector2[2][i]);
                }
                Serial.println(WiFitype);
                Serial.println("SSID   = "+String(ssid));
                Serial.println("USERID = "+String(userid));
                Serial.println("PASSWD = "+String(passwd));
            };
            bool ReadWiFi_RFID(char* ssid, char* userid,char* passwd,char *WiFitype){
                byte sector1[4][18]={0};
                byte sector2[4][18]={0};
                readSector(1,sector1);
                readSector(2,sector2);
                *WiFitype = char(sector1[1][15]);
                // SSID part
                if(sector1[0][0]==0&&sector1[1][0]==0&&sector1[2][0]==0&&sector2[0][0]&&sector2[1][0]&&sector2[2][0]){
                    Serial.println("Read Fail->all '\\0'");
                    return false;
                }
                
                
                for(int i = 0; i<16;i++){
                    ssid[i] = char(sector1[0][i]);
                }
                for(int i = 0; i<15;i++){
                    ssid[i+16] = char(sector1[1][i]);
                }
                // USERID part
                for(int i = 0; i<16;i++){
                    userid[i] = char(sector1[2][i]);
                }
                for(int i = 0; i<16;i++){
                    userid[i+16] = char(sector2[0][i]);
                }
                // SSID part
                for(int i = 0; i<16;i++){
                    passwd[i] = char(sector2[1][i]);
                }
                for(int i = 0; i<16;i++){
                    passwd[i+16] = char(sector2[2][i]);
                }
                Serial.println(WiFitype);
                Serial.println("SSID   = "+String(ssid));
                Serial.println("USERID = "+String(userid));
                Serial.println("PASSWD = "+String(passwd));
                
                return true;
            };

            /**
             * @brief Select the key of the sector [0-15] to be updated.
             * update with A or B (control by is_A).
             * using key stored in update_key. 
             * Have be authenticated by key A
             */




            void UpdateKey(byte _sector,bool is_A=false){
                /**
                 * @brief 
                 * NOT YET
                 */
                // byte blockNum = _sector * 4 + _block;  // 計算區塊的實際編號（0~63）
                byte trailerBlock = _sector * 4 + 3;   // 控制區塊編號
                Authenticate_A(trailerBlock);
                if(!is_A){

                }
            };

            byte uidXOR(){
                byte result;
                result = mfrc522.uid.uidByte[0] ^ mfrc522.uid.uidByte[1] ^ mfrc522.uid.uidByte[2] ^ mfrc522.uid.uidByte[3];
                readBlock(0,0);
                
                for(int i = 0; i< 4;i++){
                    Serial.print(mfrc522.uid.uidByte[i]<0x10?" 0": " ");
                    Serial.print(mfrc522.uid.uidByte[i],HEX);
                    Serial.print(" ");
                }
                Serial.print(result<0x10?" 0": " ");
                Serial.print(result,HEX);
                Serial.println();
                return result;
            }


            void writeBlock_unsafe_cuid(byte _blockData[]) {
                byte _sector = 0;
                byte _block = 0;
                byte blockNum = _sector * 4 + _block;  // 計算區塊的實際編號（0~63）
                byte trailerBlock = _sector * 4 + 3;   // 控制區塊編號
                bool Authenticated_state = false;
                bool write_state = false;
                
                Authenticated_state = Authenticate_A(trailerBlock);//(MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
                if(!Authenticated_state){
                    Serial.println("Authenticated_state is not correct. End writeBlock_safe.");
                    return;
                }
                // 在指定區塊寫入16位元組資料
                write_state = write(blockNum,_blockData);
                if(!write_state){
                    Serial.println("Write_state is not correct. End writeBlock_safe.");
                    return;
                }
                // 顯示「寫入成功！」
                Serial.println("Data was written @ sector "+String(_sector)+" block "+String(_block));
            }

    };
#endif