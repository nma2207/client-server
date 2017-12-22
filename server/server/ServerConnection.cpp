
#include "ServerConnection.h"
#include<iostream>
#include"ClientDialog.h"
#include<vector >

/////////////////////////////////////////////////////////////
CServerConnection::CServerConnection()
{
	m_thread = NULL;
  
}

/////////////////////////////////////////////////////////////
CServerConnection::~CServerConnection()
{
}

/////////////////////////////////////////////////////////////
bool  CServerConnection::start(WORD port)
{
  m_status = CServerConnectionStatus::RUN;
	m_port = port;
	if (FAILED(WSAStartup(MAKEWORD(2, 2), &m_wData)))
	{
		std::cout << "WSA Startup failed " << WSAGetLastError() << std::endl;
		return false;
	}

	sockaddr_in addres;
	ZeroMemory(&addres, sizeof(addres));

	//сервер будет доступен с любой клиентской машины
	addres.sin_addr.S_un.S_addr = INADDR_ANY;
	//устанавливаем порт
	addres.sin_port = htons(m_port);

	//будем использовать IPv4, для IPv6 надо AF_INET6
	addres.sin_family = AF_INET;
	

	//SOCK_STREAM - значит TCP
	// Для UDP надо SOCK_DGRAM
	if (SOCKET_ERROR == (m_socket = socket(AF_INET, SOCK_STREAM, NULL)))
	{
		std::cout << "Cannot create socket " << WSAGetLastError() << std::endl;
		return false;
	}
	//сажаем на данный адрес
	if (SOCKET_ERROR == bind(m_socket, (sockaddr*)&addres, sizeof(addres)))
	{
		std::cout << "Cannot bind " << WSAGetLastError() << std::endl;
		return false;
	}
	//заставляем слушать
	if (SOCKET_ERROR == listen(m_socket, SOMAXCONN))
	{
		std::cout << "Cannot listen " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Server started in " <<m_port<<" port!"<< std::endl;
	return true;
}

/////////////////////////////////////////////////////////////
void  CServerConnection::close()
{
	if (m_thread != NULL)
	{
		WaitForSingleObject(m_thread, INFINITE);
		CloseHandle(m_thread);
		delete[]m_thread;
		m_thread = NULL;
	}
	closesocket(m_socket);
	WSACleanup();
	std::cout << "Server stopped!" << std::endl;
}
/////////////////////////////////////////////////////////////
//
//void ClientDialogRun(CClientDialog *dialog)
//{
//	printf("Start run client dialog");
//	//CClientDialog *dialog = (CClientDialog*)lParam;
//	if (dialog)
//		dialog->run();
//}
/////////////////////////////////////////////////////////////
void  CServerConnection::handle()
{

	//HANDLE *threads = new HANDLE[100];
	const WORD MAX_CLIENT_COUNT = 100;

  m_clients.reserve(MAX_CLIENT_COUNT);
  m_sockets.reserve(MAX_CLIENT_COUNT);
  m_address.reserve(MAX_CLIENT_COUNT);
  m_deleteTimer = CreateTimerQueue();
  CreateTimerQueueTimer(&m_deleteTimer, m_deleteTimerQueue, (WAITORTIMERCALLBACK)&DeleteConnectionsByTimer, (LPVOID)this, 0, 1000, 0);
	while (true)
	{
		//SOCKET accept_client;
		//sockaddr_in client_addr;
		sockaddr_in *addr = new sockaddr_in;
		//ddress[client_count] = new sockaddr_in();
		m_address.push_back(addr);
		int addr_len = sizeof(sockaddr_in);
		
		//ждет, пока подключится клиент
		SOCKET *socket = new SOCKET(accept(m_socket, (sockaddr*)addr, &addr_len));
    if (*socket != INVALID_SOCKET )
		{
			m_sockets.push_back(socket);
			std::cout << "Connect client: \n";
			printf("Address: %u.%u.%u.%u:%u\n",
				(unsigned char)addr->sin_addr.S_un.S_un_b.s_b1,
				(unsigned char)addr->sin_addr.S_un.S_un_b.s_b2,
				(unsigned char)addr->sin_addr.S_un.S_un_b.s_b3,
				(unsigned char)addr->sin_addr.S_un.S_un_b.s_b4,
				ntohs(addr->sin_port));
			CClientDialog *client = new CClientDialog(socket, addr);
			m_clients.push_back(client);
			//устанавливаем поведение
			client->setBehavior(m_behavior->getNew());
			//заупускаем
			client->start();
			
			//threads[thread_count] = CreateThread(NULL, 0,
			//	ClientDialogRun, (void*)clients[thread_count], 0, 0);
			//std::thread clientThread(ClientDialogRun, clients[thread_count]);
			//clientThread.detach();
      //deleteStopClients();
      if (m_clients.size() > MAX_CLIENT_COUNT)
      {
        m_status = CServerConnectionStatus::CRASHED;
        std::cout << "connection crashed" << std::endl;

        break;
      }
      Sleep(50);
			
		}

	}
  m_status = CServerConnectionStatus::CRASHED;
  std::cout << "\r\nSERVER CRASHED =( \r\n";
  std::cout << m_clients.size() << " clients is too much for me\r\n";
	//for (int i = 0; i < m_clients.size(); i++)
	//{
	//	delete m_sockets[i];
	//	delete m_address[i];
	//	//CloseHandle(threads[i]);
	//	delete m_clients[i];
	//}
 // m_clients.erase(m_clients.begin(), m_clients.end());
 // m_sockets.erase(m_sockets.begin(), m_sockets.end());
 // m_address.erase(m_address.begin(), m_address.end());
	

}
/////////////////////////////////////////////////////////////
DWORD WINAPI CServerConnection::RunServerConnection(LPVOID lpParam)
{
	CServerConnection *connection = (CServerConnection*)lpParam;
	if (connection)
	{
		connection->handle();
		return 0;
	}
	return 1;
}
/////////////////////////////////////////////////////////////
void CServerConnection::run()
{
	m_thread = CreateThread(0, 0, RunServerConnection, (LPVOID)this, 0, 0);

}
/////////////////////////////////////////////////////////////
void CServerConnection::setBehavior(Behavior *behavior)
{
	m_behavior = behavior;

}
/////////////////////////////////////////////////////////////
int CServerConnection::checkSocket()
{

	//возможно здесь надо испльзовт функци. select
	char sockbufsize[1024];
	int size = 1024;
	fd_set read_check_set;
	fd_set write_check_set;
	fd_set exctpt_check_set;
	
	//чистим
	FD_ZERO(&read_check_set);
	FD_ZERO(&write_check_set);
	FD_ZERO(&exctpt_check_set);
	
	//добавляем сокет в множества для проверок
	FD_SET(m_socket, &read_check_set);
	FD_SET(m_socket, &write_check_set);
	FD_SET(m_socket, &exctpt_check_set);
  if (m_status == CServerConnectionStatus::CRASHED)
  {
    return -1;
  }
	//можно еще в каждом запилить проверку на 
	//проверка на чтение
	if (SOCKET_ERROR == select(0, &read_check_set, 0, 0, NULL))
	{
		if (!FD_ISSET(m_socket, &read_check_set))
			return -1;
	}
	//проверка на запись
	if (SOCKET_ERROR == select(0, 0, &write_check_set, 0, NULL))
	{
		if (!FD_ISSET(m_socket, &write_check_set))
			return -1;
	}
	//проверка на существование что-ли
	if (SOCKET_ERROR == select(0, 0, 0, &exctpt_check_set, NULL))
	{
		if (!FD_ISSET(m_socket, &exctpt_check_set))
			return -1;
	}

	return 1;
}
/////////////////////////////////////////////////////////////
void CServerConnection::deleteStopClients()
{
  std::vector<int> stoppedIndex;
  stoppedIndex.reserve(m_clients.size());
  for (int i = 0; i < m_clients.size(); i++)
  {
    if (m_clients[i]->getStatus() == ClientDialogStatus::STOP)
    {
      stoppedIndex.push_back(i);
    }
  }

  for (int i = 0; i < stoppedIndex.size(); i++)
  {
    delete m_clients[stoppedIndex[i] - i];
    delete m_sockets[stoppedIndex[i] - i];
    delete m_address[stoppedIndex[i] - i];
    m_clients.erase(m_clients.begin() + stoppedIndex[i] - i);
    m_sockets.erase(m_sockets.begin() + stoppedIndex[i] - i);
    m_address.erase(m_address.begin() + stoppedIndex[i] - i);
    std::cout << "Delete " << stoppedIndex[i] << std::endl;
    
  }
  //std::cout << "I have " << m_clients.size() << " clients!\r\n";

}
/////////////////////////////////////////////////////////////
VOID CALLBACK CServerConnection::DeleteConnectionsByTimer(PVOID _lpParam, BOOL _timerOrWaitFired)
{
  CServerConnection *serverConnection = (CServerConnection*)_lpParam;
  if (serverConnection)
  {
    serverConnection->deleteStopClients();
  }
}
/////////////////////////////////////////////////////////////