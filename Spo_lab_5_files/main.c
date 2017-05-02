#include <Windows.h>
#include <stdio.h>
#include "aio_file.h"
#include "thread_fun.h"
#include "WIN_API_help.h"

/**
This thread gets information from another thread
and writes it in file
*/



int main (void)
{
	struct FileMappingInformation fileMapping;
	char massage[BUF_SIZE];
	HANDLE	hFileHasBeenReadEvent, hFileHasBeenWrittenEvent, 
			hFMapHasBeenReadEvent, hFMapHasBeenWrittenEvent, 
			hAsyncWritingIsDoneEvent, hFile;
	struct ThreadInformation thread;

	DWORD nBytesToWrite      = BUF_SIZE;
    DWORD dwBytesWritten       = 0;
    DWORD dwFileSize;
	OVERLAPPED stOverlapped = {0};
	DWORD dwError  = 0;
	BOOL bResult   = FALSE;

	char *names[] = {
		".\MyFileMappingObject\0",		//name of file mapping objct
		"FileHasBeenReadEvent\0",				//name of event that file's been read
		"FileHasBeenWrittenEvent\0",			//etc
		"FileMappingObjHasBeenReadEvent\0",
		"FileMappingObjHasBeenWrittenEvent\0",
		"AsynchronousWritingFileEvent\0",
		"C:/Users/Greenwalrus/Google\ Диск/LabWorks/4Term/SSA/Spo_lab_5_files/OutputFile.txt\0",
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
		printf ("\Creation of %s is failed (%x) main.c.", names[1], GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hFileHasBeenWrittenEvent = CreateEvent (NULL, FALSE, FALSE, names[2])) ){
		printf ("\Creation of %s is failed (%x) main.c.", names[2], GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hFMapHasBeenReadEvent = CreateEvent (NULL, FALSE, FALSE, names[3])) ){
		printf ("\Creation of %s is failed (%x) main.c.", names[3], GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hFMapHasBeenWrittenEvent = CreateEvent (NULL, FALSE, FALSE, names[4])) ){
		printf ("\Creation of %s is failed (%x) main.c.", names[4], GetLastError());
		getchar ();
		return -1;
	}

	if ( !(hAsyncWritingIsDoneEvent = CreateEvent (NULL, FALSE, FALSE, names[5])) ){
		printf ("\Creation of %s is failed (%x) main.c.", names[5], GetLastError());
		getchar ();
		return -1;
	}
	
	if ( !(thread.handle = CreateThread (
								NULL,															// default security attributes
								0,																// use default stack size
								readFileThread,													// thread function name
								NULL,															// argument to thread function
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
		ExitThread (-1);
    }


	

	//while (1)
	//{
		
	//if (WaitForSingleObject (hFileHasBeenReadEvent, 10) == WAIT_OBJECT_0) {
		WaitForSingleObject (hFileHasBeenReadEvent, INFINITE);
		WaitForSingleObject (hFMapHasBeenWrittenEvent, INFINITE);
		//get inf from chld thread
		//strcpy (massage, (char*) fileMapping.pBuf);
		//printf ("\nGet message: ");
		//
		dwFileSize = GetFileSize(hFile, NULL);
		stOverlapped.hEvent = hAsyncWritingIsDoneEvent;

		stOverlapped.Offset = dwFileSize;

		bResult = WriteFile (hFile, (char*) fileMapping.pBuf, strlen ((char*) fileMapping.pBuf), &dwBytesWritten, &stOverlapped);
     
		dwError = GetLastError();
		
		if (!bResult && dwError && dwError != ERROR_IO_PENDING)
		{
			printf ("\nError of writing file file (%x).", dwError);
			getchar ();
			ExitThread (-1);
		}

		WaitForSingleObject (hAsyncWritingIsDoneEvent, INFINITE);
			
		//puts ((char*) fileMapping.pBuf);
		SetEvent (hFMapHasBeenReadEvent);
		//write here to file
		SetEvent (hFileHasBeenWrittenEvent);
	//} //else if (WaitForSingleObject (thread.handle, 10) == WAIT_OBJECT_0) break;
//}

	//close all handles
	UnmapViewOfFile (fileMapping.pBuf);
	CloseHandle (fileMapping.hMapFile);
	//WaitForSingleObject (thread.handle, INFINITE);
	CloseHandle (thread.handle);
	getchar ();

	return 0;
}