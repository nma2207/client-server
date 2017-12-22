
#include"Client.h"
#include<iostream>
#include<string>

int main()
{
	setlocale(0, "");
	CClient client;
	WORD port;
	char ip[20];
	std::cout << "write Ip-adress" << std::endl;
	std::cin >> ip;
	std::cout << "write the port" << std::endl;
	std::cin >> port;
	if (strcmp(ip, "localhost") == 0)
		strcpy_s(ip, "127.0.0.1");
	//std::cout << ip;
	if(client.connectToServer(ip, port))
		client.run();
	client.disconnect();
	return 0;
}