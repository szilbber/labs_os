#include<iostream>
#include<limits>
#include<string>
#include<cmath>
using namespace std;
void Program() {
	cout << "Hello!\n" << "This program converts a number from septenary to decimal." << endl;
}


char valid_continue() {
	cout << "Do you want to continue? (y/n) >> ";
 	char answer;
 	cin >> answer;
 	// С помощью библиотеки locale и функции tolower понижаем регистр
 	answer = tolower(answer);
 	while (answer != 'y' && answer != 'n') {
 		cout << "I don't understand you, sorry. Please, try again.\n";
 		cout << "Do you want to continue? (y/n) >> ";
 		cin >> answer;
    }
    return answer;
}

bool isSeptenary(int a){
    bool flag = true;
    while(a){
        if (a%10<7) a/=10;
        else{
            flag = false;
            break;
        }
    }
    return flag;
}

int main(){
	char answer;
	Program();
	do{
		int sevNum = 0;
		int decNum = 0;
		cout<<"Please, enter a non-negative number>>";
		cin>>sevNum;
		
		//Проверка на отрицательное и семеричное число
		while(!isSeptenary(sevNum) ||sevNum<0|| sevNum == 7||sevNum==8||sevNum==9){
			cout << "I don't understand you, sorry. Please, try again.\n";
			cout<<"Enter a non-negative number>>";
			cin>>sevNum;
		}
		
		int pos = 0;
		while(sevNum){
		    decNum+=(sevNum%10)*pow(7,pos);
		    sevNum = sevNum/10;
		    pos++;
		}
		cout<<"Your number in dec notation>>" << decNum << endl;
		answer = valid_continue();	
	}while(answer == 'y');
	cout<<"Thanks for using this program!\n"<<"Goodbye!";
	return 0;
}


