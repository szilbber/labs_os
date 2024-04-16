#include <windows.h>
#include <iostream>
#define BUFSIZE 512
using namespace std;

int main(int argc, char* argv[])
{
	char buffer[512];
	DWORD cbBytes;
	HANDLE Pipe;

	const wchar_t* nameofpipe = L"\\\\.\\pipe\\mynamedpipe";
	
	Pipe = CreateFile(
		nameofpipe, //Имя канала
		GENERIC_READ | //Чтение и запись из канала
		GENERIC_WRITE,
		0, //Уровень общего доступа
		NULL, //Атрибуты безопасности
		OPEN_EXISTING, //Открыть существующий
		0, //Атрибуты файла
		NULL); //Файл шаблона
	//Проверка открылся ли канал
	if (Pipe == INVALID_HANDLE_VALUE) return 0;

	while (1) {
		cout << "Enter the formula : ";
		string str_console;
		cin >> str_console;
		WriteFile(
			Pipe,
			str_console.c_str(),
			sizeof(str_console) + 1,
			&cbBytes,
			nullptr);

		ReadFile(
			Pipe,
			buffer,
			sizeof(buffer),
			&cbBytes,
			nullptr);
		string word(buffer);
		cout << word << endl;
		if (word == ("Program stopped! GoodBye<3 GoodLuck") || word == "Error: Incorrect element!") break;
	}
	return 0;
}