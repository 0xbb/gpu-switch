// Copyright (c) 2015 Bruno Bierbaumer
// PrintError and ObtainPrivileges borrowed from https://github.com/black3r/efireboot

#include "stdafx.h"
#include <windows.h>

#define VARIABLE_ATTRIBUTE_NON_VOLATILE 0x00000001
#define VARIABLE_ATTRIBUTE_BOOTSERVICE_ACCESS 0x00000002
#define VARIABLE_ATTRIBUTE_RUNTIME_ACCESS 0x00000004

void PrintError(DWORD errorCode) {
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	printf("Error code was: %ld\n", errorCode);
	printf("Error message was: %ws", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void ObtainPrivileges(LPCTSTR privilege) {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	BOOL res;
	DWORD error;
	// Obtain required privileges
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		printf("OpenProcessToken failed!\n");
		PrintError(GetLastError());
		exit(GetLastError());
	}

	res = LookupPrivilegeValue(NULL, privilege, &tkp.Privileges[0].Luid);
	if (!res) {
		printf("LookupPrivilegeValue failed!\n");
		PrintError(GetLastError());
		exit(GetLastError());
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	error = GetLastError();
	if (error != ERROR_SUCCESS) {
		printf("AdjustTokenPrivileges failed\n");
		PrintError(error);
		exit(error);
	}

}

BOOL switch_gpu(char integrated) {
	printf("Switching: to %s GPU\n", integrated ? "integrated" : "dedicated");

	LPCTSTR lpName = (LPCTSTR) TEXT("gpu-power-prefs");
	LPCTSTR lpGuid = (LPCTSTR) TEXT("{FA4CE28D-B62F-4C99-9CC3-6815686E30F9}");
	char pValue[] = { integrated, 0x0, 0x0, 0x0 };
	DWORD nSize = 4;
	DWORD dwAttribures = VARIABLE_ATTRIBUTE_NON_VOLATILE | VARIABLE_ATTRIBUTE_BOOTSERVICE_ACCESS | VARIABLE_ATTRIBUTE_RUNTIME_ACCESS;

	BOOL status = SetFirmwareEnvironmentVariableEx(lpName, lpGuid, pValue, nSize, dwAttribures);
	
	printf("Switching: %s\n", status ? "Successful" : "Error");
	if (!status){
		PrintError(GetLastError());
	}

	return status;
}

int _tmain(int argc, _TCHAR* argv[]) {
	int status = 0;

	if (argc == 2) {
		ObtainPrivileges(SE_SYSTEM_ENVIRONMENT_NAME);

		if (_tcscmp(argv[1], TEXT("-i")) == 0 || _tcscmp(argv[1], TEXT("--integrated")) == 0) {
			status = switch_gpu(1);
		}
		else if (_tcscmp(argv[1], TEXT("-d")) == 0 || _tcscmp(argv[1], TEXT("--dedicated")) == 0) {
			status = switch_gpu(0);
		}
	}
	return 0;
}	

