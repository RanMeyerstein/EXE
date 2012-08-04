// ProRBT.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "Socket.h"

Socket _socket;
int size;

using namespace std;

struct PRORBTPARAMS
{
	_TCHAR Header[1],Barcode[14], Qty[4], SessionId[17], LineNum[5], TotalLines[5], Directive[2];
};


int _tmain(int argc, _TCHAR* argv[])
{
	PRORBTPARAMS ProRbtParams;
	wofstream ParFile;

	//Set Header
	ProRbtParams.Header[0] = '`';

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

		_socket.Connect("10.0.0.3",50004);
		_socket.Send((void*)&ProRbtParams, sizeof(ProRbtParams),0);
		_socket.Disconnect();
	}
	
	//temp ranm
	std::cin.get();

	ParFile.close();
	return 0;
}
