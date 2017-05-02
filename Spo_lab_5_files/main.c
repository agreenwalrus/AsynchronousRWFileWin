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
	HANDLE hFileHasBeenReadEvent, hFileHasBeenWrittenEvent, hFMapHasBeenReadEvent, hFMapHasBeenWrittenEvent, hAsyncWritingIsDoneEvent;
	struct ThreadInformation thread;

	char *names[] = {
		".\MyFileMappingObject\0",		//name of file mapping objct
		"FileHasBeenReadEvent\0",				//name of event that file's been read
		"FileHasBeenWrittenEvent\0",			//etc
		"FileMappingObjHasBeenReadEvent\0",
		"FileMappingObjHasBeenWrittenEvent\0",
		"AsynchronousWritingFileEvent\0",
		"OutputFile",
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

	//while (1)
	//{
		
		if (WaitForSingleObject (hFileHasBeenReadEvent, 10) == WAIT_OBJECT_0) {
			WaitForSingleObject (hFMapHasBeenWrittenEvent, INFINITE);
			//get inf from chld thread
			//strcpy (massage, (char*) fileMapping.pBuf);
			printf ("\nGet message: ");
			puts ((char*) fileMapping.pBuf);
			SetEvent (hFMapHasBeenReadEvent);
			//write here to file
			SetEvent (hFileHasBeenWrittenEvent);
		} //else if (WaitForSingleObject (thread.handle, 10) == WAIT_OBJECT_0) break;
	//}

	//close all handles
	UnmapViewOfFile (fileMapping.pBuf);
	CloseHandle (fileMapping.hMapFile);
	//WaitForSingleObject (thread.handle, INFINITE);
	CloseHandle (thread.handle);
	getchar ();

	return 0;
}