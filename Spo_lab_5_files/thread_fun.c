#include "thread_fun.h"

/**
This thread reads information from files
and send it to main thread
*/

DWORD WINAPI readFileThread (LPVOID lpParam)
{
	struct FileMappingInformation fileMapping;
	char dir[MAX_PATH], mask[] = "/*.txt\0", maskDir[MAX_PATH];
	HANDLE	hFileHasBeenReadEvent,		hFileHasBeenWrittenEvent,
			hFMapHasBeenReadEvent,		hFMapHasBeenWrittenEvent,
			hAsyncReadingIsDoneEvent,	hFile,
			hFindFile = INVALID_HANDLE_VALUE;

	WIN32_FIND_DATA ffd;

	DWORD nBytesToRead      = BUF_SIZE;
    DWORD dwBytesRead       = 0;
    DWORD dwFileSize;
	OVERLAPPED stOverlapped = {0};
	DWORD dwError  = 0;
	BOOL bResult   = FALSE;

	char *names[] = {
		"./MyFileMappingObject\0",
		"FileHasBeenReadEvent\0",
		"FileHasBeenWrittenEvent\0",
		"FileMappingObjHasBeenReadEvent\0",
		"FileMappingObjHasBeenWrittenEvent\0",
		"AsynchronousReadingFileEvent\0",
		NULL
	};

	strcpy_s (dir, MAX_PATH, (char*) lpParam);

	if (strlen (dir) + strlen (mask) > MAX_PATH)
		ExitThread (-1);
	else{
		strcpy_s (maskDir, MAX_PATH, dir);
		strcat_s (maskDir, MAX_PATH, mask);
	}

	if ( !(fileMapping.hMapFile = OpenFileMapping (
										FILE_MAP_ALL_ACCESS,			//read/write access
										FALSE,							//don't inherit the name
										names[0]))) {						
		printf ("\nOpening file mapping is faild (%x) thread_fun.c.", GetLastError());
		getchar ();
		return -1;
	}

	if ( !(fileMapping.pBuf = (LPTSTR) MapViewOfFile (
											fileMapping.hMapFile,		//handle to map object
											FILE_MAP_ALL_ACCESS,		// read/write permission
											0,							//hight offset
											0,							//low offset
											BUF_SIZE))) {
		printf ("\nCreating of map view of file is faild (%x) thread_fun.c.", GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hFileHasBeenReadEvent = CreateEvent (NULL, FALSE, FALSE, names[1])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[1], GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hFileHasBeenWrittenEvent = CreateEvent (NULL, FALSE, FALSE, names[2])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[2], GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hFMapHasBeenReadEvent = CreateEvent (NULL, FALSE, FALSE, names[3])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[3], GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hFMapHasBeenWrittenEvent = CreateEvent (NULL, FALSE, FALSE, names[4])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[4], GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hAsyncReadingIsDoneEvent = CreateEvent (NULL, FALSE, FALSE, names[5])) ){
		printf ("\nCreation of %s is failed (%x) main.c.", names[5], GetLastError());
		getchar ();
		ExitThread (-1);
	} 


	hFindFile = FindFirstFile(maskDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFindFile) 
		ExitThread (-1);



	do
   {
		strcpy_s (dir, MAX_PATH, (char*) lpParam);
		strcat_s (dir, MAX_PATH, "/\0");
		strcat_s (dir, MAX_PATH, ffd.cFileName);
		hFile = CreateFile(dir,                // file to open
                       GENERIC_READ,           // open for reading
                       FILE_SHARE_READ,        // share for reading
                       NULL,                   // default security
                       OPEN_EXISTING,          // existing file only
                       FILE_FLAG_OVERLAPPED,   // overlapped operation
                       NULL);                  // no attr. template
 
		if (hFile == INVALID_HANDLE_VALUE) 
		{ 
			printf("Could not open file (%d): %s\n",  GetLastError(), dir); 
			getchar ();
			ExitThread (-1);
		}
	
		dwFileSize = GetFileSize(hFile, NULL);
		stOverlapped.Offset = 0;
		stOverlapped.hEvent = hAsyncReadingIsDoneEvent;

		while (dwFileSize)
		{
			dwBytesRead = ReadFile_gw(hFile, (char*)fileMapping.pBuf, BUF_SIZE, &stOverlapped);

			if (dwBytesRead == -1)
			{
				printf ("\nError of reading file");
				ExitThread (-1);
			}

			dwFileSize -= dwBytesRead;

			SetEvent (hFileHasBeenReadEvent);
			SetEvent (hFMapHasBeenWrittenEvent);
			WaitForSingleObject (hFMapHasBeenReadEvent, INFINITE);
			WaitForSingleObject (hFileHasBeenWrittenEvent, INFINITE);
		}
		CloseHandle (hFile);
	} while (FindNextFile(hFindFile, &ffd) != 0);
	
	FindClose(hFindFile);
	UnmapViewOfFile (fileMapping.pBuf);
	CloseHandle (fileMapping.hMapFile);
	UnmapViewOfFile (fileMapping.pBuf);
	CloseHandle (fileMapping.hMapFile);
	CloseHandle (hFileHasBeenReadEvent);
	CloseHandle (hFileHasBeenWrittenEvent);
	CloseHandle (hFMapHasBeenWrittenEvent);
	CloseHandle (hFMapHasBeenReadEvent);
	CloseHandle (hAsyncReadingIsDoneEvent);
	CloseHandle (hFile);

	ExitThread (0);
}

