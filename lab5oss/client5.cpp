
#include <string>
#pragma warning(disable: 4996)
using namespace std;
#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
	// Инициализация Winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0) {
		std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
		return 1;
	}

	// Создание сокета
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		std::cerr << "Can't create socket! Quitting" << std::endl;
		WSACleanup();
		return 1;
	}

	// Подключение к серверу
	//Адрес сокета
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;


	int connectRes = connect(sock, (sockaddr*)&addr, sizeof(addr));
	if (connectRes == SOCKET_ERROR) {
		std::cerr << "Can't connect to server! Quitting" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// Основной цикл отправки сообщений
	char buf[4096];
	string userInput;

	do {
		// Получение ввода пользователя
		cout << "Enter the expression: ";
		getline(cin, userInput);

		if (userInput.size() > 0) {
			// Отправка сообщения на сервер
			int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendRes != SOCKET_ERROR) {
				// Получение ответа от сервера
				//ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0) {
					std::cout << "Result is: " << std::string(buf, 0, bytesReceived) << std::endl;
				}
			}
		}

	} while (userInput.size() > 0);

	// Закрытие сокета
	closesocket(sock);
	WSACleanup();

	return 0;
}
