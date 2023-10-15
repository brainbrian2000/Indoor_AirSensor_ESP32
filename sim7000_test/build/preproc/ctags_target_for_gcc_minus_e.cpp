# 1 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino"
# 2 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino" 2
# 3 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino" 2
WaterBox_PMU PMU;

# 6 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino" 2




SIM7000 NBIOT;

String DEVICE_ID = "NTU4AQ_00028";
// String DEVICE_ID = "NTU4AQ00RCB";
uint8_t _c = 0;
String _Data;
int sleep_time =5;
// int sleep_time = 700;
bool data_loading=false;
const uint8_t HeadLength = 27;
uint8_t MQTT_Head[HeadLength] = {
  0x10, 0x19, // command, Remaining Packet Length
  0x00, 0x04, // Protocol Length: 4
  (byte)'M', (byte)'Q', (byte)'T', (byte)'T', // Protocol Name:MQTT
  0x04, // Portocol Type
  0x02, // AL
  0x00, 0x3C,
  0x00, 0x0D, // ClientID Lenght: 12
  (byte)'N', (byte)'T', (byte)'U', (byte)'4', (byte)'A', (byte)'Q', (byte)'_',
  (byte)'0', (byte)'0', (byte)'0', (byte)'0', (byte)'0', (byte)random(32, 126) // ClientID: NTU4AQ_00001(R)
};

const uint8_t PayloadLength = 205; // CMD(1,0x30)+RL(2,200)+TopicLength(2)+Topic(28)+payload(170)
uint8_t MQTT_Data[PayloadLength] = {
  0x30, 0xC8, 0x01, // command, Remaining Packet Length
  0x00, 0x1C, // Topic Length
  (byte)'N', (byte)'T', (byte)'U', (byte)'/', (byte)'S', (byte)'m', (byte)'a', // Topic(28):NTU/SmartCampus/NTU4AQ_00001
  (byte)'r', (byte)'t', (byte)'C', (byte)'a', (byte)'m', (byte)'p', (byte)'u',
  (byte)'s', (byte)'/', (byte)'N', (byte)'T', (byte)'U', (byte)'4', (byte)'A',
  (byte)'Q', (byte)'_', (byte)'0', (byte)'0', (byte)'0', (byte)'0', (byte)'1',

  (byte)'|',

  (byte)'B', (byte)'A', (byte)'T', (byte)'_', (byte)'V', (byte)'=', // Message:BAT_V=3.99   @ i40
  (byte)'3', (byte)'.', (byte)'9', (byte)'9', (byte)'|',

  (byte)'B', (byte)'A', (byte)'T', (byte)'_', (byte)'A', (byte)'=', // Message:BAT_A=-100.0 @ i51
  (byte)'-', (byte)'1', (byte)'0', (byte)'0', (byte)'.', (byte)'0', (byte)'|',

  (byte)'W', (byte)'S', (byte)'=', // Message:WS=3.004     @ i64
  (byte)'3', (byte)'.', (byte)'0', (byte)'0', (byte)'4', (byte)'|',

  (byte)'W', (byte)'D', (byte)'=', // Message:WD=345       @ i73
  (byte)'3', (byte)'4', (byte)'0', (byte)'|',

  (byte)'R', (byte)'A', (byte)'I', (byte)'N', (byte)'=', // Message:Rain=500.4   @ i80
  (byte)'5', (byte)'0', (byte)'0', (byte)'.', (byte)'4', (byte)'|',

  (byte)'P', (byte)'M', (byte)'0', (byte)'2', (byte)'5', (byte)'=', // Message:PM025=100.1  @ i91
  (byte)'1', (byte)'0', (byte)'0', (byte)'.', (byte)'1', (byte)'|',

  (byte)'P', (byte)'M', (byte)'1', (byte)'0', (byte)'0', (byte)'=', // Message:PM100=1500.2 @ i103
  (byte)'1', (byte)'5', (byte)'0', (byte)'0', (byte)'.', (byte)'2', (byte)'|',

  (byte)'P', (byte)'M', (byte)'0', (byte)'1', (byte)'0', (byte)'=', // Message:PM010=50.4   @ i116
  (byte)'5', (byte)'0', (byte)'.', (byte)'4', (byte)'|',

  (byte)'T', (byte)'E', (byte)'M', (byte)'P', (byte)'=', // Message:TEMP=34.14   @ i127
  (byte)'3', (byte)'4', (byte)'.', (byte)'1', (byte)'4', (byte)'|',

  (byte)'H', (byte)'U', (byte)'M', (byte)'=', // Message:HUM=100.0    @ i138
  (byte)'1', (byte)'0', (byte)'0', (byte)'.', (byte)'0', (byte)'|',

  (byte)'L', (byte)'U', (byte)'X', (byte)'=', // Message:LUX=100000   @ i148
  (byte)'1', (byte)'0', (byte)'0', (byte)'0', (byte)'0', (byte)'0', (byte)'|',

  (byte)'k', (byte)'P', (byte)'a', (byte)'=', // Message:kPa=111.001  @ i159
  (byte)'1', (byte)'1', (byte)'1', (byte)'.', (byte)'0', (byte)'0', (byte)'1', (byte)'|',

  (byte)'l', (byte)'a', (byte)'t', (byte)'=', // Message:lat=25.123456 @ i171
  (byte)'2', (byte)'5', (byte)'.', (byte)'1', (byte)'2', (byte)'3', (byte)'4', (byte)'5', (byte)'6', (byte)'|',

  (byte)'l', (byte)'o', (byte)'n', (byte)'=', // Message:lon=121.123456 @ i186
  (byte)'1', (byte)'2', (byte)'1', (byte)'.', (byte)'1', (byte)'2', (byte)'3', (byte)'4', (byte)'5', (byte)'6', (byte)'|',

  (byte)'d', (byte)'B', (byte)'A', (byte)'=', // Message:dBA=100.04  @ i200
  (byte)'1', (byte)'0', (byte)'0', (byte)'.', (byte)'0', (byte)'4', (byte)'|'
};


