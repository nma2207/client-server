#include<iostream>
#include"Ddoser.h"

int main()
{
  CDdoser *ddoser = new CDdoser("127.0.0.1", 8888, 50, 10, 1000);
  ddoser->run();
  system("pause");
  return 0;
}