
#include "Server.h"

#include<iostream>

/////////////////////////////////////////////////////////////
CServer::CServer()
{
	controller.startTimer();
}
/////////////////////////////////////////////////////////////

CServer::~CServer()
{
	for (auto item : connections)
	{
		item.second->close();
		delete item.second;
	}
}
/////////////////////////////////////////////////////////////
//DWORD WINAPI RunConnection(LPVOID lpParam)
//{
//	CServerConnection *connection = (CServerConnection*)connection;
//	if (connection)
//	{
//		connection->handle();
//	}
//}
/////////////////////////////////////////////////////////////
// создаем новое соединение
bool CServer::addConnection(WORD port)
{
	CServerConnection *newConnection = new CServerConnection();
	//если смогли запустить соединение на данном порту
	if (newConnection->start(port))
	{
		//вывести в отдельный поток
		newConnection->run();
		connections.insert(std::pair<WORD, CServerConnection*>(port, newConnection));
		//добавляем в контроллер, чтобы проверял
		controller.addConnection(port, newConnection);
		return true;
	}
	else
	{
		delete newConnection;
		return false;
	}
	
}
/////////////////////////////////////////////////////////////
//удаляем соединени
void CServer::deleteConnection(WORD port)
{
	auto it = connections.find(port); //Iterator
	//удаляем из контроллера
	controller.deleteConnection(it->first);
	CServerConnection* connection = it->second;
	if (connection)
	{
		connection->close();
		delete connection;
	}
	connections.erase(it);
}
/////////////////////////////////////////////////////////////
void CServer::setBehavior(WORD port, Behavior *behavior)
{
	//устанавливаем поведение
	CServerConnection* connection = connections[port];
	if (connection)
	{
		std::cout << "SetBehavior" << std::endl;
		connection->setBehavior(behavior);
	}
}
/////////////////////////////////////////////////////////////