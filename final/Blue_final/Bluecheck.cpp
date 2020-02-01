#include "Bluecheck.h"

#define     THRD_MESSAHE_CONNECT_DEVICE   WM_APP	//ù ��⿬��
#define		THRD_MESSAGE_SOMEWORK		  WM_APP + 1 //��ũ����	
#define		THRD_MESSAGE_EXIT			  WM_APP + 2 //��ũ���� ����
#define     THRD_MESSAGE_NOSIGNAL_DEVICE  WM_APP + 3 //������� ���� ����
#define     THRD_MESSAGE_NODEVICE         WM_APP + 4 //����� ��� ����
#define     THRD_CLOSE					  WM_APP + 5  //��羲��������

using namespace std;

namespace Monitoring
{
	int Lock_state = 0;
	int MSG_repeat_control = 0;
	bool bContinue = true;
	HANDLE M_Thread;	//����͸� ���� �������ڵ�
	DWORD m_idThread = 0;	//���� �� ������ ID (�޼��� ������ ID)
	DWORD bt_idThread = 0;	//������� ������ ID
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


		if (S_Device.Address.ullLong == 0){
			if (C_Device.fConnected){
				S_Device.Address.ullLong = C_Device.Address.ullLong;
				PostThreadMessage(m_idThread, WM_APP, 0, 0);

			}
			else{
				if (MSG_repeat_control != 1){
					PostThreadMessage(m_idThread, WM_APP + 4, 0, 0);

					MSG_repeat_control = 1;
				}
			}
		}
		else{
			if (C_Device.fConnected){
				if (Lock_state == 1){
					if (C_Device.Address.ullLong == S_Device.Address.ullLong){
						if (MSG_repeat_control != 4){
							PostThreadMessage(m_idThread, WM_APP + 2, 0, 0);
							Lock_state = 0;
							MSG_repeat_control = 4;
						}
					}
				}
				else{
					if (C_Device.Address.ullLong != S_Device.Address.ullLong){
						if (MSG_repeat_control != 3){
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
						PostThreadMessage(m_idThread, WM_APP + 1, 0, 0);
						Lock_state = 1;
						MSG_repeat_control = 2;
					}
				}
			}
		}
	}

	void MonitoringFuncs::bthMain(){		//������� �����Լ�
		HANDLE m_radio = NULL;
		HBLUETOOTH_RADIO_FIND m_bt = NULL;
		HBLUETOOTH_DEVICE_FIND m_bt_dev = NULL;
		int m_device_id = 0;
		DWORD mbtinfo_ret;

		while (bContinue)
		{

			PostThreadMessage(m_idThread, WM_APP + 3, 0, 0);
			MSG_repeat_control = 5;

			m_bt = BluetoothFindFirstRadio(&m_bt_find_radio, &m_radio);

			do {
				if (m_bt == 0){
					if (MSG_repeat_control != 5){

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
					Monitoring::MonitoringFuncs::checkConn(S_device_info, C_device_info);
					m_device_id++;

				} while (BluetoothFindNextDevice(m_bt_dev, &S_device_info));

			} while (BluetoothFindNextRadio(&m_bt_find_radio, &m_radio));

			// Give some time for the 'signal' which is a typical for crap wireless devices
			Sleep(5000);
		}

	}

	void MonitoringFuncs::stopMon(){  //���ξ����忡�� ȣ���ϴ� �Լ�

		Monitoring::bContinue = false;
	}

	void MonitoringFuncs::InitMon(DWORD ThreadID){   //���ξ����忡�� ���ξ�����ID�� �Ű������� ȣ���ϴ� �Լ�
		Monitoring::m_idThread = ThreadID;	//�޾ƿ� ������ ID ��
		startMon();
	}

	void MonitoringFuncs::UnInitMon(){   //���ν����忡�� ȣ���ϴ� �Լ�
		CloseHandle(Monitoring::M_Thread);  //�ڵ� ����
	}

	void MonitoringFuncs::startMon(){
		char *M_Thread_Msg = "������� ������ �����Ϸ� "; // ���Ͻ��Ķ����

		M_Thread = CreateThread(NULL, 0, ThreadProc_BT, (LPVOID)M_Thread_Msg, 0, &bt_idThread);
		WaitForSingleObject(M_Thread, INFINITE);

		bContinue = true;	//stopMon()���� �� �ٽ� �����ϱ� ���� flag�� ��ȯ
	}

	DWORD WINAPI ThreadProc_BT(LPVOID lpParam)
	{
		char *TP_Msg = (char *)lpParam; // ���Ͻ÷� ���� �Ķ����
		Monitoring::MonitoringFuncs::bthMain();

		return (DWORD)lpParam;
	}
}