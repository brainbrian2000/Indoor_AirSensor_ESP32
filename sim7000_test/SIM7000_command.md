# NB-IOT 連接板指令筆記與操作紀錄
```=
Mainly Using SIM7000 series(SIM7000E and SIM7000G), [Made In China]
SIM7000E-Euro
SIM7000G-Global

```
`Defult using 115200,CRLF`
## 工作目的與簡介
```
支援的頻段不同，理論上SIM7000G所支援的頻段涵蓋所有頻段，但可能因為設定問題，因此暫時無法操作
主要要解決訊號的傳遞問題
```
此次主要目的為增加機器傳輸資料的筆數與穩定性<br>初步思考後得出以下可能問題來源：
1. SIM7000G模組無法傳輸資料
2. 解決機器訊號問題
    - 因為大學里基地台很少，訊號較弱，因此資料不容易傳輸回server
3. 解決供電問題
    - 機器電力供應不足，使機器無法開機
        - 太陽能發電不足，陽光不夠
        - SIM7000模組耗電問題
- -> 綜合以上敘述，可能因為實驗場域(大學里、台大校園)訊號爛的徹底，導致每次傳輸訊號(15min)皆須要多次嘗試連線，長時間高功率耗電大，導致訊號打不出來，又耗電

## 重要前置資訊->讓電腦跟SIM7000系列能溝通
`Seiral Port 問題`<br>
`第一步永遠是開啟Serial Monitor去測試連線`
### 第一次無法連線
倘若無法連線，可以透過以下步驟嘗試解決：
1. 有偵測到COM port但是一直顯示無法連線，可能是因為port位置不對，因此可以透過裝置管理員進行管理 
2. Using Windows Powershell
    > ```=
    > [System.IO.Ports.SerialPort]::getportnames()
    > ```
    > 得到跑出目前的port name，應該會有一個莫名其妙沒顯示在Serial monitor，但卻有偵測到的port [COM $\alpha$]<br>
    > 將"SimTech HS-USB AT port 9001"的COM改為該port(COM $\alpha$)，就可以成功連入
此狀況會發生在"有[初次]連線過的SIM7000G上"
### 第n次無法連線
狀況初步估計原因：
`因為Serial無法正確關閉，因此一直不斷地佔用某一個Port，且使連進後因為占用SIM7000的port而無法正確溝通`
#### Solution：
1. 開啟裝置管理員，找到AT Port
2. Using Windows Powershell
    > ```=
    > [System.IO.Ports.SerialPort]::getportnames()
    > ```
    > 得到跑出目前的port name，可能只有四個(一個，若有手動停用其他port)port<br>
3. 手動將AT的Port name改為期現在以外的COM編號，並拔除USB線，等待一小段時間(幾秒即可)
4. 再插入USB，並使用Powershell 輸入<br>
    > `[System.IO.Ports.SerialPort]::getportnames()`
    應該會得到一個空的，沒有顯示於裝置管理員的COM編號
5. 再從裝置管理員把AT的編號改為那個只在powershell顯示而沒有在裝置管理員顯示的port
6. `重開Serial monitor，並記得調整參數`



