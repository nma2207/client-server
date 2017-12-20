
#include "ConnectController.h"
#include<iostream>

/////////////////////////////////////////////////////////////
ConnectController::ConnectController()
{
	m_hTimer = NULL;
	m_hTimerQueue = NULL;
}

/////////////////////////////////////////////////////////////
ConnectController::~ConnectController()
{
	stopTimer();
}
/////////////////////////////////////////////////////////////
//добавляет на проверку то соединение, которое ему создает и дает сервер
void ConnectController::addConnection(WORD port, CServerConnection *connection)
{
	m_connections.insert(std::pair<WORD, CServerConnection*>(port, connection));
}


/////////////////////////////////////////////////////////////
//проверяет каждое соединение и если оно плохое, закрывавет его и запускает заново
void ConnectController::checkConnections()
{
  printf("Check connections %d\n", m_connections.size());
	for (auto item : m_connections)
	{
		int res;
		if ((res = item.second->checkSocket()) == -1)
		{
			printf("BAD\n");
			item.second->close();
			item.second->start(item.first);
      item.second->run();
		}
			
	}
}

/////////////////////////////////////////////////////////////
//для таймера. периодически запускаем проверку всех соединений
VOID CALLBACK ConnectController::CheckConnectionsByTimer(PVOID lpParam, BOOL TimerOrWaitFired)
{
	ConnectController *connect = (ConnectController*)lpParam;
	if (connect)
		connect->checkConnections();
}

/////////////////////////////////////////////////////////////
void ConnectController::startTimer()
{
	m_hTimer = CreateTimerQueue();
	CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue, (WAITORTIMERCALLBACK)&CheckConnectionsByTimer, (LPVOID)this, 0, 1000, 0);

}

/////////////////////////////////////////////////////////////
void ConnectController::stopTimer()
{
	DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, NULL);
	DeleteTimerQueue(m_hTimerQueue);
}
/////////////////////////////////////////////////////////////
//удаляем соединение из списка проверяемых
void ConnectController::deleteConnection(WORD port)
{
	auto it = m_connections.find(port);
	m_connections.erase(it);
}
/////////////////////////////////////////////////////////////