
#include"Server.h"
#include<iostream>
#include"Behavior.h"



int main()
{


	CServer server;

	//если смогли установить соединение на это порту, то ставим данное поведение
	if(server.addConnection(8888))
		server.setBehavior(8888, new ChangeRegisterBehavior());

	Sleep(5000);
	if(server.addConnection(8765))
		server.setBehavior(8765, new UpperWordBehavior());

	Sleep(5000);
	if(server.addConnection(8080))
		server.setBehavior(8080, new DownWordBehavior());
	
	Sleep(5000);
	if(server.addConnection(8008))
		server.setBehavior(8008, new ExpandWordBehavior());
	return 0;
}