## SIM7000G相關指令與操作筆記
中華電信NB-IoT 是走Band 8，頻段為900MHz，Cat M1 是走Band 3，頻段為1800MHz
- 基本指令玩法與組合規則
```=
1. type [A]
    不可調整指令與回報


AT+[]
     單純指令，倘若非互動式或調整式，則會跑出結果
     若為調整內容，則會回報error
```
```=
2. type [B]
    可調整數值之指令


AT+[]?
    現在系統狀態
AT+[]=?
    系統有哪些狀態能設定，以及設定參數
AT+[]=<>[,<>,]
    系統參數設定  
```
### 查詢可用頻段
#### [B]
```=
AT+CBANDCFG?

original output:

 +CBANDCFG: "CAT-M",1,2,3,4,5,8,12,13,18,19,20,26,28,39
 +CBANDCFG: "NB-IOT",1,2,3,5,8,12,13,17,18,19,20,26,28

AT+CBANDCFG=<mode>,<band>
- example : AT+CBANDCFG="NB-IOT",8(中華電信)
```
~~~
參閱後半部分的供應商基地台頻段部分
~~~
### Get and Set Mobile Operation Band
#### [B]
```=
AT+CPSI?

output example : 
 +CPSI: LTE NB-IOT,Online,466-92,0xCABC,54690159,364,EUTRAN-BAND8,3640,0,0,-14,-106,-92,7
```
### 得到現在的連線模式，推薦使用ALL_MODE
#### [B]
```=
AT+CBAND=?
output example :
 +CBAND: (EGSM_MODE,DCS_MODE,ALL_MODE)
AT+CBAND?
```

 
### LTE/GSM模式設定 預設AT+CNMP=51
#### [B]
```=
AT+CNMP
 +CNMP: ((2-Automatic),(13-GSM Only),(38-LTE Only),(51-GSM And LTE Only))

```
- 在測試過程中 發現參數設定為38可以連上

### CAT-M and NB-IoT選擇 預設AT+CMNB=3
#### [B]
```=
AT+CMNB
 +CMNB: ((1-Cat-M),(2-NB-IoT),(3-Cat-M And NB-IoT))
```
- 測試中參數設定為2

### NB-IOT掃描規則
#### [B]
```=
AT+CNBS
```
> Configure Band Scan Optimization For NB-IOT
> > level 0  
> >```
> > Used for good SNR levels(0 db and above)
> > detects strong cells first and takes the shortest time to acquire cells.
> > UE scans each raster in 30 ms.
> >```
> > level 1   
> > ```
> > Used for medium SNR levels(-9 dB and above),UE scans each raster for 201 ms 
> >```
> > level 2
> > ```
> > Used for poor SNR levels(-12.6 dB and above),UE scans each raster for 500 ms.
> > ```
> 在不同level下搭配不同的掃描方法，編號如以下：
> > [n]
> > >   1      UE tries SNR level 0 band scan<br>
> > >   2      UE tries SNR level 0 and level 1 band scan<br>
> > >   3      UE tries SNR level 0, level 1, and level 2 band scan<br>
> > >   4      Reserved<br>
> > >   5      UE tries SNR level 2 band scan only

### 數據服務狀態
#### [B]
Data Service's Status.
```=
AT+CGATT=?
-  0 Detached
-  1 Attached
```
### SIM卡狀況與編號
- SIM狀態
  - [B]
```=
AT+CPIN?
    +CPIN: READY
```
- SIM編號
  - [A]
```=
AT+CIMI
    466924700027595
->其實從SIM卡編號開頭可能可以大致猜出其公司名稱與使用頻段，參閱後面MCC與MNC環節，至少中華感覺可以
```
### 設定IOT供應商的APN與查詢
- 獲取APN
  - [A]
```=
AT+CGNAPN
```
- 現在網路供應商與供應商設定
  - [B] **必須設定**
```=
AT+CSTT?
+CSTT: "internet.iot","",""

- 中華電信
    AT+CSTT="internet.iot"
- 台灣大哥大
    AT+CSTT="twm.nbiot"
```
### IP連線型態->IPV4/IPV6
#### [B]
```=
AT+CNACTCFG
-  IP type, default value is IPV4
```
### 設定連線，**必須check**
#### [B]
AT+CNACT
-  AT+CNACT=<mode>[,<APN or IP>]
-  AT+CNACT=1 ->open the wireless connection
-  AT+CNACT? get local IP
### Configure NB-IOT Scrambling Feature(Not Necessary, Using default)
#### [B]
`AT+NBSC=?`


