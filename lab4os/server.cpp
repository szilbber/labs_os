#include <windows.h>
#include <iostream>
#include <thread>
#include <stack>
#include <vector>
#define CONNECTING_STATE 0
#define READING_STATE 1
#define WRITING_STATE 2
#define INSTANCES 1
#define PIPE_TIMEOUT 5000
#define BUFSIZE 512
using namespace std;

int order(char op) {
	if (op == '+' || op == '-') return 1;
	if (op == '*' || op == '/') return 2;
	return 0;
}

double applyOp(double a, double b, char op) {
	switch (op) {
	case '+': return a + b;
	case '-': return a - b;
	case '*': return a * b;
	case '/': return a / b;
	}
	return 0;
}

double evaluateExpression(string expression) {
	stack<double> values;
	stack<char> ops;

	for (size_t i = 0; i < expression.length(); i++) {

		if (expression[i] == ' ')
			continue;
		if (isdigit(expression[i]) || ((expression[i] == '-' || expression[i] == '+') && (i == 0 || !isdigit(expression[i - 1])))) {
			double num = 0;
			int sign = 1;
			if (expression[i] == '+') i++;
			else if (expression[i] == '-') {
				sign = -1;
				i++;
			}
			while (i < expression.length() && isdigit(expression[i]))
				num = num * 10 + (expression[i++] - '0');
			i--;
			values.push(sign * num);

		}
		else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
			while (!ops.empty() && order(ops.top()) >= order(expression[i])) {

				double b = values.top(); values.pop();
				double a = values.top(); values.pop();
				char op = ops.top(); ops.pop();
				values.push(applyOp(a, b, op));
			}
			ops.push(expression[i]);
		}
	}

	while (!ops.empty()) {

		double b = values.top(); values.pop();
		double a = values.top(); values.pop();
		char op = ops.top(); ops.pop();
		values.push(applyOp(a, b, op));
	}

	return values.top();
}
string str_global = "";
int main(void)
{
	HANDLE Pipe;
	const wchar_t* nameofpipe = L"\\\\.\\pipe\\mynamedpipe";
	//Инициализируем сам канал
	Pipe = CreateNamedPipe(
		nameofpipe, //Имя канала
		PIPE_ACCESS_DUPLEX | //Чтение и запись в канал
		FILE_FLAG_OVERLAPPED, //Включен перекрывающийся ввод/вывод
		PIPE_TYPE_MESSAGE | // Канал передает сообщения, а не поток байт
		PIPE_READMODE_MESSAGE | // Канал считывает сообщения
		PIPE_WAIT, // Клиент будет ожидать, когда канал станет доступен
		INSTANCES, // Максимальное количество экземпляров канала
		BUFSIZE * 4, //Выходной размер буфера
		BUFSIZE * 4, //Входной размер буфера
		PIPE_TIMEOUT, // Время ожидания клиента
		NULL); //Атрибут защиты
		
	//Проверка открылся ли канал
	if (Pipe == INVALID_HANDLE_VALUE) {
		cout << "[ERROR] Pipe is not open " << '\n';
		return 0;
	}
	//Подключаемся к клиенту
	BOOL fConnected = ConnectNamedPipe(Pipe, nullptr);
	if (fConnected == false) {
		cout << "[ERROR] Connect Pipe failed " << GetLastError() << '\n';
		return 0;
	}
	while(1){
		bool fSuccess;
		char buffer[512];
		DWORD cbBytes;
		//Читаем данные из канала
		fSuccess = ReadFile(
			Pipe,          // Канал для чтения
			buffer,        // Указатель на буфер, в который будут записаны прочитанные данные
			sizeof(buffer),// Размер буфера в байтах
			&cbBytes,      // Указатель на переменную, куда будет записано количество реально прочитанных байт
			nullptr        // Указатель на структуру overlapped (асинхронный ввод-вывод)
		);

		//При успешном завершении чтения
		if (fSuccess) {
			string word(buffer);
			if (word == "exit") {
				cout << "Program stopped! GoodBye<3 GoodLuck\n";
				string str = "Program stopped! GoodBye<3 GoodLuck";
				WriteFile(
					Pipe, //Канал для записи
					str.c_str(), //Откуда будут записаны данные
					sizeof(str), //Размер строки в байтах
					&cbBytes,// Указатель на переменную, куда будет записано количество реально прочитанных байт
					nullptr        // Указатель на структуру overlapped (асинхронный ввод-вывод)
				);
				return 0;
			}
			else if (word == "calc") {
				if (str_global[0] == '*' || str_global[0] == '/') {
					string str = "";
					cout << "Error: Incorrect element!";
					str = "Error: Incorrect element!";
					WriteFile(
						Pipe,
						str.c_str(),
						sizeof(str),
						&cbBytes,
						nullptr);
					return 0;
				}
				double result = evaluateExpression(str_global);
				WriteFile(
					Pipe,
					to_string(result).c_str(),
					sizeof(to_string(result).c_str()) + 1,
					&cbBytes,
					nullptr);

			}
			else if (word == "clear") {
				str_global = "";
				string str = "Pipe is clear. Enter new string >>>> \n";
				WriteFile(
					Pipe,
					str.c_str(),
					sizeof(str),
					&cbBytes,
					nullptr);
			}
			else if (word == "check") {
				string str;
				size_t found = str_global.find("/0");
				if (found != std::string::npos || str_global[0] == '*' || str_global[0] == '/') {
					cout << "Error: Incorrect element!";
					str = "Error: Incorrect element!";
					WriteFile(
						Pipe,
						str.c_str(),
						sizeof(str),
						&cbBytes,
						nullptr);
					return 0;
				}
				else {
					str = "Formula is right!";
					WriteFile(
						Pipe,
						str.c_str(),
						sizeof(str),
						&cbBytes,
						nullptr);
				}

			}
			else {
				str_global += word;
				WriteFile(
					Pipe,
					str_global.c_str(),
					sizeof(str_global),
					&cbBytes,
					nullptr);
			}
		}
		//Если чтение не удалось
		else {
			cout << "Reading error \n";
			string str = "Reading error \n";
			WriteFile(
				Pipe,
				str.c_str(),
				sizeof(str),
				&cbBytes,
				nullptr);
			return 0;

		}
	}
	
 return 0;
}
