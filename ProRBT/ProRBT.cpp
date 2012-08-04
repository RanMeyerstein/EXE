// ProRBT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "Socket.h"
#include "Iphlpapi.h"
#include "windows.h"

using namespace std;

typedef struct
{
	_TCHAR Barcode[14], Qty[4], SessionId[17], LineNum[5], TotalLines[5], Directive[2];
}PRORBTPARAMS, *pPRORBTPARAMS;

BOOL GetDesktopIp(char *ipAddr, int len);

Socket _socket;
char desktopIp[16] = {0};
int port = 50004;

DWORD WINAPI SocketThread(HANDLE hExitEvent)
{
	while (WaitForSingleObject(hExitEvent, 100) == WAIT_TIMEOUT)
	{
		if (_socket.IsConnected())
		{
			char *buffer = (char *)_socket.Receive(6);
			if (!buffer)
				continue;

			if (buffer[0] == '`')
			{

			}

			free(buffer);
		}
		else
		{
			_socket.Disconnect();
			_socket.Connect(desktopIp, port);
		}
	}

	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	PRORBTPARAMS ProRbtParams;
	wofstream ParFile;
	static HANDLE hSocketThread; 

	if (!GetDesktopIp(desktopIp, sizeof(desktopIp)))
	{
		std::wcout << "Failed to obtain IP. Exiting!" << endl;
		std::cin.get();
		exit(0);
	}

	hSocketThread = CreateThread(NULL, 0, SocketThread, NULL, 0, NULL);

	ParFile.open ("ProRBT.log",ios::app);

	if (argc < 6) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
		std::cout << "Usage is [Barcode 13] [Qty 3] [ID 16] [Line no. 4] [Total no. 4] [Directive 1]\n"; // Inform the user of how to use the program
		std::cin.get();
		exit(0);
	} else {
		memcpy((void*)ProRbtParams.Barcode, argv[1], 14 * sizeof (_TCHAR));
		memcpy((void*)ProRbtParams.Qty, argv[2], 4 * sizeof (_TCHAR));
		memcpy((void*)ProRbtParams.SessionId, argv[3], 17 * sizeof (_TCHAR));
		memcpy((void*)ProRbtParams.LineNum, argv[4], 5 * sizeof (_TCHAR));
		memcpy((void*)ProRbtParams.TotalLines, argv[5], 5 * sizeof (_TCHAR));
		memcpy((void*)ProRbtParams.Directive, argv[6], 2 * sizeof (_TCHAR));
		std::wcout << L"Bardcode [" << (LPCTSTR)argv[1] << "]" << endl;
		std::wcout << "Quantity [" << ProRbtParams.Qty << "]" << endl;
		std::wcout << "Session ID [" << ProRbtParams.SessionId << "]" << endl;
		std::wcout << "Line number[" << ProRbtParams.LineNum << "]" << endl;
		std::wcout << "Total number of lines [" << ProRbtParams.TotalLines << "]" << endl;
		if ((ProRbtParams.Directive[0] != '1') && (ProRbtParams.Directive[0] != '2'))
		std::wcout << L"Bad Directive[" << argv[6] << "] Expected: 1 - Query, 2 - Dispense" << endl;
		else
		std::cout << "Directive [" << (ProRbtParams.Directive[0] == '1' ? "Query": "Dispense") << "]" << endl;

		ParFile << ProRbtParams.Barcode << L" ; " << ProRbtParams.Qty <<  L" ; " << ProRbtParams.SessionId <<
			L" ; " << ProRbtParams.LineNum << L" ; " << ProRbtParams.TotalLines <<  L" ; " << ProRbtParams.Directive << endl;
	}
	
	CloseHandle(hSocketThread);
	ParFile.close();
	return 0;
}

BOOL GetDesktopIp(char *ipAddr, int len)
{
	BOOL ret = FALSE;

	IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)new BYTE[sizeof(IP_ADAPTER_INFO)];
	ULONG OutBufLen = 0;
	DWORD res = GetAdaptersInfo(pAdapterInfo, &OutBufLen);
	if (res == ERROR_BUFFER_OVERFLOW)
	{
		delete (pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)new BYTE[OutBufLen];
		res = GetAdaptersInfo(pAdapterInfo, &OutBufLen);
	}

	if (res == NO_ERROR)
	{
		IP_ADAPTER_INFO *info = pAdapterInfo;
		do
		{
			//RETAILMSG(1, (L"AdapterName = %s\r\n", info->AdapterName));
			//if (strstr(info->AdapterName, "USB CABLE"))
			if (strcmp(info->GatewayList.IpAddress.String,"0.0.0.0") != 0)
			{
				strncpy_s(ipAddr, len, info->IpAddressList.IpAddress.String, len);
				ret = TRUE;
				//break;
			}

			info = info->Next;
		}
		while (info);

	}

	delete (pAdapterInfo);
	return ret;
}
