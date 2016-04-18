#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <aio.h>
#include <errno.h>
#endif

extern "C" {
	struct IncomingInformation {
#ifdef _WIN32
		HANDLE hFile;
		DWORD number_of_bytes;
		CHAR buffer[100];
		DWORD position_input_file;
		DWORD position_output_file;
		OVERLAPPED Overlapped;
#elif __linux__
		int hFile;
		char buffer[100];
		size_t number_of_bytes;
		size_t number_of_trasferred_bytes;
		off_t position_input_file;
		off_t position_output_file;
		struct aiocb aiocbStruct;
#endif
	};

#ifdef _WIN32
	__declspec(dllexport) BOOL read(IncomingInformation *information) {
		BOOL readInfo;
		DWORD number_of_trasferred_bytes;
		information->Overlapped.Offset = information->position_input_file;

		ReadFile(information->hFile, information->buffer, information->number_of_bytes, NULL, &information->Overlapped);
		WaitForSingleObject(information->Overlapped.hEvent, INFINITE);
		readInfo = GetOverlappedResult(information->hFile, &information->Overlapped, &number_of_trasferred_bytes, FALSE);

		if (readInfo) {
			information->position_input_file = information->position_input_file + number_of_trasferred_bytes;
		}
		return readInfo;
#elif __linux__
	int read_async(struct IncomingInformation *information) {
		information->aiocbStruct.aio_offset = information->position_input_file;
		information->aiocbStruct.aio_fildes = information->hFile;
		information->aiocbStruct.aio_nbytes = information->number_of_bytes;
		aio_read(&information->aiocbStruct);

		while (aio_error(&information->aiocbStruct) == EINPROGRESS);
		information->number_of_trasferred_bytes = aio_return(&information->aiocbStruct);
		if (information->number_of_trasferred_bytes) {
			information->position_input_file = information->position_input_file + information->number_of_trasferred_bytes;
		}

		return information->number_of_trasferred_bytes;
#endif
	}

#ifdef _WIN32
	__declspec(dllexport) BOOL write(IncomingInformation *information) {
		BOOL writeInfo;
		DWORD number_of_trasferred_bytes;
		information->Overlapped.Offset = information->position_output_file;

		WriteFile(information->hFile, information->buffer, information->Overlapped.InternalHigh, NULL, &information->Overlapped);
		WaitForSingleObject(information->Overlapped.hEvent, INFINITE);
		writeInfo = GetOverlappedResult(information->hFile, &information->Overlapped, &number_of_trasferred_bytes, FALSE);

		if (writeInfo) {
			information->position_output_file = information->position_output_file + number_of_trasferred_bytes;
		}
		return writeInfo;
#elif __linux__
	int write_async(struct IncomingInformation *information) {
		int writeInfo;
		information->aiocbStruct.aio_offset = information->position_output_file;
		information->aiocbStruct.aio_fildes = information->hFile;
		information->aiocbStruct.aio_nbytes = information->number_of_trasferred_bytes;
		aio_write(&information->aiocbStruct);

		while ((writeInfo = aio_error(&information->aiocbStruct)) == EINPROGRESS);
		information->position_output_file = information->position_output_file + aio_return(&information->aiocbStruct);

		return writeInfo;
#endif
	}
}