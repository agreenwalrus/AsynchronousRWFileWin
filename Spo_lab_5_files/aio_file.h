#ifndef AIO_FILE_H
#define AIO_FILE_H

//#include <WinDef.h>
#include "WIN_API_help.h"
#include <stdio.h>

DWORD ReadFile_gw (HANDLE hFile, char *buf, int bufSize, OVERLAPPED *stOverlapped);
DWORD WriteFile_gw (HANDLE hFile, char *buf, OVERLAPPED *stOverlapped);

#endif AIO_FILE_H