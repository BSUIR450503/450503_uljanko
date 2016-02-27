#pragma once
#include "libraries.h"

class process {
private:
	string input_number;
	string output_number;

public:
	process();
	process(string);
	~process();

	void show_number();
	bool control();
	void start_process();
};