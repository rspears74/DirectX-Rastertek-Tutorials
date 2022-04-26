#include "application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application* application;
	bool result;

	//create the application object
	application = new Application();
	if (!application)
	{
		return 0;
	}

	//init and run the application
	result = application->Initialize();
	if (result)
	{
		application->Run();
	}

	application->Shutdown();
	delete application;
	application = 0;

	return 0;
}