
#include "ClientDialog.h"
#include<iostream>

#pragma comment(lib, "ws2_32.lib")

/////////////////////////////////////////////////////////////
CClientDialog::CClientDialog(SOCKET *s, sockaddr_in *addres)
{
	m_socket = s;
	m_addres = addres;
	thread = NULL;
}
/////////////////////////////////////////////////////////////

CClientDialog::~CClientDialog()
{
	//delete m_socket;
	//delete m_addres;
  if (m_readThread)
  {
    TerminateThread(m_readThread,0);
    CloseHandle(m_readThread);
  }
  if (m_writeThread)
  {
    TerminateThread(m_writeThread, 0);
    CloseHandle(m_writeThread);
  }
  if (m_actionThread)
  {
    TerminateThread(m_actionThread, 0);
    CloseHandle(m_actionThread);
  }
  if (m_forActionQueueMutex)
  {
    CloseHandle(m_forActionQueueMutex);
  }
  if (m_forWriteQueueMutex)
  {
    CloseHandle(m_forWriteQueueMutex);
  }
  TerminateThread(thread, 0);
  CloseHandle(thread);
	shutdown(*m_socket, SD_SEND);
	closesocket(*m_socket);
}
/////////////////////////////////////////////////////////////

//void CClientDialog::runThread(CClientDialog* dialog)
//{
//	if (dialog)
//		dialog->run();
//}
/////////////////////////////////////////////////////////////
//DWORD WINAPI CClientDialog::RunClientDialog(LPVOID lpParam)
//{
//	CClientDialog* dialog = (CClientDialog*)lpParam;
//	if (dialog)
//	{
//		dialog->run();
//		return 0;
//	}
//	return 1;
//
//}
/////////////////////////////////////////////////////////////
//void CClientDialog::run()
//{
//	std::cout << "ClientDialog:: run!" << std::endl;
//	char buffer[1024];
//	ZeroMemory(buffer, 1024);
//	//message = "You have been watching\nBe prepared...\n";
//	strcpy_s(buffer, "You have been watching\nBe prepared...\n");
//	if (send(*m_socket, buffer, strlen(buffer) + 1, 0) == SOCKET_ERROR)
//	{
//		puts("Send failed");
//		std::cout << WSAGetLastError() << std::endl;
//
//	}
//	while (true)
//	{
//		ZeroMemory(buffer, 1024);
//		int k = recv(*m_socket, buffer, sizeof(buffer), NULL);
//		std::cout << strlen(buffer) << std::endl;
//		if (strcmp("exit", buffer) == 0)
//			break;
//		m_behavior->action(buffer);
//		if (send(*m_socket, buffer, strlen(buffer) + 1, 0) == SOCKET_ERROR)
//		{
//			puts("Send failed");
//			std::cout << WSAGetLastError() << std::endl;
//			break;
//		}
//		Sleep(30);
//	}
//}
/////////////////////////////////////////////////////////////
//при подсоединении клиента, ему выделяем поток, чтобы он там крутился
void CClientDialog::start()
{
  thread = CreateThread(0, 0, RunThreadFunction, (LPVOID)this, 0, 0);

}


/////////////////////////////////////////////////////////////
//устанавливаем поведение, на самом деле бесполезная штука, можно убрать и так же будет рабоатть,
//т.к. на лету нельзя изменить поведение.
void CClientDialog::setBehavior(Behavior *behavior)
{
	m_behavior = behavior;
	//WaitForSingleObject(thread, INFINITE);
	//CloseHandle(thread);
	//thread = CreateThread(0, 0, RunClientDialog, (LPVOID)this, 0, 0);
}
/////////////////////////////////////////////////////////////
void CClientDialog::run()
{
  std::cout << "ClientDialog::run\r\n";
  m_forWrite.push(std::string("You have been watching\nBe prepared...\n"));
  m_forActionQueueMutex = CreateMutex(NULL, FALSE, NULL);
  m_forWriteQueueMutex = CreateMutex(NULL, FALSE, NULL);
  m_writeThread = CreateThread(0, 0, WriteThreadFunction, this, 0, 0);
  m_readThread = CreateThread(0, 0, ReadThreadFunction, this, 0, 0);
  m_actionThread = CreateThread(0, 0, ActionThreadFunction, this, 0, 0);
  m_status = ClientDialogStatus::RUN;
  WaitForSingleObject(m_actionThread, INFINITE);
  stop();
  m_status = ClientDialogStatus::STOP;

  std::cout << "Waitd actions";

}
/////////////////////////////////////////////////////////////
DWORD WINAPI CClientDialog::RunThreadFunction(LPVOID _lpParam)
{
  CClientDialog* dialog = (CClientDialog*)_lpParam;
  if (dialog)
  {
    dialog->run();
    return 0;
  }
  return 1;
}
/////////////////////////////////////////////////////////////
DWORD WINAPI CClientDialog::WriteThreadFunction(LPVOID _lpParam)
{
  CClientDialog* dialog = (CClientDialog*)_lpParam;
  if (dialog)
  {
    dialog->write();
    return 0;
  }
  return 1;
}
/////////////////////////////////////////////////////////////
DWORD WINAPI CClientDialog::ActionThreadFunction(LPVOID _lpParam)
{
  CClientDialog* dialog = (CClientDialog*)_lpParam;
  
  if (dialog)
  {
    dialog->action();

    return 0;
  }
  return 1;
}
/////////////////////////////////////////////////////////////
DWORD WINAPI CClientDialog::ReadThreadFunction(LPVOID _lpParam)
{
  CClientDialog* dialog = (CClientDialog*)_lpParam;
  if (dialog)
  {
    dialog->read();
    return 0;
  }
  return 1;
}
/////////////////////////////////////////////////////////////

