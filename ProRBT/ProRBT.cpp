// ProRBT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "windows.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	_TCHAR lBarcode[14], Qty[4], SessionId[17], LineNum[5], TotalLines[5], Directive[2];
	wofstream ParFile;

	ParFile.open ("ProRBT.log",ios::app);

	if (argc < 6) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
		std::cout << "Usage is [Barcode] [Qty] [ID] [Line no.] [Total no.] [Directive]\n"; // Inform the user of how to use the program
		std::cin.get();
		exit(0);
	} else {
		memcpy((void*)lBarcode, argv[1], 14 * sizeof (_TCHAR));
		memcpy((void*)Qty, argv[2], 4 * sizeof (_TCHAR));
		memcpy((void*)SessionId, argv[3], 17 * sizeof (_TCHAR));
		memcpy((void*)LineNum, argv[4], 5 * sizeof (_TCHAR));
		memcpy((void*)TotalLines, argv[5], 5 * sizeof (_TCHAR));
		memcpy((void*)Directive, argv[6], 2 * sizeof (_TCHAR));
		std::wcout << L"Bardcode [" << (LPCTSTR)argv[1] << "]" << endl;
		std::wcout << "Quantity [" << Qty << "]" << endl;
		std::wcout << "Session ID [" << SessionId << "]" << endl;
		std::wcout << "Line number[" << LineNum << "]" << endl;
		std::wcout << "Total number of lines [" << TotalLines << "]" << endl;
		if ((Directive[0] != '1') && (Directive[0] != '2'))
		std::wcout << L"Bad Directive[" << argv[6] << "] Expected: 1 - Query, 2 - Dispense" << endl;
		else
		std::cout << "Directive [" << (Directive[0] == '1' ? "Query": "Dispense") << "]" << endl;

		ParFile << lBarcode << L" ; " << Qty <<  L" ; " << SessionId <<  L" ; " << LineNum << 
			L" ; " << TotalLines <<  L" ; " << Directive << endl;
		ParFile.close();
		exit(0);
	}
	
	std::cin.get();
	return 0;
}



