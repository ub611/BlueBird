#include "test.h"

#define     THRD_MESSAHE_CONNECT_DEVICE   WM_APP	//ù ��⿬��
#define		THRD_MESSAGE_SOMEWORK		  WM_APP + 1 //��ũ����	
#define		THRD_MESSAGE_EXIT			  WM_APP + 2 //��ũ���� ����
#define     THRD_MESSAGE_NOSIGNAL_DEVICE  WM_APP + 3 //������� ���� ����
#define     THRD_MESSAGE_NODEVICE         WM_APP + 4 //����� ��� ����
#define     THRD_CLOSE					  WM_APP + 5  //��羲��������

using namespace Monitoring;

void M_start();
void M2_start();
DWORD WINAPI ThreadProc_MAPP(LPVOID);
DWORD WINAPI ThreadProc_MAPP1(LPVOID);
DWORD rereturnThreadId();

DWORD MAPP_Thread_ID = 0;
DWORD MAPP_Thread_ID1 = 0;
DWORD pp = 0;

int MSG_EXIT = 0;

int main()
{
	HANDLE MAPP_Thread1;
	HANDLE MAPP_Thread;


	char *APP_Thread_Msg = "(text.cpp)   ������� üũ ������ �����Ϸ� "; // ���Ͻ��Ķ����
	char *APP_Thread_Msg1 = "(text.cpp)   ������� üũ ������ �����Ϸ�111 "; // ���Ͻ��Ķ����

	MAPP_Thread1 = CreateThread(NULL, 0, ThreadProc_MAPP1, (LPVOID)APP_Thread_Msg, 0, &MAPP_Thread_ID1);
	MAPP_Thread = CreateThread(NULL, 0, ThreadProc_MAPP, (LPVOID)APP_Thread_Msg1, 0, &MAPP_Thread_ID);


	WaitForSingleObject(MAPP_Thread1, INFINITE);
	WaitForSingleObject(MAPP_Thread, INFINITE);

	printf("(text.cpp)   ����2\n");

	//Monitoring::MonitoringFuncs::startMon(MAPP_Thread_ID);
	//Monitoring::MonitoringFuncs::startMon(MAPP_Thread_ID);




	return 0;
}

//void M2_start(){
//	HANDLE MAPP_Thread1;
//	printf("text�� M_start����\n");
//
//	char *APP_Thread_Msg = "������� üũ ������ �����Ϸ� "; // ���Ͻ��Ķ����
//
//	MAPP_Thread1 = CreateThread(NULL, 0, ThreadProc_MAPP1, (LPVOID)APP_Thread_Msg, 0, &MAPP_Thread_ID1);
//
//	WaitForSingleObject(MAPP_Thread1, INFINITE);
//	printf("M_start���� ������ ������\n");
//}
//
//void M_start(){
//	HANDLE MAPP_Thread;
//	printf("text�� M_start����111\n");
//
//	char *APP_Thread_Msg = "������� üũ ������ �����Ϸ�111 "; // ���Ͻ��Ķ����
//	
//	MAPP_Thread = CreateThread(NULL, 0, ThreadProc_MAPP, (LPVOID)APP_Thread_Msg, 0, &MAPP_Thread_ID);
//
//	WaitForSingleObject(MAPP_Thread, INFINITE);
//	printf("M_start���� ������ ������111\n");
//}

void MSGfunc()
{

	MSG msg = { 0 };
	printf("(text.cpp)   �����Լ� �޼�������\n");
	while (!MSG_EXIT){
		//printf("����1 ::::%d\n", GetLastError());
		printf("(text.cpp)   �����Լ� �޼������� �� whlie����\n");
		Sleep(1000);
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//printf("����2 :::%d\n", GetLastError());
			printf("(text.cpp)   �޼���������\n");
			if (msg.message == WM_APP + 1)
			{
				printf("(text.cpp)   ��ũ����\n");
			}
			if (msg.message == WM_APP + 2)
			{
				printf("(text.cpp)   ��ũ��������\n");
			}
			if (msg.message == WM_APP + 3)
			{
				printf("(text.cpp)   ������� ��������\n");
			}
			if (msg.message == WM_APP + 4)
			{
				printf("(text.cpp)   ����� ��� ����\n");
			}
			if (msg.message == WM_APP + 5)
			{
				printf("(text.cpp)   ��� ������ ����\n");
				PostThreadMessage(MAPP_Thread_ID, WM_APP + 5, 0, 0);
			}

			DispatchMessage(&msg);
			printf("����3 :::%d\n", GetLastError());
		}
		printf("(text.cpp)MSG������ ��������\n");

	}
}

DWORD WINAPI ThreadProc_MAPP(LPVOID M_param)
{
	char *TP_Msg = (char *)M_param; // ���Ͻ÷� ���� �Ķ����

	printf("(text.cpp)   %s :: �޼��� ������ ID: %d\n", TP_Msg, MAPP_Thread_ID);
	///�̺κк��� ���� �Ѿ����忡�� �� �Լ��� ����־���� �׷��� ������ �ϳ� �� ����� �׽�Ʈ�غ���
	Monitoring::MonitoringFuncs::InitMon(MAPP_Thread_ID1);

	printf("(text.cpp)   ������ ��������\n");
	return (DWORD)M_param;
}


DWORD WINAPI ThreadProc_MAPP1(LPVOID M_param)
{
	char *TP_Msg = (char *)M_param; // ���Ͻ÷� ���� �Ķ����
	MSGfunc();
	printf("(text.cpp)  %s :: �޼��� ������ ID: %d\n", TP_Msg, MAPP_Thread_ID1);
	///�̺κк��� ���� �Ѿ����忡�� �� �Լ��� ����־���� �׷��� ������ �ϳ� �� ����� �׽�Ʈ�غ���
	printf("(text.cpp)  ������ ��������111\n");
	return (DWORD)M_param;
}















//void M_start(){
//	HANDLE MMSG_Thread;
//
//	char *MMSG_Thread_Msg = "������� üũ ������ �����Ϸ� "; // ���Ͻ��Ķ����
//
//	MMSG_Thread = CreateThread(NULL, 0, ThreadProc_MSG, (LPVOID)MMSG_Thread_Msg, 0, &MAPP_Thread_ID);
//
//	WaitForSingleObject(MMSG_Thread, INFINITE);
//}
//DWORD WINAPI ThreadProc_MSG(LPVOID M_param)
//{
//	char *TP_Msg = (char *)M_param; // ���Ͻ÷� ���� �Ķ����
//
//	printf("%s :: �޼��� Ȯ�� ������ ID: %d\n", TP_Msg, MAPP_Thread_ID);
//	
//
//	return (DWORD)M_param;
//}