/*

   BAT_V=3.99|      // 數據長度 4

   BAT_A=-100.0|     // 數據長度 6

   WS=3.004|        // 數據長度 5

   WD=340|          // 數據長度 3

   RAIN=500.4|      // 數據長度 5

   PM025=100.1|     // 數據長度 5

   PM105=1500.2|    // 數據長度 6

   PM010=50.4|      // 數據長度 4

   TEMP=34.14|      // 數據長度 5

   HUM=100.0|       // 數據長度 5

   LUX=100000|      // 數據長度 6

   kPa=111.001|     // 數據長度 7

   lat=121.123456|  // 數據長度 10

   lon=25.123456|   // 數據長度 8

   dBA=100.04|      // 數據長度 6

*/
# 108 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino"
struct MaxSize {
  uint8_t BAT_V; // 數據長度 4, BAT_V=3.99
  uint8_t BAT_A; // 數據長度 6, BAT_A=-100.0
  uint8_t WS; // 數據長度 5, WS=3.004
  uint8_t WD; // 數據長度 3, WD=340
  uint8_t RAIN; // 數據長度 5, RAIN=500.4
  uint8_t PM025; // 數據長度 5, PM025=100.1
  uint8_t PM100; // 數據長度 6, PM100=1500.2
  uint8_t PM010; // 數據長度 4, PM010=50.4
  uint8_t TEMP; // 數據長度 5, TEMP=34.14
  uint8_t HUM; // 數據長度 5, HUM=100.0
  uint8_t LUX; // 數據長度 6, LUX=100000
  uint8_t kPa; // 數據長度 6, kPa=111001
  uint8_t lat; // 數據長度 10, lat=25.123456
  uint8_t lon; // 數據長度 9, lon=121.123456
  uint8_t dBA; // 數據長度 6, dBA=100.04
};

MaxSize pSize = {
  .BAT_V = 4, // 數據長度 4, BAT_V=3.99
  .BAT_A = 6, // 數據長度 6, BAT_A=-100.0
  .WS = 5, // 數據長度 5, WS=3.004
  .WD = 3, // 數據長度 3, WD=340
  .RAIN = 5, // 數據長度 5, RAIN=500.4
  .PM025 = 5, // 數據長度 5, PM025=100.1
  .PM100 = 6, // 數據長度 6, PM100=1500.2
  .PM010 = 4, // 數據長度 4, PM010=50.4
  .TEMP = 5, // 數據長度 5, TEMP=34.14
  .HUM = 5, // 數據長度 5, HUM=100.0
  .LUX = 6, // 數據長度 6, LUX=100000
  .kPa = 7, // 數據長度 6, kPa=111.001
  .lat = 9, // 數據長度 10, lat=25.123456
  .lon = 10, // 數據長度 9, lon=121.123456
  .dBA = 6, // 數據長度 6, dBA=100.04
};

