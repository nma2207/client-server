
#include "Behavior.h"
#include<iostream>


/////////////////////////////////////////////////////////////
void ExpandWordBehavior::action(char *str)
{
	int n = strlen(str);
	for (int i = 0, j = n - 1; i < j; i++, j--)
	{
		char t = str[i];
		str[i] = str[j];
		str[j] = t;
	}
}

Behavior* ExpandWordBehavior::getNew()
{
	return new ExpandWordBehavior;
}
/////////////////////////////////////////////////////////////

void UpperWordBehavior::action(char *str)
{
	int d = 'A' - 'a';
	for (int i = 0; str[i] != '\0'; i++)
	{
		if ('a' <= str[i] && str[i] <= 'z')
			str[i] += d;
	}
}
/////////////////////////////////////////////////////////////

Behavior* UpperWordBehavior::getNew()
{
	return new UpperWordBehavior;
}

/////////////////////////////////////////////////////////////
void DownWordBehavior::action(char *str)
{
	int d = 'a' - 'A';
	for (int i = 0; str[i] != '\0'; i++)
	{
		if ('A' <=str[i] && str[i] <= 'Z')
			str[i] += d;
	}
}

/////////////////////////////////////////////////////////////
Behavior* DownWordBehavior::getNew()
{
	return new DownWordBehavior;
}



/////////////////////////////////////////////////////////////
void ChangeRegisterBehavior::action(char *str)
{
	int d = 'A' - 'a';
	for (int i = 0; str[i] != '\0'; i++)
	{
		if ('a' <= str[i] && str[i] <= 'z')
		{
			str[i] += d;
		}
		else if ('A' <= str[i] && str[i] <= 'Z')
		{
			str[i] -= d;
		}
	}
}
/////////////////////////////////////////////////////////////

Behavior* ChangeRegisterBehavior::getNew()
{
	return new ChangeRegisterBehavior;
}