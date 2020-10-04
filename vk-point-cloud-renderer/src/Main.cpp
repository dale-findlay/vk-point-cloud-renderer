#include "core/CoreMinimal.h"
#include "application/Application.h"
#include "memory/MemoryAllocator.h"

int RunApplication()
{
	vkpc::Application* application = vkpcnew vkpc::Application();

	int returnCode = -1;
	if (application->Initialize())
	{
		returnCode = application->Run();
	}

	application->Shutdown();

	delete application;

	return returnCode;
}

int main()
{
	vkpc::MemoryAllocator::Initialize();

	int returnCode = RunApplication();

	/*	
		Don't want to assert on this when the application doesn't initialize properly,
		it causes me to think its a memory leak instead of checking if the system initialized properly :)
	*/
	if (returnCode != -1)
	{
		//By this point all the memory should be deallocated by application.
		VKPC_ASSERT(!vkpc::MemoryAllocator::IsMemoryAllocated());
	}
	else
	{
	}

#ifdef WIN32
    system("PAUSE");
#elif __APPLE__
     system( "read -n 1 -s -p \"Press any key to continue...\"" );
#endif
    
	return returnCode;
}
