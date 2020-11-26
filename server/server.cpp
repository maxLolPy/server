#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <stdio.h>
#include "server.h"
DWORD WINAPI workWithClient(LPVOID param) {
	struct res counts;

	counts.words = 0;
	counts.lines = 0;
	int real_socket_file = *(int*)param;
	enum command cmd;
	int res = 0, res2 = 0;
	int i = 0;
	char c;

	do {
		res = recv(real_socket_file, (char*)&cmd, sizeof(cmd), 0);
		if (res == SOCKET_ERROR) {
			printf("recv #1 error: %d\n", WSAGetLastError());
		}
		else if (res != 0) {
			if (res != sizeof(cmd))
			{
				printf("unexpected size of recv #1");
			}
			else {
				switch (cmd) {
				case sendMessage:

					do {
						int res2 = recv(real_socket_file, &c, sizeof(char), 0);
						if (res2 == SOCKET_ERROR) {
							printf("err rcv#2 %d\n", WSAGetLastError());
							res = -1;
						}
						else if (res2 == 0) {
							printf("Unexpected close sock\n");
							res = -1;
						}
						else if (res2 != sizeof(char)) {
							printf("Unexpected size in recv #2");
							res = -1;
						}
						else {
							if (c == ' ') counts.words++; else if (c == '\n') counts.lines++;//считаем количество слов и строк
						}
					} while ((res > 0) && (i < MAXLENMESS + 1) && (c != 0));
					if (res > 0) {
						if (c != 0) {
							printf("mem overflow attac!\n");
							res = -1;
						}
						else {//получил строку от клиента
							//но она мне не нужна
						}
					}
					break;
				case getCount:
					res2 = send(real_socket_file, (char*)&counts, sizeof(counts), 0);
					if (res2 == SOCKET_ERROR) {
						printf("err send #1 %d\n", WSAGetLastError());
						res = -1;
					}
					else if (res2 != sizeof(counts)) {
						printf("Unexpected size in send #1");
						res = -1;
					}
					break;
				default:
					printf("unexpected command\n");
					res = -1;
				}
			}
		}
	} while (res > 0);

	return 0;
}

int main()
{
	WSADATA wsadata = { 0 };
	int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (res != 0) {
		printf("Err of init lib\n");
	}
	else {
		int socket_file = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_file == INVALID_SOCKET) {
			printf("bind err %d\n", WSAGetLastError());
		}
		else {

			struct sockaddr_in address;
			address.sin_family = AF_INET;
			address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			address.sin_port = htons(7077);

			res = bind(socket_file, (const sockaddr*)&address, sizeof(address));
			if (res == SOCKET_ERROR) {
				printf("bind err %d\n", WSAGetLastError());
			}
			else {
				int res3 = listen(socket_file, BACKLOG);
				if (res3 == SOCKET_ERROR) {
					printf("listen err %d", WSAGetLastError());
				}
				else {
					//recv
					//send
					res = 0;
					do {
						int* real_socket_file = (int*)malloc(sizeof(int));
						if (real_socket_file == 0) {
							printf("err of malloc 129");
							res = -1;
						}
						else {
							*real_socket_file = accept(socket_file, NULL, NULL);
							if (*real_socket_file == INVALID_SOCKET) {
								printf("err accept %d\n", WSAGetLastError());
								res = -1;
							}
							else {
								//создаём тред
								if (CreateThread(NULL, 0, workWithClient, (LPVOID)real_socket_file, 0, NULL) == 0) {
									printf("err of creating thread");
									res = -1;
								}

							}
						}
					} while (res!=-1);
				}
			}


			res = closesocket(socket_file);
			if (res == SOCKET_ERROR) {
				printf("err close socket %d\n", WSAGetLastError());
			}
		}
		WSACleanup();
	}
}