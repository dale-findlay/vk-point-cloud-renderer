#include "CoreMacros.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef WIN32
bool __HandleError(const char* file, int line)
{
	std::stringstream errorString;
	errorString << "Assertion failed in file: " << file << " at line: " << line;

	int returnCode = MessageBox(NULL, errorString.str().c_str(), "Assertion failed", MB_ABORTRETRYIGNORE);

	if (returnCode == IDABORT)
	{
		return true;
	}

	return false;
}
#endif
