#include <Windows.h>
#include <stdio.h>

__declspec(dllexport)
DWORD ReadFile_gw (HANDLE hFile, char *buf, int bufSize, OVERLAPPED *stOverlapped)
{
	DWORD dwError  = 0, dwBytesRead;
	BOOL bResult   = FALSE;
	bResult = ReadFile(hFile,
						buf,
						bufSize,
						&dwBytesRead,
						stOverlapped); 
	dwError = GetLastError();
	if (!bResult && dwError && dwError != ERROR_IO_PENDING)
	{
		printf ("\nError of reading file (%x).", dwError);
		getchar ();
		ExitThread (-1);
	}
			
	WaitForSingleObject (stOverlapped->hEvent, INFINITE);
	bResult = GetOverlappedResult(hFile,
								stOverlapped,
								&dwBytesRead,
								FALSE) ;
	buf[dwBytesRead] = '\0';
	stOverlapped->Offset += dwBytesRead;
	return dwBytesRead;
}


__declspec(dllexport)
DWORD WriteFile_gw (HANDLE hFile, char *buf, OVERLAPPED *stOverlapped)
{
	DWORD dwError  = 0, dwBytesWritten, dwFileSize;
	BOOL bResult   = FALSE;

	dwFileSize = GetFileSize(hFile, NULL);
	stOverlapped->Offset = dwFileSize;

	bResult = WriteFile (hFile, buf, strlen (buf), &dwBytesWritten, stOverlapped);
     
	dwError = GetLastError();
		
	if (!bResult && dwError && dwError != ERROR_IO_PENDING)
	{
		printf ("\nError of writing file file (%x).", dwError);
		return -1;
	}

	WaitForSingleObject (stOverlapped->hEvent, INFINITE);
	bResult = GetOverlappedResult(hFile,
                                stOverlapped,
                                &dwBytesWritten,
                                FALSE) ;
	return dwBytesWritten;
}