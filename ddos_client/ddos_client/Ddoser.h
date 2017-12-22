#pragma once
#include"Client.h"
#include<vector>
class CDdoser
{
public:
  CDdoser(char *_ip, WORD _port, size_t _clientCount, WORD _sleepTime, DWORD _msgLength);
  ~CDdoser();
  void run();
private:
  void initClients();
  char *m_ip = NULL;
  WORD m_port;
  std::vector<CClient*>m_clients;
  WORD m_sleepTime;
  DWORD m_msgLength;
  size_t m_clientsCount;
  static DWORD WINAPI s_runClient(LPVOID _lpParam);
};

