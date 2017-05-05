#include <Windows.h>
#include <stdio.h>
//#include "aio_file.h"
#include "thread_fun.h"
#include "WIN_API_help.h"

/**
This thread gets information from another thread
and writes it in file
*/

typedef DWORD (__cdecl *WriteAsFileProc) (HANDLE, char *, OVERLAPPED *);

int main (void)
{	
	HINSTANCE hinstLib;
	WriteAsFileProc WriteAsFile;
	struct FileMappingInformation fileMapping;
	HANDLE	hFileHasBeenReadEvent, hFileHasBeenWrittenEvent, 
			hFMapHasBeenReadEvent, hFMapHasBeenWrittenEvent, 
			hAsyncWritingIsDoneEvent, hFile;
	struct ThreadInformation thread;

	DWORD nBytesToWrite      = BUF_SIZE;
    DWORD dwBytesWritten       = 0;
	OVERLAPPED stOverlapped = {0};
	DWORD dwError  = 0;
	BOOL bResult   = FALSE;

	char *names[] = {
		"./MyFileMappingObject\0",		//name of file mapping objct
		"FileHasBeenReadEvent\0",				//name of event that file's been read
		"FileHasBeenWrittenEvent\0",			//etc
		"FileMappingObjHasBeenReadEvent\0",
		"FileMappingObjHasBeenWrittenEvent\0",
		"AsynchronousWritingFileEvent\0",
		"C:\\Users\\Greenwalrus\\Google\ Диск\\LabWorks\\4Term\\SSA\\Spo_lab_5_files\\OutputFile.txt\0",
		NULL
	};

	if ( !(fileMapping.hMapFile = CreateFileMapping (
										INVALID_HANDLE_VALUE,						//use paging file
										NULL,										//default security
										PAGE_READWRITE,								//read/write access
										0,											//max obj siz (hight-order DWORD)
										BUF_SIZE,									//max obj siz (low-order DWORD)
										names[0]))) {
		printf ("\nCreating of file mapping is faild (%x) main.c.", GetLastError());
		getchar ();
		return -1;
	}

	//Sleep (10000);
	
	if ( !(fileMapping.pBuf = (LPTSTR) MapViewOfFile (
											fileMapping.hMapFile,		//handle to map object
											FILE_MAP_ALL_ACCESS,		// read/write permission
											0,
											0,
											BUF_SIZE))) {
		printf ("\nCreating of map view of file is faild (%x) main.c.", GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hFileHasBeenReadEvent = CreateEvent (NULL, FALSE, FALSE, names[1])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[1], GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hFileHasBeenWrittenEvent = CreateEvent (NULL, FALSE, FALSE, names[2])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[2], GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hFMapHasBeenReadEvent = CreateEvent (NULL, FALSE, FALSE, names[3])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[3], GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hFMapHasBeenWrittenEvent = CreateEvent (NULL, FALSE, FALSE, names[4])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[4], GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hAsyncWritingIsDoneEvent = CreateEvent (NULL, FALSE, FALSE, names[5])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[5], GetLastError());
		getchar ();
		return -1;
	}
	
	if ( !(thread.handle = CreateThread (
								NULL,															// default security attributes
								0,																// use default stack size
								readFileThread,													// thread function name
								"D:/TestFiles\0",															// argument to thread function
								0,																// use default creation flags
								&thread.identifier))) {
																								// returns the thread identifier
		printf ("\nError of creating thread (%x) main.c.", GetLastError());
		getchar ();
		return -1;
	}

	hFile = CreateFile(names[6],               // file to open
					   GENERIC_WRITE,          // open for writing
					   FILE_SHARE_WRITE,       // share for writing
                       NULL,                   // default security
					   OPEN_ALWAYS,            // existing file only
                       FILE_FLAG_OVERLAPPED,   // overlapped operation
                       NULL);                  // no attr. template
 
    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        printf("Could not open file (%d): %s\n",  GetLastError(), names[6]); 
        getchar ();
		TerminateThread (thread.handle, -1);
		WaitForSingleObject (thread.handle, INFINITE);
		ExitThread (-1);
    }

	
	stOverlapped.hEvent = hAsyncWritingIsDoneEvent;
	
	if ((hinstLib = LoadLibrary (TEXT("AsyncFileWork.dll"))) != NULL)
	{
		if ( (WriteAsFile = (WriteAsFileProc) GetProcAddress (hinstLib, "WriteFile_gw")) != NULL)
			while (1)
			{
		
				if (WaitForSingleObject (hFileHasBeenReadEvent, 10) == WAIT_OBJECT_0) {
					WaitForSingleObject (hFMapHasBeenWrittenEvent, INFINITE);

					dwBytesWritten = WriteAsFile (hFile, (char*) fileMapping.pBuf, &stOverlapped);

					if (dwBytesWritten == -1)
					{
						printf ("\nError of writing file");
						TerminateThread (thread.handle, -1);
						WaitForSingleObject (thread.handle, INFINITE);
						break;
					}

					SetEvent (hFMapHasBeenReadEvent);
					SetEvent (hFileHasBeenWrittenEvent);
				} else if (WaitForSingleObject (thread.handle, 10) == WAIT_OBJECT_0) break;
			}
		else printf ("\nError of loading proc (%x)", GetLastError());
		FreeLibrary(hinstLib);
	} else printf ("\nError of loading library (%x)", GetLastError());

	UnmapViewOfFile (fileMapping.pBuf);
	CloseHandle (fileMapping.hMapFile);
	CloseHandle (thread.handle);
	CloseHandle (hFileHasBeenReadEvent);
	CloseHandle (hFileHasBeenWrittenEvent);
	CloseHandle (hFMapHasBeenWrittenEvent);
	CloseHandle (hFMapHasBeenReadEvent);
	CloseHandle (hAsyncWritingIsDoneEvent);
	CloseHandle (hFile);
	
	printf ("Press any key...");
	getchar ();

	return 0;
}