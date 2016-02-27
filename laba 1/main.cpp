#include "process.h"
#include "libraries.h"
//#include "process.cpp"
#define ARG 3

const char *input_number_short = "-n";
const char *input_number_long = "--number";

int main(int argc, char *argv[]) {
	
	//work whith consol
	if (argc != 1) {
		if (argc == ARG) {
			if (strcmp(argv[1], input_number_short) == 0
				|| strcmp(argv[1], input_number_long) == 0)
			{
				process number_1(argv[2]);
				if (number_1.control() == 0) {
					number_1.start_process();
					number_1.show_number();
				}
				else {
					cout << "Enter error!" << endl;
					return 1;
				}
			}
			else {
				cout << "Enter error!" << endl;
				return 1;
			}
		}
		else {
			cout << "Enter error!" << endl;
			return 1;
		}
		return 0;
	}

	//work whith menu
	string str_temp;
	cout << "Enter number (example: 123,4): ";
	cin >> str_temp;
	cin.clear();
	cin.sync();
	process number_2(str_temp);
	if (number_2.control() == 0) {
		number_2.start_process();
		number_2.show_number();
	}
	else {
		cout << "Enter error!" << endl;
		return 1;
	}

	return 0;
}
