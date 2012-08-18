// ProRBT.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <afxsock.h>    // For CSocket 
#include <iostream>
#include <fstream>


using namespace std;

struct PRORBTPARAMS
{
	_TCHAR Header[1],Barcode[14], Qty[4], SessionId[17], LineNum[5], TotalLines[5], Directive[2], CounterUnit[4];
};

wchar_t ServerIp[16];
HANDLE hSocketThread;
PRORBTPARAMS ProRbtParams;

void GetParamsFromConfFile()
{
	filebuf *inbuf;
	char content[100];
	streamsize size;

	ifstream ifs("ProRBTConf.txt");
	if (!ifs.bad())
	{
		inbuf = ifs.rdbuf();
		size = inbuf->sgetn(content,100);
		content[size] = '\0';
		CString stContent = content;
		CString StServerIp, StCounterId;

		if (stContent.Find(L"SERVER IP ADDRESS = ") != -1)
		{
			StServerIp = stContent.TrimLeft(L"SERVER IP ADDRESS = ");
			wsprintf(ServerIp,StServerIp.Left(StServerIp.Find('\n')));
			std::wcout <<L"Server IP from configuration file: " << ServerIp << endl;
		}
		else
		{
			std::cout <<"Server IP not found. using default 10.0.0.3" << endl;
			wsprintf(ServerIp,L"10.0.0.3");
		}

		if (stContent.Find(L"COUNTER ID = ") != -1)
		{
			StCounterId = stContent.TrimLeft(ServerIp); 
			int loc = StCounterId.GetLength() - 14;
			StCounterId = StCounterId.Right(loc);
			wsprintf(ProRbtParams.CounterUnit,StCounterId.GetString());
			std::wcout <<L"Counter Unit ID from configuration file: " << ProRbtParams.CounterUnit << endl;		
			//Search for Counter ID
		}
		else
		{
			wsprintf(ProRbtParams.CounterUnit,L"0");
			std::wcout <<L"Counter Unit ID default: " << ProRbtParams.CounterUnit << endl;
		}

		ifs.close();
	}
}

void SendtoTcpSever()
{
	CSocket echoClient;              // Socket descriptor

	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
	// Initialize the AfxSocket
	AfxSocketInit(NULL);

	// Create a reliable, stream socket using TCP
	if (!echoClient.Create()) {
		std::cout << "Create() failed";
	}
	else
	{
		// Establish the connection to the echo server
		if (!echoClient.Connect(ServerIp, 50004)) {
			std::wcout <<L"Connection to: " << ServerIp << L" failed" << endl; 
			AfxMessageBox(L"לא נוצר קשר עם שרת המנפיק",MB_OK | MB_TOPMOST | MB_RTLREADING | MB_ICONERROR);
		}
		else
		{
			std::cout <<"Connect() Success" << endl;
			//Set Header
			ProRbtParams.Header[0] = '`';
			// Send the string to the server
			if (echoClient.Send((void*)&(ProRbtParams.Header[0]), sizeof(ProRbtParams), 0) != sizeof(ProRbtParams)) { 
				std::cout <<"Send() sent a different number of bytes than expected" << endl;
			}

			std::cout <<"Message Sent to Server" << endl;

			_TCHAR echoBuffer[100]; // Buffer for echo string  

			int bytesRcvd; // Bytes read in single Receive()   

			if ((bytesRcvd = echoClient.Receive(echoBuffer, sizeof(echoBuffer))) <= 0) {
				std::cout <<"Receive() failed or connection closed prematurely";
			}

			cout << "bytesRcvd[" << bytesRcvd << "]" << endl;  // Setup to print the echoed string

			AfxMessageBox(echoBuffer,MB_OK | MB_TOPMOST | MB_RTLREADING);

			echoClient.Close(); // Close the connection
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	wofstream ParFile;

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
		if ((ProRbtParams.Directive[0] != '1') && (ProRbtParams.Directive[0] != '2')) {
			std::wcout << L"Bad Directive[" << argv[6] << "] Expected: 1 - Query, 2 - Dispense" << endl;
			goto END;
		}
		else
		std::cout << "Directive [" << (ProRbtParams.Directive[0] == '1' ? "Query": "Dispense") << "]" << endl;

		ParFile << ProRbtParams.Barcode << L" ; " << ProRbtParams.Qty <<  L" ; " << ProRbtParams.SessionId <<
			L" ; " << ProRbtParams.LineNum << L" ; " << ProRbtParams.TotalLines <<  L" ; " << ProRbtParams.Directive << endl;
        
		GetParamsFromConfFile();

		SendtoTcpSever();

	}
	//for debug ranm
	//std::cin.get();
END:
	ParFile.close();
	return 0;
}