struct PayloadIndex {
  uint16_t BAT_V; // 數據長度 4, BAT_V=3.99
  uint16_t BAT_A; // 數據長度 6, BAT_A=-100.0
  uint16_t WS; // 數據長度 5, WS=3.004
  uint16_t WD; // 數據長度 3, WD=340
  uint16_t RAIN; // 數據長度 5, RAIN=500.4
  uint16_t PM025; // 數據長度 5, PM025=100.1
  uint16_t PM100; // 數據長度 6, PM100=1500.2
  uint16_t PM010; // 數據長度 4, PM010=50.4
  uint16_t TEMP; // 數據長度 5, TEMP=34.14
  uint16_t HUM; // 數據長度 5, HUM=100.0
  uint16_t LUX; // 數據長度 6, LUX=100000
  uint16_t kPa; // 數據長度 6, kPa=111001
  uint16_t lat; // 數據長度 10, lat=25.123456
  uint16_t lon; // 數據長度 9, lon=121.123456
  uint16_t dBA; // 數據長度 6, dBA=100.04
};

PayloadIndex pIndex = {
  .BAT_V = 40, // 數據長度 4, BAT_V=3.99
  .BAT_A = 51, // 數據長度 6, BAT_A=-100.0
  .WS = 61, // 數據長度 5, WS=3.004
  .WD = 70, // 數據長度 3, WD=340
  .RAIN = 79, // 數據長度 5, RAIN=500.4
  .PM025 = 91, // 數據長度 5, PM025=100.1
  .PM100 = 103, // 數據長度 6, PM100=1500.2
  .PM010 = 116, // 數據長度 4, PM010=50.4
  .TEMP = 126, // 數據長度 5, TEMP=34.14
  .HUM = 136, // 數據長度 5, HUM=100.0
  .LUX = 146, // 數據長度 6, LUX=100000
  .kPa = 157, // 數據長度 6, kPa=111001
  .lat = 169, // 數據長度 10, lat=25.123456
  .lon = 183, // 數據長度 9, lon=121.123456
  .dBA = 198, // 數據長度 6, dBA=100.04
};


char DATABUFFER[13]; // 用來存放各項目數值

void updateStringToHexArray(uint8_t *_array, uint16_t _index, String _str, uint8_t _strLengthLimit)
{

  if (_str.length() == 0) _str = "-0.";

  //  Serial.print(_str.length());
  //  Serial.print("-");
  //  Serial.println(_strLengthLimit);

  if (_str.length() > _strLengthLimit)
  {
    Serial.print("String length ERROR: ");
    Serial.print(_str.length());
    Serial.print(">");
    Serial.println(_strLengthLimit);
  }
  else {
    // 遇到沒有資料的時候塞 -0.0
    //把長度補完
    for (int _i = 0; _i < _str.length() - _strLengthLimit; _i++)
    {
      _str += "0";
    }
    Serial.println(_str);

  }
  if (_strLengthLimit > 12) Serial.println("ERROR the String length limit must <= 13");



  _str.toCharArray(DATABUFFER, _strLengthLimit + 1);
  for (int _j = 0; _j < _strLengthLimit; _j++)
  {
    _array[_index + _j] = (uint8_t)DATABUFFER[_j];
  }
}

String ToHexStr(byte _byte, uint8_t _show = false)
{
  String _buffer;

  if (byte(_byte) < 16) _buffer = "0";
  _buffer += String(byte(_byte), 16);

  _buffer.toUpperCase();

  if (_show) {
    Serial.print("0x" + _buffer);
  }

  return _buffer;
}

void showByteInChar(uint8_t *_array, int _length)
{
  for (int _i = 0; _i < _length; _i++) {
    Serial.print((char)_array[_i]);
  }
  Serial.println();
  Serial.flush();
}


void setup()
{
  Serial.begin(9600);

  NBIOT.begin(9600);
  NBIOT.setDebuger(Serial);
  NBIOT.init(5);

  PMU.init(6);
  PMU.setDebuger(Serial);
  PMU.setSleepSec(sleep_time); // 900-132-60-10
  PMU.setWakeUpVolate(0);//3.2
  PMU.PowerSaveMode(PMU.ON);
  PMU.setINA219(0x40);

  // 初始化機器資料

  updateStringToHexArray(MQTT_Head, 14, DEVICE_ID, 12); // 更新 MQTT的資料欄位(Head部分)
  updateStringToHexArray(MQTT_Data, 21, DEVICE_ID, 12); // 更新 MQTT的資料欄位(Data部分)
  //  showByteInChar(MQTT_Head, HeadLength);
  //  showByteInChar(MQTT_Data, PayloadLength);

  Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 268 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 268 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino"
                "[System] Setup Done"
# 268 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino" 3
                ); &__c[0];}))
# 268 "e:\\.NTU\\IPCS\\NTU4AQ\\sim7000_test\\PMU_AllSystem_V1.2.2\\PMU_AllSystem_V1.2.2.ino"
                )));

  PMU.Sleep();
}

