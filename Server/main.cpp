#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>


using std::cin;
using std::cout;
using std::endl;

#define DEFAULT_PORT "27015"
#define BUFFER_SIZE 1500
#define MAX_CONNECTIONS 3
#pragma comment(lib, "Ws2_32.lib")
union ClientSocketData
{
	SOCKADDR client_socket;
	
	unsigned long long data;
	ClientSocketData(SOCKADDR client_socket)
	{
		this->client_socket = client_socket;
	}
	unsigned long long get_data() const
	{
		return data;
	}
	unsigned short get_port() const
	{
		/*int i_port = (data >> 16) & 0xFFFF;
		return i_port;*/
		return (unsigned char)(client_socket.sa_data[0]) << 8 | (unsigned char)(client_socket.sa_data[1]);
	}
	char* get_socket(char* sz_client_name) const
	{

		sprintf
		(
			sz_client_name,
			"%i.%i.%i.%i:%i",
			(unsigned char)client_socket.sa_data[2],
			(unsigned char)client_socket.sa_data[3],
			(unsigned char)client_socket.sa_data[4],
			(unsigned char)client_socket.sa_data[5],
			get_port()
		);
		return sz_client_name;
	}
};
void HandleClient(LPVOID lParam);
void PrintNumberOfClients();
SOCKET ClientSocket;
SOCKET client_sockets[MAX_CONNECTIONS]{};
HANDLE client_handles[MAX_CONNECTIONS]{};
DWORD dw_thread_id[MAX_CONNECTIONS]{};
int* client_number[MAX_CONNECTIONS]{};
int number_of_clients = 0;

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
		PrintNumberOfClients();
		int namelen = 32;
		CHAR sz_client_name[32];
		SOCKADDR client_socket;
		ZeroMemory(&client_socket, sizeof(client_socket));
		if (number_of_clients < MAX_CONNECTIONS)
		{
			client_number[number_of_clients] = (int*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(int));
			*client_number[number_of_clients] = number_of_clients;
			client_sockets[number_of_clients] = accept(ListenSocket, &client_socket, &namelen);
			if (ClientSocket == INVALID_SOCKET)
			{
				cout << "Accept failed with error #" << WSAGetLastError() << endl;
				//closesocket(ListenSocket);
				//WSACleanup();
				//return;
			}
			//HandleClient(ClientSocket);
			client_handles[number_of_clients] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleClient, client_number[number_of_clients], 0, 0);
			number_of_clients++;
		}
		else
		{
			SOCKET extra_socket = accept(ListenSocket, &client_socket, &namelen);
			char recvextrabuffer[BUFFER_SIZE]{};
			recv(extra_socket, recvextrabuffer, BUFFER_SIZE, 0);
			char message[] = "No free connections left";
			send(extra_socket, message, sizeof(message), 0);
			shutdown(extra_socket, SD_BOTH);
			closesocket(extra_socket);
			cout << ClientSocketData(client_socket).get_socket(sz_client_name) << " was disconnected\n";
			
		}
		Sleep(100);
	} while (true);
	system("PAUSE");
	WSACleanup();
	
}

void HandleClient(LPVOID lParam)
{
	int i = *((int*)lParam);

	int namelen = 32;
	CHAR sz_client_name[32];
	SOCKADDR client_socket;

	ZeroMemory(&client_socket, sizeof(client_socket));

	getpeername(client_sockets[i], &client_socket, &namelen);
		/*sprintf
		(
			sz_client_name,
			"%i.%i.%i.%i:%i",
			(unsigned char)client_socket.sa_data[2],
			(unsigned char)client_socket.sa_data[3],
			(unsigned char)client_socket.sa_data[4],
			(unsigned char)client_socket.sa_data[5],
			(unsigned char)(client_socket.sa_data[0]) << 8 | (unsigned char)(client_socket.sa_data[1])
		);*/
		//ClientSocketData client_data(client_socket);
		// << "Client: " << client_data.get_socket(sz_client_name) << endl;
	cout << "Client: " << ClientSocketData(client_socket).get_socket(sz_client_name) << endl;
	//cout << client_data.get_data() << endl;
	//cout << "Port: " << client_data.get_port() << endl;
	//closesocket(ClientSocket);
	//closesocket(ListenSocket);
	//6. Receive & Send data;
	char recvbuffer[BUFFER_SIZE]{};
	int received = 0;
	do
	{
		ZeroMemory(recvbuffer, BUFFER_SIZE);
		received = recv(client_sockets[i], recvbuffer, BUFFER_SIZE, 0);
		if (received > 0)
		{
			cout << "Bytes received: " << received << endl;
			cout << "Message from " << sz_client_name << "\t" << recvbuffer << endl;
			int iSendResult = send(client_sockets[i], recvbuffer, received, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				cout << "Send failed with error #" << WSAGetLastError() << endl;
				closesocket(client_sockets[i]);
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
			closesocket(client_sockets[i]);
			//WSACleanup();
			//return;
		}
	} while (received > 0);

	//7. Disconnection:
	int iResult = shutdown(client_sockets[i], SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed with error #" << WSAGetLastError() << endl;

	}
	closesocket(client_sockets[i]);
}

void PrintNumberOfClients()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	ZeroMemory(&consoleInfo, sizeof(consoleInfo));
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	SetConsoleCursorPosition(hConsole, COORD{ 85, 0 });
	cout << "Количество клиентов: " << number_of_clients << endl;
	SetConsoleCursorPosition(hConsole, consoleInfo.dwCursorPosition);
}
