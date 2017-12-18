#pragma once
#include<WinSock2.h>
#pragma comment (lib, "ws2_32.lib")
class Behavior
{
public:
	virtual Behavior* getNew() =0;
	virtual void action(char *str) = 0;
	virtual ~Behavior(){};
};

class ExpandWordBehavior :public Behavior
{
	Behavior* getNew()override;
	void action(char *str) override;
};

class UpperWordBehavior : public Behavior
{
	Behavior* getNew()override;
	void action(char *str) override;
};

class DownWordBehavior : public Behavior
{
	Behavior* getNew()override;
	void action(char *str) override;
};

class ChangeRegisterBehavior : public Behavior
{
	Behavior* getNew()override;
	void action(char *str) override;
};
