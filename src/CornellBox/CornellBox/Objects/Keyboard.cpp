#include "Keyboard.h"

bool Keyboard::keys[150] = { 0 };
bool Keyboard::keysDown[150] = { 0 };
bool Keyboard::keysUp[150] = { 0 };

Keyboard::Keyboard() {

}

void Keyboard::KeyCallback(const int key, const int action) {
	if (key < 0)
		return;

	if (key == VK_ESCAPE && action == PRESS) {
	}
		// close program

	if (action == PRESS && keys[key] == false) {
		keysDown[key] = true;
		keysUp[key] = false;
	}

	if (action == RELEASE && keys[key] == true) {
		keysDown[key] = false;
		keysUp[key] = true;
	}

	keys[key] = action != RELEASE;
}

bool Keyboard::KeyDown(const int key) {
	const bool x = keysDown[key];
	keysDown[key] = false;
	return x;
}

bool Keyboard::KeyUp(const int key) {
	const bool x = keysUp[key];
	keysUp[key] = false;
	return x;
}

bool Keyboard::KeyPressed(const int key) {
	return keys[key];
}
