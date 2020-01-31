#pragma once

#include <Windows.h>
#define PRESS 1
#define RELEASE 2

class Keyboard {
private:
	Keyboard();

	static bool keys[];
	static bool keysDown[];
	static bool keysUp[];

public:
	static void KeyCallback(const int key, const int action);

	static bool KeyDown(const int key);
	static bool KeyUp(const int key);
	static bool KeyPressed(const int key);

};