void loop()
{
  //PMU.ControlPower(PMU.ON);
  //PMU.getBetteryState();

  if (PMU.state == PMU.MASTER)
  {
    _c++;

    PMU.getBetteryState();
    PMU.ControlPower(PMU.OFF);

    NBIOT.ON(true);
    NBIOT.AT_Test();

//      NBIOT.getGPS();
//      Serial.println("[NBIOT] GPS TimeTag " + NBIOT.GPSTimeTag);  // GPS 上的時間戳

    // 檢查是否要更新GPS
    if (_c > 48) // 每48次就更新一次ＧＰＳ
    {
      Serial.println("[NBIOT] Reflash GPS");
      NBIOT.getGPS();
      Serial.println("[NBIOT] GPS TimeTag " + NBIOT.GPSTimeTag); // GPS 上的時間戳
      _c = 0;
    }

    NBIOT.OFF();
    NBIOT.ON(true);
    NBIOT.AT_Test();
    if(data_loading==true){
    }

    updateStringToHexArray(MQTT_Data, pIndex.BAT_V, PMU.BAT_V, pSize.BAT_V); // BAT_V
    updateStringToHexArray(MQTT_Data, pIndex.BAT_A, PMU.BAT_A, pSize.BAT_A); // BAT_A
    updateStringToHexArray(MQTT_Data, pIndex.WS, PMU.WS, pSize.WS); // WS
    updateStringToHexArray(MQTT_Data, pIndex.WD, PMU.WD, pSize.WD); // WD
    updateStringToHexArray(MQTT_Data, pIndex.RAIN, PMU.RAIN, pSize.RAIN); // RAIN
    updateStringToHexArray(MQTT_Data, pIndex.PM025, PMU.PM025, pSize.PM025); // PM025
    updateStringToHexArray(MQTT_Data, pIndex.PM100, PMU.PM100, pSize.PM100); // PM100
    updateStringToHexArray(MQTT_Data, pIndex.PM010, PMU.PM010, pSize.PM010); // PM010
    updateStringToHexArray(MQTT_Data, pIndex.TEMP, PMU.TEMP, pSize.TEMP); // TEMP
    updateStringToHexArray(MQTT_Data, pIndex.HUM, PMU.HUM, pSize.HUM); // HUM
    updateStringToHexArray(MQTT_Data, pIndex.LUX, PMU.LUX, pSize.LUX); // LUX
    updateStringToHexArray(MQTT_Data, pIndex.kPa, PMU.kPa, pSize.kPa); // kPa
    updateStringToHexArray(MQTT_Data, pIndex.lat, NBIOT.Latitude, pSize.lat); // lat
    updateStringToHexArray(MQTT_Data, pIndex.lon, NBIOT.Longitude, pSize.lon); // lon
    updateStringToHexArray(MQTT_Data, pIndex.dBA, PMU.dBA, pSize.dBA); // dBA



    Serial.println(NBIOT.Longitude);
    Serial.println(PMU.dBA);
    showByteInChar(MQTT_Data, PayloadLength);

    // 開啟TCP連線
    //    NBIOT.openNetwork("35.194.169.153", 1883);
    bool debuging_iot = 0;
    if (debuging_iot==1){
      for (int times = 0; times<5;times++){
        NBIOT.openNetwork("scplus-iot.ipcs.ntu.edu.tw", 1883);
      }
    }

    NBIOT.openNetwork("scplus-iot.ipcs.ntu.edu.tw", 1883);

    delay(10000);

    NBIOT.AT_CMD("AT+CIPSEND", true);

    Serial.println("[ >> ] Send Data");
    for (uint8_t _i = 0; _i < HeadLength; _i++)
    {
      if (_i % 10 == 0) Serial.println();
      NBIOT.AT_print(ToHexStr(MQTT_Head[_i], false));
      NBIOT.AT_print(" ");
      //      Serial.print(" ");
      delay(5);
    }
    Serial.println();

    for (uint8_t _i = 0; _i < PayloadLength; _i++)
    {
      if (_i % 10 == 0) Serial.println();
      NBIOT.AT_print(ToHexStr(MQTT_Data[_i], false));
      NBIOT.AT_print(" ");
      //      Serial.print(" ");
      delay(5);
    }

    NBIOT.AT_end();
    Serial.println("Ctrl + Z");
    delay(2000);

    NBIOT.closeNetwork();

    NBIOT.OFF();

    PMU.Sleep();
  }
  else {
    if (PMU.state == PMU.SLAVER) {
      Serial.println("[I2C] I2C->slaver");
      delay(5000);
    }

    if (PMU.state == PMU.MASTER) {
      Serial.println("[I2C] I2C->master");
      delay(500);
    }
  }

}
