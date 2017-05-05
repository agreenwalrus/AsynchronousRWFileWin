#ifndef WIN_API_HELP_H
#define WIN_API_HELP_H

#include <Windows.h>

#define BUF_SIZE 101


struct FileMappingInformation
{
	HANDLE hMapFile;
	LPCTSTR pBuf;
};

struct ThreadInformation
{
	DWORD identifier;
	HANDLE handle;

};


#endif WIN_API_HELP_H