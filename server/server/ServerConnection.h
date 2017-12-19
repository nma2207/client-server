#pragma once
#include<WinSock2.h>
#include"Behavior.h"
#include<vector>
#include"ClientDialog.h"
#pragma comment(lib, "ws2_32.lib")

class CServerConnection
{
public:
	CServerConnection();
	~CServerConnection();
	bool start(WORD port);//открывает соединение на донном порту
	void close(); //закрывает его

	int checkSocket(); //проверка сокета
	void run(); //в потоке запускает handle()
	void setBehavior(Behavior *);
private:
	void handle(); //подсоединяет клиентов
  void deleteStopClients();
	std::vector<CClientDialog*> m_clients;
  std::vector<SOCKET*> m_sockets;
  std::vector<sockaddr_in*> m_address;
	SOCKET m_socket;
	WSADATA m_wData;
	WORD m_port;
	HANDLE m_thread;
	Behavior *m_behavior;
  HANDLE m_deleteTimer = NULL;
  HANDLE m_deleteTimerQueue = NULL;

	static DWORD WINAPI RunServerConnection(LPVOID lpParam); //создает поток. в котором вызовется handle()
  static VOID CALLBACK DeleteConnectionsByTimer(PVOID lpParam, BOOL TimerOrWaitFired);
};

