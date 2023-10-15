# NB-IOT(SIM7000 module)衛星定位AT指令筆記
###### tags: `GPS` `NTU4AQ` `IOT ` `NB-IOT` `GNSS`

```=
使用全球衛星定位系統進行定位
最高頻率為一秒鐘更新一次
目前使用[serial port]進行通訊
連線測試：參閱 NB-IOT連接板指令筆記與操作紀錄

```

```=
Mainly Using SIM7000 series(SIM7000E and SIM7000G), [Made In China]
SIM7000E-Euro
SIM7000G-Global
兩者在4G通訊支援頻段上不同，但都有支援GPS模組，必須加裝天線
```
`Defult using 115200,CRLF`
`Also can using 9600,CRLF`
# SIM7000G相關指令與操作筆記
## 基本指令與連線設定

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
- 基本上可以將其分為設定與非設定，依照其功能去做分別

### AT test
- 測試AT是否有work，基本中的基本
```=
AT
    OK
```
### 設定NMEA USB/URAT output
`NMEA : National Marine Electronics Association`
> 是否將資料輸出在NMEA USB port，其與AT占用不同的COM port
> > 類似於若為常開電源並設定其須常態將GPS讀取到之資料回報於USBport，則須設定
> >
> > 倘若只使用AT command 去讀取資料則不需要注意此點
> 不確定是否可以直接在pin上面讀取該資料
> 
-  輸出 GNSS NMEA格式
-  必須在AT+CGNSPWR開啟之前設定完畢

[B]
```=
AT+CGNSCFG=?
    - 0 not output in NMEA port
    - 1 output in NMEA port
    - 2  UART3
```
### 開啟GNSS power
[B]
```
AT+CGNSPWR=?
    - 1 power on
    - 0 power off
```
- both for UART and USB AT serial port

### 設定不同衛星系統
[B]
```=
AT+CGNSMOD
    AT+CGNSMOD=<gps mode>,<glo mode>,<bd mode>,<gal mode>

```
### 從AT看NMEA 衛星資料
[B]->可設定參數後一次性output
```=
AT+CGNSTST=[STAT],[TIMES]
    - STAT
        - 0 turn off GNSS NMEA data to AT
        - 1 turn on
    - TIMES
        - times of output NMEA package
```
### GPS 讀取資料
[A]
```=
AT+CGNSINF
    +CGNSINF: <GNSS run status>,<Fix status>,
    <UTC date & Time>,<Latitude>,<Longitude>,<MSL Altitude>,
    <Speed Over Ground>, <Course Over Ground>,<Fix Mode>,
    <Reserved1>,<HDOP>, <PDOP>,<VDOP>,<Reserved2>,
    <GNSS Satellites in View>, <GNSS Satellites Used>,<GLONASS Satellites Used>,<Reserved3>, <C/N0 max>,<HPA>,<VPA>
```
### GPS 自動讀取資料
[B]
```=
AT+CGNSURC=[mode]
    - mode
        - 0 turn off
        - 1 on and report every GNSS FIX
        - 2 on and report every 2 GNSS FIX
        - [...]
        - 255 on and report every 255 GNSS FIX
```