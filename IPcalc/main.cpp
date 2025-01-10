#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "resource.h"
#include <CommCtrl.h>
#include <cstdio>
#include <iostream>
using std::cout;
using std::endl;

//#define HAPPY_NEW_YEAR

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD PrefixToSubnetMask(int prefix);

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
		//std::cout << LOWORD(wParam) << "\n";
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			FreeConsole();
			EndDialog(hwnd, 0);
			break;

		case IDOK:
		{
			CONST INT SIZE = 256;
			CHAR sz_info[SIZE] = "Info:\n����� ����� ���������� � ����";
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
				//���
				//DWORD address = 0;
				//SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&address);
				//short int msb = address >> 24; //MSB - most significant byte
				//DWORD mask = msb > 0 and msb <= 127 ? 0xFF000000 : msb >= 128 and msb <= 191 ? 0xFFFF0000 : msb >= 192 and msb <= 223 ? 0XFFFFFF00 : 0; 
				//SendMessage(hIPmask, IPM_SETADDRESS, 0, mask);  
#endif // HAPPY_NEW_YEAR


				//�����
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
//	//	//�������� �������
//	//	//������������� ��� � �����
//	//	//�������� ��������� �� ����� � �����
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
//	//	//���� ������������ 16-������ dword, � ���������� ����� �� �����������.
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
	// ��������� ����� ������� � �������� ���������
	return (0xFFFFFFFF << (32 - prefix));
}
