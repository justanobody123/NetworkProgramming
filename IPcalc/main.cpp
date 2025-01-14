#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "resource.h"
#include <CommCtrl.h>
#include <cstdio>
#include <iostream>
using std::cout;
using std::endl;

//#define HAPPY_NEW_YEAR
#define IDC_COLUMN_NETWORK_ADDRESS			2000
#define IDC_COLUMN_BROADCAST_ADDRESS		2001
#define IDC_COLUMN_NUMBER_OF_IP_ADDRESSED	2002
#define IDC_COLUMN_NUMBER_OF_HOSTS			2003



BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcSubnets(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LPSTR FormatIPaddress(CONST CHAR * sz_message, DWORD IPaddress);
LPSTR FormatMessageWithNumber(CONST CHAR* sz_message, DWORD number);
DWORD PrefixToSubnetMask(int prefix);

VOID INITLVCcolumn(LPLVCOLUMN column, LPSTR text, INT subitem);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		//system("chcp 1251");
		//InitCommonControls();
		HWND hUpDown = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(30, 1));
	}
	break;

	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0, dwIPmask = 0;
		CHAR sz_prefix[3]{};
		CONST INT SIZE = 256;
		//std::cout << LOWORD(wParam) << "\n";
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_SUBNETS:
			DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_SUBNETS), hwnd, (DLGPROC)DlgProcSubnets, 0);
			break;
		case IDCANCEL:
			FreeConsole();
			EndDialog(hwnd, 0);
			break;

		case IDOK:
		{
			CHAR sz_info[SIZE] = "Info:\n";
			CHAR sz_brodcast_address[SIZE]{};
			CHAR sz_network_address[SIZE]{};
			CHAR sz_number_of_hosts[SIZE]{};

			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			DWORD dwNetwokrAddress = dwIPaddress & dwIPmask;
			DWORD dwBroadcastAddress = dwNetwokrAddress | ~dwIPmask;
			strcat(sz_info, FormatIPaddress("Адрес сети", dwNetwokrAddress));
			strcat(sz_info, FormatIPaddress("Широковещательный адрес", dwBroadcastAddress));
			/*strcat(sz_info, "\nКоличество IP-адресов:\t");
			strcat(sz_info, _itoa(dwBroadcastAddress - dwNetwokrAddress + 1, sz_number_of_hosts, 10));
			strcat(sz_info, "\nКоличество узлов:\t");
			strcat(sz_info, _itoa(dwBroadcastAddress - dwNetwokrAddress - 1, sz_number_of_hosts, 10));*/;
			strcat(sz_info, FormatMessageWithNumber("Количество IP-адресов", dwBroadcastAddress - dwNetwokrAddress + 1));
			strcat(sz_info, FormatMessageWithNumber("Количество узлов", dwBroadcastAddress - dwNetwokrAddress - 1));
			HWND hInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
			SendMessage(hInfo, WM_SETTEXT, 0, (LPARAM)sz_info);
		}
		break;

		case IDC_IPADDRESS:
		{
			//cout << HIWORD(wParam) << endl;
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
			{

#ifdef HAPPY_NEW_YEAR
				//МОЕ
				//DWORD address = 0;
				//SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&address);
				//short int msb = address >> 24; //MSB - most significant byte
				//DWORD mask = msb > 0 and msb <= 127 ? 0xFF000000 : msb >= 128 and msb <= 191 ? 0xFFFF0000 : msb >= 192 and msb <= 223 ? 0XFFFFFF00 : 0; 
				//SendMessage(hIPmask, IPM_SETADDRESS, 0, mask);  
#endif // HAPPY_NEW_YEAR


				//КЛАСС
				//DWORD dwIPaddress = 0, dwIPmask = 0;
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				BYTE first = FIRST_IPADDRESS(dwIPaddress);
				cout << first << "\n";
				if (first < 128)		dwIPmask = MAKEIPADDRESS(255, 0, 0, 0);
				else if (first < 192)	dwIPmask = MAKEIPADDRESS(255, 255, 0, 0);
				else if (first < 224)	dwIPmask = MAKEIPADDRESS(255, 255, 255, 0);
				SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
			}
			break;
			}
		}
		break;

		case IDC_IPMASK:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
			{
				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
				int i = 1;
				for (; dwIPmask <<= 1; i++);
				sprintf(sz_prefix, "%i", i);
				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)sz_prefix);
				break;
			}
			}
		}
		break;
		case IDC_EDIT_PREFIX:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hEditPrefix, WM_GETTEXT, 3, (LPARAM)sz_prefix);
				DWORD dwIPprefix = atoi(sz_prefix);
				UINT dwIPmask = UINT_MAX;
				//TODO:
				dwIPmask <<= (32 - dwIPprefix);
				if (dwIPprefix) SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);

			}
		}
		break;
