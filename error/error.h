#pragma once

#include <exception>
#include <string>

// auto link
#ifdef _DEBUG
	#pragma comment(lib, "errord.lib")
#else
	#pragma comment(lib, "error.lib")
#endif

class myexception : public std::exception
{
	std::string message;
public:
	myexception(std::string s) : message(s) {}
	virtual const char *what( ) const {return message.c_str();}
};

void error(const char* message, ...);
void WinAPIError(const char* message, ...);
