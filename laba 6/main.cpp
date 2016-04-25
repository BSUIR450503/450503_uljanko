#include <iostream>
#include <fstream>
#include <windows.h>
#include "memoryManager.h"

using namespace std;

static int average = 0;

__declspec(selectany) MManagerHeap managerHeap;
__forceinline void * operator new(size_t n) {
	return managerHeap.Alloc(n);
}
__forceinline void operator delete(void* p) {
	return managerHeap.Free(p);
}
__forceinline void * operator new[](size_t n) {
	return managerHeap.Alloc(n);
}
__forceinline void operator delete[](void* p) { 
	return managerHeap.Free(p);
}

class TestClassOne {
public:
	char *array;

	TestClassOne() {
		array = new char[20];
	}

	~TestClassOne() {
		delete[] array;
	}
};

class TestClassTwo {
public:
	char *array;
	TestClassOne *obj;

	TestClassTwo(int count) {
		obj = new TestClassOne;
		array = new char[count];
		if (average == 0) {
			average = count;
		}
		else {
			average = (average + count) / 2;
		}
	}

	~TestClassTwo() {
		delete obj;
		delete[] array;
	}
};

void main() {
	int maxObjSize = 10000;
	int maxListSize = 100;
	int count = 0;
	typedef TestClassTwo *TestObject;
	TestObject *coll = new TestObject[maxListSize];
	memset(coll, 0, sizeof(TestObject) *maxListSize);

	for (int i = 0; i < 20; i++) {
		for (int y = 0; y < 1000000; y++) {
			coll[count] = new TestClassTwo(maxObjSize);
			if (++count >= maxListSize) {
				for (int k = 0; k < maxListSize; k++) {
					delete coll[k];
				}
				memset(coll, 0, sizeof(TestObject) * maxListSize);
				count = 0;
			}
		}
		cout << "OK!\n";
	}

	for (int i = 0; i < maxListSize; i++) {
		delete coll[i];
	}
}




