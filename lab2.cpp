#include <iostream>
#include <fstream>
#include <cstring> 
//Запрос информации о файле
#include <sys/stat.h> 

using namespace std;

//Функция копирования файла
void copying(const char *file1, const char *file2)
{
    if (file1 == file2) {
        cout << " > copying error" << endl;
        return;
    }
    ifstream fin;
    size_t bufsize = 4;
    char* buf = new char[bufsize];
    fin.open(file1, ios::binary);
    if (fin.is_open()) {
        ofstream fout;
        fout.open(file2, ios::binary);
        while (!fin.eof()) {
            fin.read(buf, bufsize);
            if (fin.gcount()) fout.write(buf, fin.gcount());
        }
        cout << " > copying success" << endl;
        delete[] buf;
        fin.close();
        fout.close();
    }
    else {
        return;
    }
}

//Перемещение 
void moving(const char *file1, const char *file2)
{
    int result = rename(file1, file2);
    if (result != 0)
        cerr << " > moving error" << endl; 
    else
        cout << " > moving success" << endl;
}

// функция для преобразования битовой маски в буквы
void show_access_right(mode_t mode)
{
    cout << "> access: ";

    // проверяем тип файла (каталог или обычный)
    if (S_ISDIR(mode))
        cout << "d";
    else
        cout << "-";

    // проверяем владельца
    cout << ((mode & S_IRUSR) ? "r" : "-");
    cout << ((mode & S_IWUSR) ? "w" : "-");
    cout << ((mode & S_IXUSR) ? "x" : "-");

    // проверяем группу
    cout << ((mode & S_IRGRP) ? "r" : "-");
    cout << ((mode & S_IWGRP) ? "w" : "-");
    cout << ((mode & S_IXGRP) ? "x" : "-");

    // проверяем остальных
    cout << ((mode & S_IROTH) ? "r" : "-");
    cout << ((mode & S_IWOTH) ? "w" : "-");
    cout << ((mode & S_IXOTH) ? "x" : "-");

    cout << endl;
}

//Информация о файле
void file_info(const char *filename)
{
    //Создаем структуру, в которой хранится инфа
    struct stat fileInfo;                
    //Функция вносит в структуру информацию, содержащуюся в файле
    int result = stat(filename, &fileInfo); 
    if (result != 0)
        cerr << " > get info error" << endl;
    else
    {
        cout << "size of file: " << fileInfo.st_size << endl;
        //Битовое представление
        mode_t access = fileInfo.st_mode; 
        //Преобразуем его в буквы и выводим
        show_access_right(access);       

        //Выводим время
        cout << "time of changing: " << ctime(&fileInfo.st_mtime); 
    }
}

//Изменение прав доступа 
void access_right(const char *filename, mode_t new_access)
{
    //Функция изменения прав доступа 
    int result = chmod(filename, new_access); 
    if (result != 0)
        cerr << " > error of changing access rights" << endl; 
    else
        cout << " > success of changing access rights" << endl;
}

int main(int size_of_arguments, char *arguments[])
{
    //Командная строка
    if (size_of_arguments > 1)
    {
        //Указываем что хотим сделать
        string action = arguments[1]; 

        if (action == "helping")
        {
            cout << "What you can do:" << endl;
            cout << "- copying file1 file2" << endl;
            cout << "- moving file1 file2" << endl;
            cout << "- info filename" << endl;
            cout << "- mode filename new_access_rights(0 - no rights, 1 - x, 2 - w, 4 - r)" << endl;
            return 0;
        }

        else if (action == "copying" && size_of_arguments == 4)
            copying(arguments[2], arguments[3]);

        else if (action == "moving" && size_of_arguments == 4)
            moving(arguments[2], arguments[3]);

        else if (action == "info" && size_of_arguments == 3)
            file_info(arguments[2]);

        else if (action == "mode" && size_of_arguments == 4)
        {
            //Преобразуем строку в восьмеричное число
            mode_t newPermissions = strtol(arguments[3], NULL, 8); 
            access_right(arguments[2], newPermissions);
        }

        else{
            cerr << " > error" << endl;
            return 1;
        }
    }

    //Консоль
    else
    {
        cout << "What you can do:" << endl;
            cout << "- copying file1 file2" << endl;
            cout << "- moving file1 file2" << endl;
            cout << "- info filename" << endl;
            cout << "- mode filename new_access_rights" << endl;
            cout << "- exit" << endl;

        string action; 

        while(true)
        {
            cout << " > Enter the action:";
            cin >> action;

            if (action == "copying")
            {
                string file1;
                string file2;
                cout << "first file: ";
                cin >> file1;
                cout << "second file: ";
                cin >> file2;
                copying(file1.c_str(), file2.c_str());
                continue;
            }

            else if (action == "moving")
            {
                string file1;
                string file2;
                cout << "first file: ";
                cin >> file1;
                cout << "second file: ";
                cin >> file2;
                moving(file1.c_str(), file2.c_str());
                continue;
            }

            else if (action == "info")
            {
                string filename;
                cout << "filename: ";
                cin >> filename;
                file_info(filename.c_str());
                continue;
            }

            else if (action == "mode")
            {
                string filename;
                mode_t newAccess;
                cout << "filename: ";
                cin >> filename;
                cout << "new access right (0 - no rights, 1 - x, 2 - w, 4 - r): ";
                cin >> oct >> newAccess;
                access_right(filename.c_str(), newAccess);
                continue;
            }

            else if (action == "exit")
            {
                return 0;
            }

            else
            {
                cerr << "error" << endl;
                continue;
            }
        }
    }

    return 0;
}
