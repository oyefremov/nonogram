#pragma once
#include <exception>
#include "windows.h"

class mutex
{
	HANDLE hMutex;
public:
	mutex();
	~mutex();
	void lock(DWORD timeout = INFINITE);
	void unlock();
};

class critical_section
{
	CRITICAL_SECTION s;
public:
	critical_section()	{ InitializeCriticalSection(&s);}
	bool try_enter()	{ return TryEnterCriticalSection(&s) != FALSE;}
	void enter()		{ EnterCriticalSection(&s);}
	void leave()		{ LeaveCriticalSection(&s);}
};

class synch
{
	critical_section& s;
public:
	synch(critical_section& _s) : s(_s)	{ s.enter();}
	~synch() { s.leave();}
};

class lock
{
	mutex& m;
public:
	lock(mutex& _m):m(_m) {m.lock();}
	~lock() {m.unlock();}
};

class timeoutexception : public std::exception
{
	char buffer[32];
public:
	timeoutexception(DWORD timeout);
	virtual const char* what() const {return buffer;}
};