void CClientDialog::read()
{
  	std::cout << "ClientDialog:: run!" << std::endl;
  	char buffer[MAX_BUF_SIZE];
  	//ZeroMemory(buffer, MAX_BUF_SIZE);

  	while (true)
  	{
  		ZeroMemory(buffer, MAX_BUF_SIZE);
  		int k = recv(*m_socket, buffer, sizeof(buffer), NULL);
      if (k <= 0)
      {
        break;
      }
  		std::cout << strlen(buffer) << std::endl;
      std::cout << "Add to for action " << buffer << std::endl;
      s_onRead(this, std::string(buffer));
      //WaitForSingleObject(m_forActionQueueMutex, INFINITE);
      //m_forAction.push(std::string(buffer));
      //std::cout << "Add to for action " << buffer<<std::endl;
      //ReleaseMutex(m_forActionQueueMutex);
  	}
}
/////////////////////////////////////////////////////////////
void CClientDialog::action()
{
  std::string buf;
  while (true)
  {
    WaitForSingleObject(m_forActionQueueMutex, INFINITE);
    if (!m_forAction.empty())
    {
      buf = m_forAction.front();
      m_forAction.pop();
      ReleaseMutex(m_forActionQueueMutex);
      if (buf.compare("exit") == 0)
      {

        break;
      }
      char *buf_char = new char[buf.length()+1];
      strcpy_s(buf_char,buf.length()+1, buf.c_str());
      m_behavior->action(buf_char);
      WaitForSingleObject(m_forWriteQueueMutex, INFINITE);
      m_forWrite.push(std::string(buf_char));
      std::cout << "Add to for write " << buf_char << std::endl;
      ReleaseMutex(m_forWriteQueueMutex);

    }
    else
      ReleaseMutex(m_forActionQueueMutex);
  }
  std::cout << "stop";
}
/////////////////////////////////////////////////////////////
void CClientDialog::write()
{
  std::string buf;
  while (true)
  {
    WaitForSingleObject(m_forWriteQueueMutex, INFINITE);
    if (!m_forWrite.empty())
    {
      buf = m_forWrite.front();
      m_forWrite.pop();
      ReleaseMutex(m_forWriteQueueMutex);
      std::cout << "send " << buf << std::endl;
      if (send(*m_socket, buf.c_str(), buf.length() + 1, 0) == SOCKET_ERROR)
      {
      	puts("Send failed");
      	std::cout << WSAGetLastError() << std::endl;
      	break;
      }
    }
    else
    {
      ReleaseMutex(m_forWriteQueueMutex);
    }
  }
}
/////////////////////////////////////////////////////////////
void CClientDialog::stop()
{

  TerminateThread(m_writeThread, 0);
  TerminateThread(m_readThread, 0);
  TerminateThread(m_actionThread, 0);

  CloseHandle(m_actionThread);
  CloseHandle(m_writeThread);
  CloseHandle(m_readThread);

  ReleaseMutex(m_forActionQueueMutex);
  ReleaseMutex(m_forWriteQueueMutex);
  m_actionThread = NULL;
  m_writeThread = NULL;
  m_readThread = NULL;
  m_forActionQueueMutex = NULL;
  m_forWriteQueueMutex = NULL;

}
/////////////////////////////////////////////////////////////

void WINAPI CClientDialog::s_onRead(LPVOID _lpParam, const std::string& _msg)
{
  CClientDialog *client = (CClientDialog*)_lpParam;
  if (client)
  {
    client->onRead(_msg);
  }
}
//////////////////////////////////////////////////////////
void CClientDialog::onRead(const std::string& _msg)
{

    char *buf_char = new char[_msg.length() + 1];
    strcpy_s(buf_char, _msg.length() + 1, _msg.c_str());
    m_behavior->action(buf_char);
    //WaitForSingleObject(m_forWriteQueueMutex, INFINITE);
    //m_forWrite.push(std::string(buf_char));
    std::cout << "Add to for write " << buf_char << std::endl;
    s_onWrite(this, std::string(buf_char));
    //ReleaseMutex(m_forWriteQueueMutex);

}


void WINAPI CClientDialog::s_onWrite(LPVOID _lpParam, const std::string& _msg)
{
  CClientDialog *client = (CClientDialog*)_lpParam;
  if (client)
  {
    client->onWrite(_msg);
  }
}

void CClientDialog::onWrite(const std::string& _msg)
{
  std::cout << "send " << _msg << std::endl;
  if (send(*m_socket, _msg.c_str(), _msg.length() + 1, 0) == SOCKET_ERROR)
  {
    puts("Send failed");
    std::cout << WSAGetLastError() << std::endl;
  }
}