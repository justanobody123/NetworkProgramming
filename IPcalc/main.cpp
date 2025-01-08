#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "resource.h"
#include <CommCtrl.h>
#include <iostream>
using std::cout;
using std::endl;
BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
		HWND hUpDown = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(30, 1));
	}
		break;
	case WM_COMMAND:
		//std::cout << LOWORD(wParam) << "\n";
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
		{
			//cout << HIWORD(wParam) << endl;
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: 
			{
				HWND hIpControl = GetDlgItem(hwnd, IDC_IPADDRESS);
				HWND hMaskControl = GetDlgItem(hwnd, IDC_IPMASK);
				DWORD address = 0;
				SendMessage(hIpControl, IPM_GETADDRESS, 0, (LPARAM)&address);
				short int msb = address >> 24; //MSB - most significant byte
				DWORD mask = msb > 0 and msb <= 127 ? 0xFF000000 : msb >= 128 and msb <= 191 ? 0xFFFF0000 : msb >= 192 and msb <= 223 ? 0XFFFFFF00 : 0; 
				SendMessage(hMaskControl, IPM_SETADDRESS, 0, mask);
			}
				break;
			default:
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
				cout << "CHANGE OF MASK\n";
				HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
				HWND hMaskControl = GetDlgItem(hwnd, IDC_IPMASK);
				DWORD mask = 0;
				SendMessage(hMaskControl, IPM_GETADDRESS, 0, (LPARAM)&mask);
				short int prefix = 0;
				char buffer[3];
				
				for (int i = 31; i >= 0; i--)
				{
					if ((mask >> i) & 1)
					{
						prefix++;
					}
					else
					{
						break;
					}
				}
				_itoa(prefix, buffer, 10);
				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)buffer);
			}
				break;
			default:
				break;
			}
		}
			break;
		case IDC_EDIT_PREFIX:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
			{
				cout << "CHANGE OF PREFIX\n";
				//вытащить префикс
				//преобразовать его в дворд
				//запихать указатель на дворд в маску
				HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
				HWND hMaskControl = GetDlgItem(hwnd, IDC_IPMASK);
				int prefix = 0; 
				const int MAX_SIZE = 3;
				char buffer[MAX_SIZE];
				SendMessage(hEditPrefix, WM_GETTEXT, MAX_SIZE, (LPARAM)buffer);
				prefix = atoi(buffer);
				cout << prefix << "\n"; 
				DWORD maskAddress = 0;
				//Ќадо сформировать 16-ричный dword, в дес€тичной адрес не принимаетс€.
				SendMessage(hMaskControl, IPM_SETADDRESS, 0, maskAddress);
			}
				break;
			default:
				break;
			}
		}
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}
