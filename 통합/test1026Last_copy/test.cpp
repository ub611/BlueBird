#include "test.h"

#define     THRD_MESSAHE_CONNECT_DEVICE   WM_APP	//첫 기기연결
#define		THRD_MESSAGE_SOMEWORK		  WM_APP + 1 //스크린락	
#define		THRD_MESSAGE_EXIT			  WM_APP + 2 //스크린락 해지
#define     THRD_MESSAGE_NOSIGNAL_DEVICE  WM_APP + 3 //블루투스 꺼져 있음
#define     THRD_MESSAGE_NODEVICE         WM_APP + 4 //연결된 기기 없음
#define     THRD_CLOSE					  WM_APP + 5  //모든쓰레드종료

using namespace std;

namespace Monitoring
{
	int Lock_state = 0;
	int MSG_repeat_control = 0;
	bool bContinue = true;
	HANDLE M_Thread;	//모니터링 진입 쓰레드핸들
	DWORD m_idThread = 0;	//메인 앱 쓰레드 ID (메세지 전송할 ID)
	DWORD bt_idThread = 0;	//블루투스 쓰레드 ID
	DWORD WINAPI ThreadProc_BT(LPVOID);

	BLUETOOTH_FIND_RADIO_PARAMS m_bt_find_radio = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
	BLUETOOTH_DEVICE_SEARCH_PARAMS m_search_params = {
		sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
		1,
		0,
		1,
		1,
		1,
		15,
		NULL
	};
	BLUETOOTH_DEVICE_INFO S_device_info = { sizeof(BLUETOOTH_DEVICE_INFO), 0, };
	BLUETOOTH_DEVICE_INFO C_device_info = { sizeof(BLUETOOTH_DEVICE_INFO), 0, };
	BLUETOOTH_RADIO_INFO m_bt_info = { sizeof(BLUETOOTH_RADIO_INFO), 0, };

	void MonitoringFuncs::checkConn(BLUETOOTH_DEVICE_INFO &C_Device, BLUETOOTH_DEVICE_INFO &S_Device){
		//printf("메세지 전달 쓰레드 ID : %d\n", m_idThread);

		if (S_Device.Address.ullLong == 0){
			if (C_Device.fConnected){
				S_Device.Address.ullLong = C_Device.Address.ullLong;
				PostThreadMessage(m_idThread, WM_APP, 0, 0);
				//printf("체크문1번(첫 기기연결)\n");
			}
			else{
				if (MSG_repeat_control != 1){
					//연결된 기기 없다고 MSG 보내기
					//printf("체크문2번(연결된 기기없음)\n");
					PostThreadMessage(m_idThread, WM_APP + 4, 0, 0);
					//printf("%d\n", GetLastError());
					MSG_repeat_control = 1;
				}
			}
		}
		else{
			if (C_Device.fConnected){
				if (Lock_state == 1){
					if (C_Device.Address.ullLong == S_Device.Address.ullLong){
						if (MSG_repeat_control != 4){
							// 락스크린중지 MSG전송
							//printf("체크문3번(스크린락 해지)\n");
							PostThreadMessage(m_idThread, WM_APP + 2, 0, 0);
							Lock_state = 0;
							MSG_repeat_control = 4;
						}
					}
				}
				else{
					if (C_Device.Address.ullLong != S_Device.Address.ullLong){
						if (MSG_repeat_control != 3){
							//락스크린 실행 MSG 전송
							//printf("체크문4번(스크린락 실행)\n");
							PostThreadMessage(m_idThread, WM_APP + 1, 0, 0);
							Lock_state = 1;
							MSG_repeat_control = 3;
						}
					}

				}
			}
			else{
				if (Lock_state == 0){
					if (MSG_repeat_control != 2){
						// 락스크린 실행 MSG 전송
						//printf("체크문5번(스크린락 실행)\n");
						PostThreadMessage(m_idThread, WM_APP + 1, 0, 0);
						Lock_state = 1;
						MSG_repeat_control = 2;
					}
				}
			}
		}
	}

	void MonitoringFuncs::bthMain(){		//블루투스 메인함수
		HANDLE m_radio = NULL;
		HBLUETOOTH_RADIO_FIND m_bt = NULL;
		HBLUETOOTH_DEVICE_FIND m_bt_dev = NULL;
		int m_device_id = 0;
		DWORD mbtinfo_ret;

		//printf("Btmain접근\n");
		while (bContinue)
		{
			//printf("접근1(radio)\n");
			m_bt = BluetoothFindFirstRadio(&m_bt_find_radio, &m_radio);

			do {
				if (m_bt == 0){
					//printf("1\n");
					if (MSG_repeat_control != 5){
						//printf("2\n");
						//컴퓨터 블루투스가 꺼진 상황
						
						PostThreadMessage(m_idThread, WM_APP + 3, 0, 0);
						MSG_repeat_control = 5;
					}
					break;
				}
				// Then get the radio device info....
				mbtinfo_ret = BluetoothGetRadioInfo(m_radio, &m_bt_info);
				m_search_params.hRadio = m_radio;
				ZeroMemory(&S_device_info, sizeof(BLUETOOTH_DEVICE_INFO));
				S_device_info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);

				// Next for every radio, get the device
				m_bt_dev = BluetoothFindFirstDevice(&m_search_params, &S_device_info);

				m_device_id = 0;

				// Get the device info
				do
				{
					//printf("접근2(device)\n");
					Monitoring::MonitoringFuncs::checkConn(S_device_info, C_device_info);
					m_device_id++;

				} while (BluetoothFindNextDevice(m_bt_dev, &S_device_info));

			} while (BluetoothFindNextRadio(&m_bt_find_radio, &m_radio));

			// Give some time for the 'signal' which is a typical for crap wireless devices
			Sleep(5000);
		}

	}

	void MonitoringFuncs::stopMon(){  //메인쓰레드에서 호출하는 함수
		//printf("stopMon() 함수 실행\n");
		Monitoring::bContinue = false;
	}

	void MonitoringFuncs::InitMon(DWORD ThreadID){   //메인쓰레드에서 메인쓰레드ID를 매개변수로 호출하는 함수
		Monitoring::m_idThread = ThreadID;	//받아온 쓰레드 ID 값
		//printf("받은 메인쓰레드 ID : %d\n", m_idThread);
		startMon();
	}

	void MonitoringFuncs::UnInitMon(){   //메인스레드에서 호출하는 함수
		//printf("UnInitMon()함수 실행\n");
		CloseHandle(Monitoring::M_Thread);  //핸들 닫힘
	}

	void MonitoringFuncs::startMon(){
		char *M_Thread_Msg = "블루투스 쓰레드 생성완료 "; // 프록시파라미터

		M_Thread = CreateThread(NULL, 0, ThreadProc_BT, (LPVOID)M_Thread_Msg, 0, &bt_idThread);
		WaitForSingleObject(M_Thread, INFINITE);

		bContinue = true;	//stopMon()실행 후 다시 시작하기 위해 flag값 변환
	}

	DWORD WINAPI ThreadProc_BT(LPVOID lpParam)
	{
		char *TP_Msg = (char *)lpParam; // 프록시로 들어온 파라미터
		//printf("%s :: 블루투스 체크 쓰레드 ID: %d\n", TP_Msg, bt_idThread);

		//여기서 이제 블루투스함수진입
		Monitoring::MonitoringFuncs::bthMain();

		return (DWORD)lpParam;
	}
}