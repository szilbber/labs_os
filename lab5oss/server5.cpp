
#include <iostream>
#include <string>
using namespace std;
#include <WS2tcpip.h>
#include <Windows.h>
#include <stack>
#pragma warning(disable: 4996)//отключения предупреждения компилятора с использованием устаревших функций
#pragma comment(lib, "ws2_32.lib")//явное указание библиотеки

HANDLE semaphore;

double calc(char oper, double num1, double num2) {
    if (oper == '+') return num1 + num2;
    else if (oper == '-') return num1 - num2;
    else if (oper == '*') return num1 * num2;
    else if (oper == '/') return num1 / num2;
}
double evaluateExpression(string expression) {
    stack<double> values;
    double res = 0;
    for (size_t i = 1; i < (expression).length(); i++) {

        if (expression[i] == ' ')
            continue;
        else if (isdigit(expression[i]) && (i == 0 || !isdigit(expression[i - 1]))) {
            double num = 0;
            while (i < (expression).length() && isdigit(expression[i]))
                num = num * 10 + (expression[i++] - '0');
            i--;
            values.push(num);

        }
    }
    double num2 = values.top(); values.pop();
    double num1 = values.top(); values.pop();
    res = calc(expression[0], num1, num2);
    return res;
}

void handleClient(SOCKET clientSocket) {
    char buffer[4096];
    int bytesReceived;

    // Принимаем данные от клиента
    while ((bytesReceived = recv(clientSocket, 
        buffer, 
        sizeof(buffer), 
        0)//флаги операции чтения
        ) > 0) {
        // Обработка данных
        buffer[bytesReceived] = '\0';
        double r = evaluateExpression(buffer);
        cout << "Received from the client " << clientSocket << " arithmetic operation" << buffer << endl;
        cout << "Result of client " << clientSocket << " is: " << r << endl;
        // отправляешь арифметическое выражение
        send(clientSocket, to_string(r).c_str(), bytesReceived + 1, 0);
    }

    if (bytesReceived == 0) {
        cout << "Client "<< clientSocket << " disconnected" << endl;
    } else {
        cout << "recv() failed " << endl;
    }

    // Закрываем сокет
    closesocket(clientSocket);

    // Освобождение семафора
    ReleaseSemaphore(semaphore,//указатель на семафор
        1,//на сколько менять счетчик
        NULL);//предыдущее значение
}

int main() {
    // Инициализация семафора
    semaphore = CreateSemaphore(NULL, //семафор получает дескриптор безопасности по умолчанию
                                2, //Начальное число для объекта семафора. Определяет, сколько задач имеют доступ к ресурсу вначале.
                                2,// максимальное количество обращений
                                NULL); //имя объекта семафора

    // Инициализация Winsock
    WSADATA wsData;
    //запрашиваемая версия библиотеки winsock
    WORD ver = MAKEWORD(2, 2);
    //проверка подключения библиотеки
    int wsOK = WSAStartup(ver, &wsData);
    if (wsOK != 0) {
        std::cerr << "Can't initialize Winsock!" << endl;
        return 1;
    }

    // Создание сокета
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "Can't create a socket!" << endl;
        return 1;
    }

    // Привязка IP и порта к сокету
    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");//Устанавливается IP-адрес, к которому будет привязан сокет
    addr.sin_port = htons(1111);//Устанавливается порт, к которому будет привязан сокет. 
    //Функция для преобразования порта в сетевой порядок байт.
    addr.sin_family = AF_INET; //Устанавливается семейство адресов.

    bind(listening, (sockaddr*)&addr, sizeof(addr));//привязка IP-адреса и порта к сокету

    // Прослушивание сокета
    listen(listening, SOMAXCONN);
    //SOMAXCON - максимальная длина очереди входящих подключений

    while (true) {
        // Ожидание подключения клиента
        sockaddr_in clientAddr; //Создается структура `sockaddr_in` для хранения информации о клиенте, который подключается к серверу
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(listening, (sockaddr*)&clientAddr, &clientSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Can't accept client connection!" << std::endl;
            continue;
        }

        // Ожидание свободного места для клиента
        WaitForSingleObject(semaphore, INFINITE);

        // Обработка клиента в отдельном потоке
        CreateThread(NULL,//дескриптор защиты потока, NULL указывает, что поток наследует уровень защиты родительского процесса
            0,//размер стека для нового потока
            (LPTHREAD_START_ROUTINE)handleClient,//указатель на функцию, которая будет выполнена в создаваемом потоке
            (LPVOID)clientSocket, // указатель на данные, передаваемые в функцию
            0,//флаги создания потока
            NULL);//выходной параметр, содержащий идентификатор созданного потока 
    }

    // Освобождение ресурсов
    CloseHandle(semaphore);
    WSACleanup();

    return 0;
}
