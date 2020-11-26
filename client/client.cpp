#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <stdio.h>
#include"../server/server.h"

int main() {
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
			address.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
			address.sin_port = htons(7077);

			res = connect(socket_file, (struct sockaddr *)&address, sizeof(address));
			if (res == SOCKET_ERROR) {
				printf("coonect error %d\n", WSAGetLastError());
			}
			else {
				do {
					printf("1 - send text\n2 - get resut\n");
					char c = getchar(); // проверь
					getchar();
					enum command cmd;
					switch (c) {
					case '1':
						cmd = sendMessage;
						char text[MAXLENMESS + 1];
						if (fgets(text, MAXLENMESS + 1, stdin) != NULL) {
							res = send(socket_file, (char*)&cmd, sizeof(cmd), 0);
							if (res == SOCKET_ERROR) {
								printf("send #1 error %d\n", WSAGetLastError());
							}
							else if (res < sizeof(cmd)) {
								printf("Unexpected size of send #1");
							}
							else {
								res = send(socket_file, (char*)&text, (strlen(text) + 1) * sizeof(char), 0);
								if (res == SOCKET_ERROR) {
									printf("send #2 error %d\n", WSAGetLastError());
								}
								else if (res < (strlen(text) + 1) * sizeof(char)) {
									printf("Unexpected size of send #2");
								}
							}
						}
						else {
							printf("err of fgets");
						}
						break;
					case '2':
						cmd = getCount;
						res = send(socket_file, (char*)&cmd, sizeof(cmd), 0);
						if (res == SOCKET_ERROR) {
							printf("send #1 error %d\n", WSAGetLastError());
						}
						else if (res < sizeof(cmd)) {
							printf("Unexpected size of send #1");
						}
						else {//получаем значения из сокета
							printf("1234\n"); 
							struct res otvet;
							int res2 = recv(socket_file, (char*)&otvet, sizeof(otvet), 0);
							if (res2 == SOCKET_ERROR) {
								printf("err rcv#2 %d\n", WSAGetLastError());
								res = -1;
							}
							else if (res2 == 0) {
								printf("Unexpected close sock\n");
								res = -1;
							}
							else if (res2 != sizeof(otvet)) {
								printf("Unexpected size in recv #2");
								res = -1;
							}
							else {
								printf("count words %d\ncount lines %d", otvet.words, otvet.lines);
							}

						}
						break;
					}
				} while (res > 0);
			}


			res = closesocket(socket_file);
			if (res == SOCKET_ERROR) {
				printf("err close socket %d\n", WSAGetLastError());
			}
		}
		WSACleanup();
	}
}