#include "input.h"

Input::Input()
{

}

Input::Input(const Input&)
{

}

Input::~Input()
{

}

void Input::Initialize()
{
	int i;

	//init all the keys to being released and not pressed
	for (i = 0; i < 256; i++)
	{
		keys[i] = false;
	}

	return;
}

void Input::KeyDown(unsigned int input)
{
	keys[input] = true;
	return;
}

void Input::KeyUp(unsigned int input)
{
	keys[input] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key)
{
	return keys[key];
}