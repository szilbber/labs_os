#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
using namespace std;
/*
//Структура интеграла
//Содержит систему счисления и число в этой системе
*/
struct Data {
	int nSystem;
	int firstNumber;
};
//Неименованные каналы связи in и out
int pipe_in[2];
int pipe_out[2];
pid_t pid;
void help() {
	cout << "Справка:\n"
		<< "Данная программа позволяет переводить числа в разные системы счисления\n"
		"Для перевода числа из любой системы счисления в десятичную используйте: \n" 
		"	для ввода и вывода в консоль --c\n"
		"	для ввода и вывода из файла --f\n"
		"Для перевода числа из десятичной системы счисления в любую другую используйте: \n"
		"	для ввода и вывода в консоль --tenc\n"
		"	для ввода и вывода из файла --tenf\n"
		<< endl;
}

/*
//Функция перевода из одной сс в другую
//Принимает систему счисления и число в этой системе
//Возвращает десятичное число
*/
int calculation(int nSystem, int firstNumber)
{
	int decNum = 0;
	int pos = 0;
	while (firstNumber) {
		decNum += (firstNumber % 10) * pow(nSystem, pos);
		firstNumber = firstNumber / 10;
		pos++;
	}
	return decNum;
}
//Из десятичной в другую
//Принимает систему счисления и число в десятичной системе
//Возвращает число в этой системе
int calculation_ten(int nSystem, int firstNumber) {

	int result = 0, multiplier = 1;
	while (firstNumber > 0) {
		result += (firstNumber % nSystem) * multiplier; // добавляем к результату остаток от деления на базу, умноженный на множитель
		firstNumber /= nSystem; // делим число на базу
		multiplier *= 10; // увеличиваем множитель на 10
	}
	return result;
}
//Функция ввода данных и проверка на корректность ввода
int read_data(const string& msg) {
	int result;
	bool flag = true;
	while (flag)
	{
		cout << msg;
		cin >> result;
		if ((result <= 0) || (cin.fail() || (cin.peek() != '\n'))) {
			cin.clear();
			cin.ignore(1000, '\n');
			cout << " > Ошибка ввода, попробуйте еще раз\n";
		}
		else {
			flag = false;
		}
	}
	return result;

}

/*
//Клиентская часть программы
//Принимает данные: сс и число в этой системе
//Записывает и читает в/из неименованных каналов
//Выводит результат вычислений
*/
void frontend(){
	Data data;
	data.nSystem = read_data("Введите из какой системы счисления вы хотите перевести: ");
	data.firstNumber = read_data("Введите число в выбранной системе счисления: ");
	write(pipe_in[1], &data, sizeof(Data));
	int result;
	read(pipe_out[0], &result, sizeof(int));
	cout << "Результат: " << result << endl << "\n";
	exit(0);
}
void frontend_ten() {
	Data data;
	data.nSystem = read_data("Введите в какую систему счисления вы хотите перевести: ");
	data.firstNumber = read_data("Введите число в десятичной системе счисления: ");
	write(pipe_in[1], &data, sizeof(Data));
	int result;
	read(pipe_out[0], &result, sizeof(int));
	cout << "Результат: " << result << endl << "\n";
	exit(0);
}
void frontend_file(){
	ofstream out;
	ifstream in("input.txt");
	out.open("output.txt");
	out << "Перевод из системы счисления в десятичную\n";
	Data data;
	if(in.is_open()){
		in >> data.nSystem >> data.firstNumber;
	}
	write(pipe_in[1], &data, sizeof(Data));
	int result;
	read(pipe_out[0], &result, sizeof(int));
	out << "Результат: " << result << endl << "\n";
	cout << "> SUCCESS!\n";
	in.close();
	out.close();
	exit(0);
}
void frontend_file_ten() {
	ofstream out;
	ifstream in("input.txt");
	out.open("output.txt");
	out << "Перевод из десятичной системы счисления в другую\n";
	Data data;
	if (in.is_open()) {
		in >> data.nSystem >> data.firstNumber;
	}
	write(pipe_in[1], &data, sizeof(Data));
	int result;
	read(pipe_out[0], &result, sizeof(int));
	out << "Результат: " << result << endl << "\n";
	cout << "> SUCCESS!\n";
	in.close();
	out.close();
	exit(0);
}
/*
//Серверная часть программы
//Производит вычисления по данным, переданным клиентской частью по неименованному каналу
//Записывает результат вычисления в неименованный канал
*/
void backend(){
	Data data; 
	read(pipe_in[0], &data, sizeof(Data));
	int result = calculation(data.nSystem, data.firstNumber);
	write(pipe_out[1], &result, sizeof(int));
}
void backend_ten() {
	Data data;
	read(pipe_in[0], &data, sizeof(Data));
	int result = calculation_ten(data.nSystem, data.firstNumber);
	write(pipe_out[1], &result, sizeof(int));
}
int main(int argc, char *argv[]) {
	setlocale(LC_ALL, 0);
if (argc == 2 && string(argv[1]) == "--help")
{
	help();
	exit(0);
}
else if (argc == 1)
{
	cout << "Запустите программу с ключом --help для получения справки\n"<< endl;
	exit(1);
}
else if (argc == 2 && string(argv[1]) == "--c")
{
	cout << "Перевод из системы счисления в десятичную и обратно\n";
	pipe(pipe_in);
	pipe(pipe_out);
	pid = fork();
	if (pid < 0)
	{
		cerr << "Критическая ошибка! Форк не создан" << endl;
		exit(1);
	}
	else if (pid > 0)//main procces
	{
		frontend();
	}
	else//child procces
	{
		backend();
	}
	for (int i = 0; i < 2; ++i)
	{
		close(pipe_in[i]);
		close(pipe_out[i]);
	}
}
else if (argc == 2 && string(argv[1]) == "--f")
{
	
	pipe(pipe_in);
	pipe(pipe_out);
	pid = fork();
	if (pid < 0)
	{
		cerr << "Критическая ошибка! Форк не создан" << endl;
		exit(1);
	}
	else if (pid > 0)//main procces
	{
		frontend_file();
	}
	else//child procces
	{
		backend();
	}
	for (int i = 0; i < 2; ++i)
	{
		close(pipe_in[i]);
		close(pipe_out[i]);
	}
	
	
}
else if (argc == 2 && string(argv[1]) == "--tenc"){

	pipe(pipe_in);
	pipe(pipe_out);
	pid = fork();
	if (pid < 0)
	{
		cerr << "Критическая ошибка! Форк не создан" << endl;
		exit(1);
	}
	else if (pid > 0)//main procces
	{
		frontend_ten();
	}
	else//child procces
	{
		backend_ten();
	}
	for (int i = 0; i < 2; ++i)
	{
		close(pipe_in[i]);
		close(pipe_out[i]);
	}
}
else if (argc == 2 && string(argv[1]) == "--tenf") {

	pipe(pipe_in);
	pipe(pipe_out);
	pid = fork();
	if (pid < 0)
	{
		cerr << "Критическая ошибка! Форк не создан" << endl;
		exit(1);
	}
	else if (pid > 0)//main procces
	{
		frontend_file_ten();
	}
	else//child procces
	{
		backend_ten();
	}
	for (int i = 0; i < 2; ++i)
	{
		close(pipe_in[i]);
		close(pipe_out[i]);
	}
}
else{
cout << "> ERROR! SOMETHING WRONG\n" << endl; 
}
return 0;
}


