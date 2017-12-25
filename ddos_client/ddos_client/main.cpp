#include<iostream>
#include"Ddoser.h"

int main()
{
  CDdoser *ddoser = new CDdoser("127.0.0.1", 8888, 1, 1000, 2000);
  ddoser->run();
  system("pause");
  return 0;
}