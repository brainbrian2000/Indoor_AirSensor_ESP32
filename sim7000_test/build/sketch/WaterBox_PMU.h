#include <Arduino.h>
#ifndef _WATERBOX_PMU_
#define _WATERBOX_PMU_

#ifndef _SIMCOM7000E_H_
#include "SIM7000.h"
#endif // _SIMCOM7000E_H_


#define _I2C_BUFFER_SIZE 30

class WaterBox_PMU
{
public:
	enum POWER
	{
		OFF,
		ON
	};

	enum STATE
	{
		SLEEP,
		MASTER,
		SLAVER
	};

	enum UART
	{
		NONE,
		EOL,
		H_PMU,
		H_I2C,
		H_CMD
	};

	enum IC2Request {
		Req_NONE,             // �LRequest
		Req_ERROR,           // ���~���O
		Req_TIME,              // �]�w�x��(��), �^��OK -> timer ? sec
		Req_SLEEP,             // �i�J�ίv, ���^��
		Req_LOWPOWER,         // �ٹq�Ҧ�,  �^��OK->LowPower ON/OFF
		Req_BAT_VOLATE,       // �q���q��,  �^��OK->volate
		Req_BAT_CURRENT,      // �q���q��,  �^��OK->current
		Req_AT                   // AT Cmd( ���~����NBIOT��)
	};

	WaterBox_PMU();						// �غc�禡 
	WaterBox_PMU(uint8_t _addr);   // �غc�禡(���w	Slaver�ɪ�I2C Address)
	~WaterBox_PMU();					// �Ѻc�禡 

	void init(uint16_t _pinMain, uint16_t _pinNBIOT=5, uint16_t _pinINA219=4);
	void setINA219(uint8_t _addr);
	static void setDebuger(Stream& refSer);                // �]�wdebug�Ϊ���X
	static void setWakeUpVolate(float _v);                    // �]�w����q��
	static void setSleepSec(uint32_t _sec);
	static uint8_t Sleep(float _voltage = _WakeUpVoltage, STATE _state = SLAVER);         // �}�l�ίv�G������Master�A�����ѹq��}�l�i�J�ίv�`���A�`�������ɧ�s�q�����A�A�C�����q����A�}�l�ѹq��^1 (�i�H��while(Sleep())�@�Ⱥ�
	static void PowerSaveMode(POWER _state);
	static void ControlPower(POWER _state);

	static void getBetteryState();					               // ���o�q�����A�G������Master�A�I�sINA219����ƫ��s�ܼ�
	static void LED(uint16_t _times, uint16_t _interval);

	static float Volate;								          // �q���W�����q��
	static float Current;									  // �q���W�����q�y�q
	static enum STATE state;

	static String BAT_V;
	static String BAT_A;
	static String WS;
	static String WD;
	static String RAIN;
	static String PM025;
  static String PM100;
  static String PM010;
  static String TEMP;
  static String HUM;
  static String LUX;
  static String kPa;
  static String dBA;
  static String rtcDate;
  static String rtcTime;

  /*
   BAT_V=3.99|      // 數據長度 4
   BAT_A=100.0|     // 數據長度 5
   WS=3.004|        // 數據長度 5
   WD=340|          // 數據長度 3
   RAIN=500.4|      // 數據長度 5
   PM025=100.1|     // 數據長度 5
   PM100=1500.2|    // 數據長度 6
   PM010=50.4|      // 數據長度 4
   TEMP=34.14|      // 數據長度 5
   HUM=100.0|       // 數據長度 5
   LUX=100000|      // 數據長度 6
   kPa=111001|      // 數據長度 6
   lat=121.123456|  // 數據長度 10  // 由class SIM7000.h  提供
   lon=25.123456|   // 數據長度 8   // 由class SIM7000.h  提供
   dBA=100.04|      // 數據長度 6
*/
	static String ATCMD;                                    //  ��NBIOT��Command
	static void ATClear();

private:
	static uint8_t _Debug;
	static uint16_t _CrtlPinMAIN;
	static uint16_t _CrtlPinNBIOT;
	static uint16_t _CrtlPinINA219;

	//static SIM7000 _NB;

	static Stream& refSerial;

	static uint8_t _Addr;
	static uint8_t _INA219Addr;

	 //uint8_t _SleepMode;                       // ���ϥ�
	static uint8_t _PowerSave;                // �w�q�O���O�n�i�J�ٹq�Ҧ�
	static uint16_t _SleepSec;				   // �ίv���

	static uint16_t _i_for;
	static float _WakeUpVoltage;
  static float _LazyVoltage;

	static void _SwitchToSlaver(uint8_t _addr);

	static void _receiveEvent(int howMany);
	static void _requestEvent();
	static void _cmd(String _str);

	static String _ComStr;
	static String _recBuffer;
	static IC2Request  _REQUEST;

	static void _Deguber(String _msg, UART _header, UART _uart = NONE);

};
#endif
