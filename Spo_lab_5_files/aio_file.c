#include "aio_file.h"



//DWORD ReadFile_gw (HANDLE hFile, char *buf, int bufSize, OVERLAPPED *stOverlapped, HANDLE hAsyncReadingIsDoneEvent)
//{
//	DWORD dwError  = 0, dwBytesRead;
//	BOOL bResult   = FALSE;
//	bResult = ReadFile(hFile,
//						buf,
//						bufSize,
//						&dwBytesRead,
//						stOverlapped); 
//	dwError = GetLastError();
//	if (!bResult && dwError && dwError != ERROR_IO_PENDING)
//	{
//		printf ("\nError of reading file (%x).", dwError);
//		getchar ();
//		ExitThread (-1);
//	}
//			
//	WaitForSingleObject (hAsyncReadingIsDoneEvent, INFINITE);
//	bResult = GetOverlappedResult(hFile,
//								stOverlapped,
//								&dwBytesRead,
//								FALSE) ;
//	buf[dwBytesRead] = '\0';
//	stOverlapped->Offset += dwBytesRead;
//	return dwBytesRead;
//}