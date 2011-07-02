#define _CRT_SECURE_NO_WARNINGS

#include "error.h"
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <sstream>
#include <Windows.h>

std::string vformat_string(const char* format, va_list argptr)
{
	if (format == 0)
		return "";

	int chars = _vscprintf(format, argptr); 
	if (chars == 0)
		return "";

	std::vector<char> buffer(chars + 1); // +1 to include zero terminator
	vsprintf(&buffer[0], format, argptr); 
	return std::string(buffer.begin(), buffer.end() - 1);
}

std::string format_string(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);

	return vformat_string(format, argptr);
}

std::string system_error_message(int lastError)
{
    char msg[1024 * 4];

    // If this program was multi-threaded, we'd want to use
    // FORMAT_MESSAGE_ALLOCATE_BUFFER instead of a static buffer here.
    // (And of course, free the buffer when we were done with it)

	DWORD res = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
		0, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msg, sizeof(msg), 0);
	if (res == 0) 
		return format_string("Error getting error message, error code %d", GetLastError());
    return msg;
}


void error(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	throw myexception(vformat_string(format, argptr));
}

void WinAPIError(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);

	int error = WSAGetLastError();

	std::string msg = vformat_string(format, argptr);

	std::ostringstream s;
	s << msg << "\n"
		<< "Error Code = " << error << "\n"
		<< system_error_message(error);

	throw myexception(s.str());
}
