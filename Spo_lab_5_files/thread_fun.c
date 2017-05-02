#include "thread_fun.h"

/**
This thread reads information from files
and send it to main thread
*/

DWORD WINAPI readFileThread (LPVOID lpParam)
{
	struct FileMappingInformation fileMapping;
	char massage[BUF_SIZE];
	HANDLE	hFileHasBeenReadEvent,		hFileHasBeenWrittenEvent,
			hFMapHasBeenReadEvent,		hFMapHasBeenWrittenEvent,
			hAsyncReadingIsDoneEvent,	hFile;

	DWORD nBytesToRead      = BUF_SIZE;
    DWORD dwBytesRead       = 0;
    DWORD dwFileSize;
	OVERLAPPED stOverlapped = {0};
	DWORD dwError  = 0;
	BOOL bResult   = FALSE;

	char *names[] = {
		".\MyFileMappingObject\0",
		"FileHasBeenReadEvent\0",
		"FileHasBeenWrittenEvent\0",
		"FileMappingObjHasBeenReadEvent\0",
		"FileMappingObjHasBeenWrittenEvent\0",
		"AsynchronousReadingFileEvent\0",
		"C:/Users/Greenwalrus/Google\ Диск/LabWorks/4Term/SSA/Spo_lab_5_files/input_file.txt\0",
		NULL
	};

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
		printf ("\Creation of %s is failed (%x) main.c.", names[1], GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hFileHasBeenWrittenEvent = CreateEvent (NULL, FALSE, FALSE, names[2])) ){
		printf ("\Creation of %s is failed (%x) main.c.", names[2], GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hFMapHasBeenReadEvent = CreateEvent (NULL, FALSE, FALSE, names[3])) ){
		printf ("\Creation of %s is failed (%x) main.c.", names[3], GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hFMapHasBeenWrittenEvent = CreateEvent (NULL, FALSE, FALSE, names[4])) ){
		printf ("\Creation of %s is failed (%x) main.c.", names[4], GetLastError());
		getchar ();
		ExitThread (-1);
	}

	if ( !(hAsyncReadingIsDoneEvent = CreateEvent (NULL, FALSE, FALSE, names[5])) ){
		printf ("\Creation of %s is failed (%x) main.c.", names[5], GetLastError());
		getchar ();
		ExitThread (-1);
	} 

	hFile = CreateFile(names[6],                // file to open
                       GENERIC_READ,           // open for reading
                       FILE_SHARE_READ,        // share for reading
                       NULL,                   // default security
                       OPEN_EXISTING,          // existing file only
                       FILE_FLAG_OVERLAPPED,   // overlapped operation
                       NULL);                  // no attr. template
 
    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        printf("Could not open file (%d): %s\n",  GetLastError(), names[6]); 
        getchar ();
		ExitThread (-1);
    }
	
	dwFileSize = GetFileSize(hFile, NULL);
	stOverlapped.hEvent = hAsyncReadingIsDoneEvent;

	//while (1)
	//{
		//if (!strcmp(massage, "q\0"))//вместо этого
			//break;					//проверк на конец файла
		
		// read here async file


		bResult = ReadFile(hFile,
                           (char*) fileMapping.pBuf,
                           nBytesToRead,
                           &dwBytesRead,
                           &stOverlapped); 
     
        dwError = GetLastError();
		
		if (!bResult && dwError && dwError != ERROR_IO_PENDING)
		{
			printf ("\nError of reading file (%x).", dwError);
			getchar ();
			ExitThread (-1);
		}

		WaitForSingleObject (hAsyncReadingIsDoneEvent, INFINITE);


		// read here async file

		SetEvent (hFileHasBeenReadEvent);

		//send inf to main thred
		/*printf ("\nPut in message: ");
		gets ((char*) fileMapping.pBuf);
		if (!strcmp((char*) fileMapping.pBuf, "q\0")) {	//вместо этого
			SetEvent (hFMapHasBeenWrittenEvent);
			break;
		}*/
		
		//strcpy ((char*) fileMapping.pBuf, massage);
		SetEvent (hFMapHasBeenWrittenEvent);
		WaitForSingleObject (hFMapHasBeenReadEvent, INFINITE);

		WaitForSingleObject (hFileHasBeenWrittenEvent, INFINITE);
	//}

	UnmapViewOfFile (fileMapping.pBuf);
	CloseHandle (fileMapping.hMapFile);
	//printf ("\nJulia is cool");


	ExitThread (0);
}

