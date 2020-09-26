#include "Getkey.h"

DWORD GetProcessIDByName(const char* pName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (strcmp(pe.szExeFile, pName) == 0) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
		//printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);
	}
	CloseHandle(hSnapshot);
	return 0;
}

//ͨ��pid��ģ������ȡ��ַ
PVOID GetProcessImageBase(DWORD dwProcessId, const char* dllName)
{
	PVOID pProcessImageBase = NULL;
	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);
	// ��ȡָ������ȫ��ģ��Ŀ���
	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (INVALID_HANDLE_VALUE == hModuleSnap)
	{
		return pProcessImageBase;
	}
	// ��ȡ�����е�һ����Ϣ
	BOOL bRet = ::Module32First(hModuleSnap, &me32);
	while (strcmp((char*)me32.szModule, dllName) != 0)
	{
		Module32Next(hModuleSnap, &me32);
	}
	pProcessImageBase = (PVOID)me32.modBaseAddr;
	// �رվ��
	::CloseHandle(hModuleSnap);
	return pProcessImageBase;
}

//�ж�΢�Ű汾��ȷ��ƫ��
DWORD IsWxVersionValid(WCHAR *VersionFilePath)
{
	string asVer = "";
	VS_FIXEDFILEINFO* pVsInfo;
	unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	int iVerInfoSize = GetFileVersionInfoSizeW(VersionFilePath, NULL);
	if (iVerInfoSize != 0) {
		char* pBuf = new char[iVerInfoSize];
		if (GetFileVersionInfoW(VersionFilePath, 0, iVerInfoSize, pBuf)) {
			if (VerQueryValue(pBuf, TEXT("\\"), (void**)&pVsInfo, &iFileInfoSize)) {
				//���汾2.9.0.123
				//2
				int s_major_ver = (pVsInfo->dwFileVersionMS >> 16) & 0x0000FFFF;
				//9
				int s_minor_ver = pVsInfo->dwFileVersionMS & 0x0000FFFF;
				//0
				int s_build_num = (pVsInfo->dwFileVersionLS >> 16) & 0x0000FFFF;
				//123
				int s_revision_num = pVsInfo->dwFileVersionLS & 0x0000FFFF;

				//�Ѱ汾����ַ���
				strstream wxVer;
				wxVer << s_major_ver << "." << s_minor_ver << "." << s_build_num << "." << s_revision_num;
				wxVer >> asVer;
			}
		}
		delete[] pBuf;
	}
	printf("var = %s\n", asVer.c_str());
	if (asVer == wxVersoin1)
		return version1;
	else if (asVer == wxVersoin2)
		return version2;
	else if (asVer == wxVersoin3)
		return version3;
	else
		return 0;
}

//��ȡ���ݿ���Կ
int GetdbKey(unsigned char *databasekey, unsigned char *wxid)
{
	HWND phandle = FindWindow("WeChatMainWndForPC", NULL);//��ȡ���
	if (!phandle)
	{
		return 0;
	}
	DWORD pid;
	GetWindowThreadProcessId(phandle, &pid);//��ȡ����id
	if (!pid)
	{
		return 0;
	}
	printf("pid = %d\n", pid);
	HANDLE mProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (mProc == NULL)
	{
		return 0;
	}

	wchar_t path[MAX_PATH];
	if (!GetModuleFileNameExW(mProc, NULL, path, MAX_PATH))
		return 0;
	else
	{
		for (int i = 0; i < sizeof(path); ++i) {

			if (path[i] == '.')
			{
				wcscpy_s(&path[i], sizeof(L"Win.dll"), L"Win.dll");
				break;
			}
		}
	}

	DWORD WxDatabaseKey = IsWxVersionValid(path);
	if (!WxDatabaseKey)
		return 0;

	//��ȡWeChatWin�Ļ�ַ
	DWORD base_address = (DWORD)GetProcessImageBase(pid, "WeChatWin.dll");
	printf("dllbase_address = %p\n", base_address);

	DWORD dwKeyAddr = base_address + WxDatabaseKey;
	printf("Addr = %p\n", dwKeyAddr);

	int addr = 0;
	DWORD dwOldAttr = 0;
	//��ȡ���ݿ���Կ
	ReadProcessMemory(mProc, (LPCVOID)dwKeyAddr, &addr, 4, NULL);
	printf("key addr = %x\n", addr);

	ReadProcessMemory(mProc, (LPCVOID)addr, databasekey, 0x20, NULL);

	unsigned char wxname[100] = { 0 };

	//��ȡ΢���ǳ�
	DWORD wxAddr = base_address + wxname_addr3;
	printf("wxAddr = %x\n", wxAddr);
	ReadProcessMemory(mProc, (LPCVOID)wxAddr, wxname, 100, NULL);
	printf("%s\n", wxname);

	//��ȡ΢��id
	wxAddr = base_address + wxid_addr3;
	printf("wxAddr = %x\n", wxAddr);
	ReadProcessMemory(mProc, (LPCVOID)wxAddr, &addr, 4, NULL);
	printf("wxid addr = %x\n", addr);

	ReadProcessMemory(mProc, (LPCVOID)addr, wxid, 100, NULL);
	printf("%s\n", wxid);

	//��ӡ��Կ
	for (int i = 0; i < 0x20; i++)
	{
		printf("%02x", databasekey[i]);
	}
	return 1;
}