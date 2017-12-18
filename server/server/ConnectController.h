#pragma once
#include<map>
#include"ServerConnection.h"




class ConnectController
{
private:
	std::map<WORD, CServerConnection*> m_connections;
	HANDLE m_hTimer;
	HANDLE m_hTimerQueue;
	void checkConnections();


public:
	void startTimer();//запускаем таймер, в котором создается таймер с CheckConnectionsByTimer(...)
	void stopTimer();
	void addConnection(WORD port, CServerConnection* connection);
	void deleteConnection(WORD port);
	ConnectController();
	~ConnectController();

	//здесь вызывается ConnectController::checkConnections()
	static VOID CALLBACK CheckConnectionsByTimer(PVOID lpParam, BOOL TimerOrWaitFired); 


};

