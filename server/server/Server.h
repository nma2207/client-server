#pragma once
#include"ServerConnection.h"
#include<map>
#include"Behavior.h"
#include"ConnectController.h"
class CServer
{
	std::map<WORD, CServerConnection*> connections;
	ConnectController controller;
public:
	CServer();
	~CServer();

	bool addConnection(WORD port);
	void deleteConnection(WORD port);
	void setBehavior(WORD port, Behavior *behavior);

};

