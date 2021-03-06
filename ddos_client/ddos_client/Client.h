#pragma once

#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

class CClient
{
public:
	CClient(DWORD _msgLength, WORD _sleepTime, int _num);
	~CClient();
	bool connectToServer(char *ip_addr, WORD port);
	void run();
	void disconnect();
private:
	WSAData m_wsaData;
	sockaddr_in m_addr;
	SOCKET m_socket;
  DWORD m_msgLength;
  WORD m_sleepTime;
  int m_num;


};

