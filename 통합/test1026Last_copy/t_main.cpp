#include "test.h"

#define     THRD_MESSAHE_CONNECT_DEVICE   WM_APP	//첫 기기연결
#define		THRD_MESSAGE_SOMEWORK		  WM_APP + 1 //스크린락	
#define		THRD_MESSAGE_EXIT			  WM_APP + 2 //스크린락 해지
#define     THRD_MESSAGE_NOSIGNAL_DEVICE  WM_APP + 3 //블루투스 꺼져 있음
#define     THRD_MESSAGE_NODEVICE         WM_APP + 4 //연결된 기기 없음
#define     THRD_CLOSE					  WM_APP + 5  //모든쓰레드종료

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


	char *APP_Thread_Msg = "(text.cpp)   블루투스 체크 쓰레드 생성완료 "; // 프록시파라미터
	char *APP_Thread_Msg1 = "(text.cpp)   블루투스 체크 쓰레드 생성완료111 "; // 프록시파라미터

	MAPP_Thread1 = CreateThread(NULL, 0, ThreadProc_MAPP1, (LPVOID)APP_Thread_Msg, 0, &MAPP_Thread_ID1);
	MAPP_Thread = CreateThread(NULL, 0, ThreadProc_MAPP, (LPVOID)APP_Thread_Msg1, 0, &MAPP_Thread_ID);


	WaitForSingleObject(MAPP_Thread1, INFINITE);
	WaitForSingleObject(MAPP_Thread, INFINITE);

	printf("(text.cpp)   시작2\n");

	//Monitoring::MonitoringFuncs::startMon(MAPP_Thread_ID);
	//Monitoring::MonitoringFuncs::startMon(MAPP_Thread_ID);




	return 0;
}

//void M2_start(){
//	HANDLE MAPP_Thread1;
//	printf("text의 M_start진입\n");
//
//	char *APP_Thread_Msg = "블루투스 체크 쓰레드 생성완료 "; // 프록시파라미터
//
//	MAPP_Thread1 = CreateThread(NULL, 0, ThreadProc_MAPP1, (LPVOID)APP_Thread_Msg, 0, &MAPP_Thread_ID1);
//
//	WaitForSingleObject(MAPP_Thread1, INFINITE);
//	printf("M_start에서 쓰레드 생성후\n");
//}
//
//void M_start(){
//	HANDLE MAPP_Thread;
//	printf("text의 M_start진입111\n");
//
//	char *APP_Thread_Msg = "블루투스 체크 쓰레드 생성완료111 "; // 프록시파라미터
//	
//	MAPP_Thread = CreateThread(NULL, 0, ThreadProc_MAPP, (LPVOID)APP_Thread_Msg, 0, &MAPP_Thread_ID);
//
//	WaitForSingleObject(MAPP_Thread, INFINITE);
//	printf("M_start에서 쓰레드 생성후111\n");
//}

void MSGfunc()
{

	MSG msg = { 0 };
	printf("(text.cpp)   메인함수 메세지접근\n");
	while (!MSG_EXIT){
		//printf("에러1 ::::%d\n", GetLastError());
		printf("(text.cpp)   메인함수 메세지접근 후 whlie접근\n");
		Sleep(1000);
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//printf("에러2 :::%d\n", GetLastError());
			printf("(text.cpp)   메세지도는중\n");
			if (msg.message == WM_APP + 1)
			{
				printf("(text.cpp)   스크린락\n");
			}
			if (msg.message == WM_APP + 2)
			{
				printf("(text.cpp)   스크린락해제\n");
			}
			if (msg.message == WM_APP + 3)
			{
				printf("(text.cpp)   블루투스 꺼져있음\n");
			}
			if (msg.message == WM_APP + 4)
			{
				printf("(text.cpp)   연결된 기기 없음\n");
			}
			if (msg.message == WM_APP + 5)
			{
				printf("(text.cpp)   모든 쓰레드 종료\n");
				PostThreadMessage(MAPP_Thread_ID, WM_APP + 5, 0, 0);
			}

			DispatchMessage(&msg);
			printf("에러3 :::%d\n", GetLastError());
		}
		printf("(text.cpp)MSG쓰레드 마지막부\n");

	}
}

DWORD WINAPI ThreadProc_MAPP(LPVOID M_param)
{
	char *TP_Msg = (char *)M_param; // 프록시로 들어온 파라미터

	printf("(text.cpp)   %s :: 메세지 쓰레드 ID: %d\n", TP_Msg, MAPP_Thread_ID);
	///이부분부터 수정 한쓰레드에는 한 함수만 들어있어야함 그래서 쓰레드 하나 더 만들고 테스트해보기
	Monitoring::MonitoringFuncs::InitMon(MAPP_Thread_ID1);

	printf("(text.cpp)   스레드 나가기전\n");
	return (DWORD)M_param;
}


DWORD WINAPI ThreadProc_MAPP1(LPVOID M_param)
{
	char *TP_Msg = (char *)M_param; // 프록시로 들어온 파라미터
	MSGfunc();
	printf("(text.cpp)  %s :: 메세지 쓰레드 ID: %d\n", TP_Msg, MAPP_Thread_ID1);
	///이부분부터 수정 한쓰레드에는 한 함수만 들어있어야함 그래서 쓰레드 하나 더 만들고 테스트해보기
	printf("(text.cpp)  스레드 나가기전111\n");
	return (DWORD)M_param;
}















//void M_start(){
//	HANDLE MMSG_Thread;
//
//	char *MMSG_Thread_Msg = "블루투스 체크 쓰레드 생성완료 "; // 프록시파라미터
//
//	MMSG_Thread = CreateThread(NULL, 0, ThreadProc_MSG, (LPVOID)MMSG_Thread_Msg, 0, &MAPP_Thread_ID);
//
//	WaitForSingleObject(MMSG_Thread, INFINITE);
//}
//DWORD WINAPI ThreadProc_MSG(LPVOID M_param)
//{
//	char *TP_Msg = (char *)M_param; // 프록시로 들어온 파라미터
//
//	printf("%s :: 메세지 확인 쓰레드 ID: %d\n", TP_Msg, MAPP_Thread_ID);
//	
//
//	return (DWORD)M_param;
//}