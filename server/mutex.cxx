#define _CRT_SECURE_NO_WARNINGS
#include "mutex.h"
#include "windows.h"
#include <error\error.h>

timeoutexception::timeoutexception(DWORD timeout)
{
	sprintf(buffer, "timeout of %d ms", timeout);
}

mutex::mutex()
{
	hMutex = CreateMutex(0, FALSE, 0);
	if (hMutex == 0)
		WinAPIError("Could not create mutex with CreateMutex(0, FALSE, 0)");

}
mutex::~mutex()
{
	CloseHandle(hMutex);
}

void mutex::lock(DWORD timeout)
{
	DWORD res = WaitForSingleObject(hMutex, timeout);
	switch (res)
	{
	case WAIT_OBJECT_0:
		return;
	case WAIT_ABANDONED:
		error("Wait for mutex was abandoned");
	case WAIT_TIMEOUT:
		throw timeoutexception(timeout);
	case WAIT_FAILED:
		WinAPIError("WaitForSingleObject failed");
	}
	error("Unknown error returned by WaitForSingleObject (res = %d)", res);
}

void mutex::unlock()
{
}

