#pragma once
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")
#include <BluetoothAPIs.h>

extern int state;
extern BLUETOOTH_FIND_RADIO_PARAMS m_bt_find_radio;
extern BLUETOOTH_DEVICE_SEARCH_PARAMS m_search_params;
extern BLUETOOTH_DEVICE_INFO m_device_info;
extern BLUETOOTH_DEVICE_INFO c_device_info;
extern BLUETOOTH_RADIO_INFO m_bt_info;

namespace Monitoring
{
	class MonitoringFuncs
	{
	public:
		static void InitMon(DWORD);
		static void startMon(); 
		static void stopMon();
		static void UnInitMon();
		static void checkConn(BLUETOOTH_DEVICE_INFO &, BLUETOOTH_DEVICE_INFO &);
		static void bthMain();
	};
}
