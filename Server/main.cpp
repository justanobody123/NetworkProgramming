#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
using std::cin;
using std::cout;
using std::endl;

#define DEFAULT_PORT "27015"
#define BUFFER_SIZE 1500
void main()
{
	setlocale(LC_ALL, "");
	//1. Initialize WinSock:
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); //(2, 2) - Version
	if (iResult != 0)
	{
		cout << "WinSock initializations failed with error #" << iResult << endl;
		return;
	}
	//2. Create Socket:
	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo hInst;
	//2.1 Получаем адрес текущего узла:
	ZeroMemory(&hInst, sizeof(hInst));
	hInst.ai_family = AF_INET;
	hInst.ai_socktype = SOCK_STREAM;
	hInst.ai_protocol = IPPROTO_TCP;
	hInst.ai_flags = AI_PASSIVE;

	iResult = GetAddrInfo(NULL, DEFAULT_PORT, &hInst, &result);
	if (iResult != 0)
	{
		cout << "GetAddrInfo failed with error #" << iResult << endl;
		WSACleanup();
		return;
	}
	//2.2 Создаем сокет:
	SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Socket error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	//3. Binding - привязываем сокет к порту:
	iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Bind failed with error #" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	freeaddrinfo(result); //После вызова функции bind информация об адресе больше не нужна
	//4. Начинаем прослушивать сокет:
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Listen failed with error #" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	cout << "Server is started on TCP port " << DEFAULT_PORT << endl;
	//5. Accept connection:
	do
	{
		int namelen = 32;
		CHAR sz_client_name[32];
		SOCKADDR client_socket;
		ZeroMemory(&client_socket, sizeof(client_socket));
		SOCKET ClientSocket = accept(ListenSocket, &client_socket, &namelen);
		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "Accept failed with error #" << WSAGetLastError() << endl;
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		
		//getsockname(ClientSocket, &client_socket, &namelen);
		sprintf
		(
			sz_client_name,
			"%i.%i.%i.%i:%i",
			(unsigned char)client_socket.sa_data[2],
			(unsigned char)client_socket.sa_data[3],
			(unsigned char)client_socket.sa_data[4],
			(unsigned char)client_socket.sa_data[5],
			(unsigned char)(client_socket.sa_data[0]) << 8 | (unsigned char)(client_socket.sa_data[1])
		);
		cout << sz_client_name << endl;
		//closesocket(ClientSocket);
		//closesocket(ListenSocket);
		//6. Receive & Send data;
		char recvbuffer[BUFFER_SIZE]{};
		int received = 0;
		do
		{
			received = recv(ClientSocket, recvbuffer, BUFFER_SIZE, 0);
			if (received > 0)
			{
				cout << "Bytes received: " << received << endl;
				cout << recvbuffer << endl;
				int iSendResult = send(ClientSocket, "Привет, клиент!", received, 0);
				if (iSendResult == SOCKET_ERROR)
				{
					cout << "Send failed with error #" << WSAGetLastError() << endl;
					closesocket(ClientSocket);
					WSACleanup();
					return;
				}
				cout << "Bytes sent: " << iSendResult << endl;
			}
			else if (received == 0)
			{
				cout << "Connection closing..." << endl;
			}
			else
			{
				cout << "Receive failed with error #" << WSAGetLastError() << endl;
				closesocket(ClientSocket);
				//WSACleanup();
				//return;
			}
		} while (received > 0);

		//7. Disconnection:
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			cout << "shutdown failed with error #" << WSAGetLastError() << endl;

		}
		closesocket(ClientSocket);
	} while (true);
	WSACleanup();
	system("PAUSE");
}