#ifdef HAPPY_NEW_YEAR
		//case IDC_IPMASK:
//{
//	/*switch (HIWORD(wParam))
//	{
//	case EN_CHANGE:
//	{
//		cout << "CHANGE OF MASK\n";
//		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
//		HWND hMaskControl = GetDlgItem(hwnd, IDC_IPMASK);
//		DWORD mask = 0;
//		SendMessage(hMaskControl, IPM_GETADDRESS, 0, (LPARAM)&mask);
//		short int prefix = 0;
//		char buffer[3];
//		
//		for (int i = 31; i >= 0; i--)
//		{
//			if ((mask >> i) & 1)
//			{
//				prefix++;
//			}
//			else
//			{
//				break;
//			}
//		}
//		_itoa(prefix, buffer, 10);
//		SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)buffer);
//	}
//		break;
//	default:
//		break;
//	}*/
//}
//	break;
//case IDC_EDIT_PREFIX:
//{
//	//switch (HIWORD(wParam))
//	//{
//	//case EN_CHANGE:
//	//{
//	//	cout << "CHANGE OF PREFIX\n";
//	//	//вытащить префикс
//	//	//преобразовать его в дворд
//	//	//запихать указатель на дворд в маску
//	//	HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
//	//	HWND hMaskControl = GetDlgItem(hwnd, IDC_IPMASK);
//	//	int prefix = 0; 
//	//	const int MAX_SIZE = 3;
//	//	char buffer[MAX_SIZE];
//	//	SendMessage(hEditPrefix, WM_GETTEXT, MAX_SIZE, (LPARAM)buffer);
//	//	prefix = atoi(buffer);
//	//	cout << "prefix: " << prefix << "\n";
//	//	DWORD maskAddress = PrefixToSubnetMask(prefix);
//	//	cout << "maskAddress: " << maskAddress << "\n";
//	//	//Надо сформировать 16-ричный dword, в десятичной адрес не принимается.
//	//	//SendMessage(hMaskControl, IPM_SETADDRESS, 0, maskAddress);
//	//}
//	//	break;
//	//default:
//	//	break;
//	//}
//}  
#endif // HAPPY_NEW_YEAR

		}
		break;
	}
	case WM_CLOSE:
		FreeConsole();
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}


DWORD PrefixToSubnetMask(int prefix)
{
	if (prefix < 1 || prefix > 30) {
		return 0;
	}
	// Генерация маски подсети с заданным префиксом
	return (0xFFFFFFFF << (32 - prefix));
}
LPSTR FormatIPaddress(CONST CHAR * sz_message, DWORD IPaddress)
{
	CHAR sz_buffer[256];
	sprintf(sz_buffer, "%s:\t%i.%i.%i.%i\n", sz_message,
		FIRST_IPADDRESS(IPaddress),
		SECOND_IPADDRESS(IPaddress),
		THIRD_IPADDRESS(IPaddress),
		FOURTH_IPADDRESS(IPaddress));
	return sz_buffer;
}
LPSTR FormatMessageWithNumber(CONST CHAR* sz_message, DWORD number)
{
	CHAR sz_buffer[256]{};
	sprintf(
		sz_buffer,
		"%s:\t%i\n", sz_message, number
	);
	return sz_buffer;
}

BOOL CALLBACK DlgProcSubnets(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg)
	{
	case WM_INITDIALOG:
		static HWND hList = GetDlgItem(hwnd, IDC_LIST_SUBNETS);
		static LVCOLUMN lvcNetworkAddress;
		static LVCOLUMN lvcBroadcastAddress;
		static LVCOLUMN lvcNumberOfIPaddresses;
		static LVCOLUMN lvcNumberOfHosts;
		INITLVCcolumn(&lvcNetworkAddress, (LPSTR)"Адрес сети", 2000);
		INITLVCcolumn(&lvcBroadcastAddress, (LPSTR)"Широковещательный адрес", 2001);
		INITLVCcolumn(&lvcNumberOfIPaddresses, (LPSTR)"Количетво IP-адресов", 2002);
		INITLVCcolumn(&lvcNumberOfHosts, (LPSTR)"Количество узлов", 2003);
		SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvcNetworkAddress);
		SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvcBroadcastAddress);
		SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvcNumberOfIPaddresses);
		SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvcNumberOfHosts);
		break;
	case WM_COMMAND:
		/*switch (LOWORD(wParam))
		{
		case :
			break;
		}
		break;*/
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	default:
		break;
	}
	return FALSE;
}
VOID INITLVCcolumn(LPLVCOLUMN column, LPSTR text, INT subitem)
{
	ZeroMemory(column, sizeof(LVCOLUMN));
	column->mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM;
	column->cx = 150;
	column->pszText = text;
	column->iSubItem = subitem;
	column->fmt = LVCFMT_LEFT;
}
