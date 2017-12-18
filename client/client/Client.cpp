
#include "Client.h"


#include"Client.h"
#include<iostream>
#include<string>
CClient::CClient()
{

}


CClient::~CClient()
{
}

bool CClient::connectToServer(char *ip_address, WORD port)
{
	if (FAILED(WSAStartup(MAKEWORD(2, 2), &m_wsaData)))
	{
		std::cout << "WSA startup error " << WSAGetLastError() << std::endl;
		return false;
	}
	if (INVALID_SOCKET == (m_socket = socket(AF_INET, SOCK_STREAM, NULL)))
	{
		std::cout << "Socket create error " << WSAGetLastError() << std::endl;
		return false;
	}
	m_addr.sin_addr.s_addr = inet_addr(ip_address);
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);

	if (FAILED(connect(m_socket, (sockaddr*)&m_addr, sizeof(m_addr))))
	{
		std::cout << "Cannot connect " << WSAGetLastError() << std::endl;
		return false;
	}

	char server_reply[2000];
	std::cout << "CONNECTED TO SERVER!" << std::endl;
	if (SOCKET_ERROR == recv(m_socket, server_reply, 2000, 0))
	{
		std::cout << "Cannot get reply " << WSAGetLastError() << std::endl;
		return false;
	}
	else
		std::cout << server_reply << std::endl;
	return true;


}

void CClient::disconnect()
{
	closesocket(m_socket);
	WSACleanup();
}
void CClient::run()
{
	setlocale(0, "");
	std::cout << "Client run!" << std::endl;
	
	char buf[1024];
	std::cin.ignore();
	while (true)
	{
		ZeroMemory(buf, 1024);
		
		std::cout << "=> ";
//		std::gets(buf);
		
		std::cin.getline(buf, 1024);
		//std::cout << "ad";

		if (strcmp(buf, "") == 0)
			continue;
		if (send(m_socket, buf, strlen(buf)+1, 0) == SOCKET_ERROR)
		{
			puts("Send failed");
			std::cout << WSAGetLastError() << std::endl;
      if (strcmp(buf, "exit") == 0)
        break;
		}
		
		//std::cout << "asd";
		recv(m_socket, buf, 1024, NULL);
		std::cout << buf << std::endl;
		
	}
  system("pause");
}