### 連線狀態、APN設定
#### [B]
```=
AT+CGDCONT?
    +CGDCONT: 1,"IPV4V6","","0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0",0,0,0,0

基地台設定
AT+CGDCONT=1,"IP","internet.iot"
    [中華電信]
自動設定基地台->直接將APN留空
AT+CGDCONT=1,"IP",""

AT+CGDCONT=?
    +CGDCONT: (1-24,100-179),"IP",,,(0-2),(0-4),(0-1),(0-1)
    +CGDCONT: (1-24,100-179),"PPP",,,(0-2),(0-4),(0-1),(0-1)
    +CGDCONT: (1-24,100-179),"IPV6",,,(0-2),(0-4),(0-1),(0-1)
    +CGDCONT: (1-24,100-179),"IPV4V6",,,(0-2),(0-4),(0-1),(0-1)
    +CGDCONT: (1-24,100-179),"Non-IP",,,(0-2),(0-4),(0-1),(0-1)
```
### 訊號強度
#### [A]
```=
AT+CSQ
```

- 訊號強度對照表格

|編號|對應訊號強度|
|---|---|
|0|≤ -115 dBm|
|1|-111 dBm|
|2~30|-110 ~ -54 dBm|
|31|≥ -52 dBm|
|99|unknown or not detectable|

基本上需要大於8才可用，個人經驗是要大於12以上
### 開啟數據網路
#### [A]
```=
AT+CIICR

> OK
```
### 找到目前IP
#### [A]
```=
AT+CIFSR
```
### Ping測試-8.8.8.8 Google DNS
#### [B]
```=
AT+CIPPING="8.8.8.8"



OUTPUT:
    +CIPPING: 1,"8.8.8.8",60000,255
    +CIPPING: 2,"8.8.8.8",60000,255
    +CIPPING: 3,"8.8.8.8",60000,255
    +CIPPING: 4,"8.8.8.8",60000,255
    <replyId>,<Ip Address>,<replyTime>,<ttl>

AT+CIPPING=<IPaddr>[,<retryNum>[,<dataLen>[,<timeout>,<ttl>]]]


```

### 關閉AT通訊
#### [A]
```=
AT+CIPSHUT
```
### 查詢公司與掃描基地台
#### [B]
```=
AT+COPS?
+COPS: 0,0,"Chunghwa Telecom",9
       [mode],[formate],[oper],[netact]
       netact->參考基地台種類

[掃描基地台]
AT+COPS=?

> +COPS: (3,"466 05","466 05","46605",0),(2,"466 92","466 92","46692",7),(1,"466 92","466 92","46692",9),,(0,1,2,3,4),(0,1,2)

註解：
#> +COPS:(基地台編號[N],MCC MNC,MCC MNC,MCCMNC,基地台種類編號[netact]))

[基地台種類][netact]
    0 User-specified GSM access technology
    7 User-specified LTE M1 A GB access technology
    9 User-specified LTE NB S1 access technology
[mode]
    0 Automatic mode; <oper> field is ignored 
    1 Manual (<oper> field shall be present, and <AcT> optionally) 
    2 manual deregister from network 
    3 set only <format> (for read Command +COPS?) - not shown in Read Command response 
    4 Manual/automatic (<oper> field shall be present); if manual selection fails, automatic mode (<mode>=0) is entered
[format]
    0 Long format alphanumeric <oper> 
    1 Short format alphanumeric <oper> 
    2 Numeric <oper>; GSM Location Area Identification

AT+COPS=,,"MCCMNC"
->指定基地台連線與連線模式、傳輸型態

```

- MCC 國家代碼
    - 台灣為466
- MNC 行動網路供應商代碼
## 電信業者編號、頻段與頻段對應編號
例如中華電信走band8要如何得知：
1. 下為表格與對應頻段

