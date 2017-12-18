#pragma once

#include<WinSock2.h>
//#include<thread>
#include"Behavior.h"
#include<queue>
#include<string>
enum class ClientDialogStatus{
  RUN,
  STOP
};
class CClientDialog
{
public:
	CClientDialog(SOCKET *s, sockaddr_in *addres);
	~CClientDialog();
	void start(); //метод, которым пользуются все, который внутри запускает поток в котором выполняется run()
	void setBehavior(Behavior *behavior);
  ClientDialogStatus getStatus() const { return m_status; }
private:
  void run();
	void write();
  void action();
  void read();
  void stop();
  ClientDialogStatus m_status;
	HANDLE thread;
  HANDLE m_readThread;
  HANDLE m_actionThread;
  HANDLE m_writeThread;
  HANDLE m_forActionQueueMutex;
  HANDLE m_forWriteQueueMutex;
  std::queue<std::string> m_forAction;
  std::queue<std::string> m_forWrite;
	SOCKET *m_socket;
	sockaddr_in *m_addres;
	//std::thread *thread;
	Behavior *m_behavior;
	static DWORD WINAPI WriteThreadFunction(LPVOID _lpParam);//чтобы запустить run
  static DWORD WINAPI ActionThreadFunction(LPVOID _lpParam);//чтобы запустить run
  static DWORD WINAPI ReadThreadFunction(LPVOID _lpParam);//чтобы запустить run
  static DWORD WINAPI RunThreadFunction(LPVOID _lpParam);//чтобы запустить run
  static const int MAX_BUF_SIZE = 1024;
	//char buffer[1024];
};

