#line 1 "E:\\.NTU\\IPCS\\airsensor\\test_code\\Time_func_esp32.cpp"
#include <ESP32Time.h>
#include "time.h"
#include <sys/time.h>
class Time_ESP32{
    private:
        int year,month,day;
        int hour,minute,second;
        int LST_hour = 0;
    public:
    ESP32Time* rtc;
    Time_ESP32(){
        rtc = new ESP32Time();
    };
    Time_ESP32(int LST_Offset){
        LST_hour = LST_Offset;
        rtc = new ESP32Time(LST_hour*3600);
    };
    void update(){
        struct tm timeinfo = rtc->getTimeStruct();
        year = timeinfo.tm_year+1900;
        month = timeinfo.tm_mon;
        day = timeinfo.tm_mday;
        hour = timeinfo.tm_hour;
        minute = timeinfo.tm_min;
        second = timeinfo.tm_sec;
    }
    void update_onlinetime(){
        /* update time by internet
        using 
        time.stdtime.gov.tw
        */
       const char* server = "www.stdtime.gov.tw";
    //    const char* server = "pool.ntp.org";
       configTime(0,0,server);
       Serial.println("update_onlinetime by "+String(server));
    }
    String getTimeFormat(){
        char *str = new char[100];
        update();
        sprintf(str,"%d/%d/%d-%d:%d:%d",year,month,day,hour,minute,second);
        String output = String(str);
        return output;
    }
    
};