| MNC編碼 | 供應商 | 對應頻段 |
| :------: | :------: | ------ |
| 01 | 遠傳電信 | GSM900 |
|03|遠傳電信|UMTS 2100 <br> FDD-LTE 700 1800 2600 <br> TDD-LTE 2600|
|05|亞太電信|CDMA2000 800 <br> FDD-LTE 700 <br> TDD-LTE 2600|
|11|中華電信|GSM 1800|
|89|台灣之星|UMTS 2100 <br> FDD-LTE 900 2600|
|92|中華電信|GSM 900<br>UMTS2100|
|97|台灣大哥大|GSM 1800<br>UMTS2100<br> FDD-LTE 700 1800|

2. LTE基本介紹與台灣頻段表(共用頻段表很多，只列用的到的台灣頻段)
   - LTE可以想成3G-4G的過度，俗稱3.9G<br>

   - LTE Long Term Evolution  <br> 長期演進技術

   |名稱|描述|速度|
   |---|---|---|
   |`LTE-Advanced`<br>`長期演進技術升級版`|事實上的唯一主流4G標準<br>向後(向下?)兼容LTE標準，基本上通過LTE的軟體升級即可|Upload 500Mbps<br>Download 1Gbps
   |`LTE FDD`<br>`分頻雙工長期演進技術`|最早提出的LTE格式|Upload 40Mbps<br>Download 150Mbps|
   |`LTE TDD`/`TD-LTE`<br>`分時雙工長期演進技術`|~|Upload 50Mbps<br>Download 100Mbps|

3. 台灣主要使用頻段與對照表

|Type| Band|Frequency|
|:---:|:---:|:---:|
FDD-LTE |Band 1 | 2100MHz|
FDD-LTE |Band 3 | 1800MHz|
FDD-LTE |Band 7 | 2600MHz|
FDD-LTE |Band 8 | 900MHz|
FDD-LTE |Band 28| 700MHz|
TD-LTE |Band 38 | 2600MHz

## **MQTT proprity**
### 連線狀態相關
**`重要`**
```=
AT+CNACTCFG?
> 輸出IP類型
```

### 查詢連線狀態與local IP address

> ```=
> AT+CNACT?
> ```
> 輸出連線狀態，若為
> >`+CNACT: 0,"0.0.0.0"`
> 
>  要調整為
> ```=
> AT+CNACT=1
> ```


### MQTT Server 設定
#### [B]
```=
AT+SMCONF?
    ------  MQTT status   ------
AT+SMCONF="URL","scplus-iot.ipcs.ntu.edu.tw","1883"
AT+SMCONF="USERNAME","riceball"
AT+SMCONF="TOPIC","NTU/SmartCampus/NTU4AQ_riceball_test"
AT+SMCONF="KEEPTIME",60
AT+SMCONF="QOS",1
AT+SMCONF="CLEANSS",1
AT+SMCONF="MESSAGE","nbiot-test-20221218"

```
### **檢測MQTT伺服器狀態**
#### [A]
```=
AT+SMCONN
->需要等一段時間
```
### 傳輸MQTT指令
#### [B]
`請注意字串長度`
```=
AT+SMPUB="[TOPIC(address under url-> as floder)]",<length>,<QOS>,<Retain>
```
>Example:
>```=
>AT+SMPUB="NTU/SmartCampus/NTU4AQ_riceball_test",10,1,1
> > [~~~~~~~~]
> 
>```
> 有`OK`出現才是傳輸成功

## 反射板初步討論
`目前在不同的反射板方向，確實有不同的訊號強度與延遲`

應該具有實驗的可行性
## 用電討論
~~~
在不同的訊號環境下會有不同的資料傳輸延遲與速度
在訊號環境不好的狀況下，會需要花比較長的時間連線與找基地台
也有可能同時傳遞多筆相同資料
~~~

- 在進行通訊時，其電流約為0.14~0.17A
  - 約為0.7~0.9W
  - 通訊時間從1秒左右到10秒都有
    - 以前出現過以分鐘計算的資料傳輸時間
    - 消耗能量約為1J-9J
- 鋰電池容量約為3000mAh
  - 約為9Wh
- 倘若訊號不好，則有可能快速且大量的耗電
  - 15min一筆，也就是一天會嘗試約96次