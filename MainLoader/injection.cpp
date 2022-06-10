#include "injection.h"

void __stdcall ShellCode(MANUAL_MAPPING_DATA* pData);

uintptr_t ManualMap(HANDLE hProc, const char* szDllFile) {
	BYTE* pSrcData = nullptr;
	IMAGE_NT_HEADERS* pOldNtHeader = nullptr;
	IMAGE_OPTIONAL_HEADER* pOldOptHeader = nullptr;
	IMAGE_FILE_HEADER* pOldFileHeader = nullptr;
	BYTE* pTargetBase = nullptr;

	//Attempt to check file attributes to see if file exists
	DWORD dwCheck = 0;
	if (GetFileAttributesA(szDllFile) == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	//Opens file setting the cursor to the end of the file (ate flag)
	std::ifstream File(szDllFile, std::ios::binary | std::ios::ate);
	if (File.fail()) {
		File.close();
		return false;
	}
	//Tellg takes the current cursor position, therefore getting the file length
	auto FileSize = File.tellg();
	if (FileSize < 0x1000) { //The first 0x1000 bytes are reserved for the PE header, therefore a file can't be less than 0x1000 bytes long
		File.close();
		return false;
	}

	pSrcData = new BYTE[static_cast<UINT_PTR>(FileSize)];
	if (!pSrcData) {
		File.close();
		return false;
	}
	//Seeks to the beginning after reading file length
	File.seekg(0, std::ios::beg);
	//Put file in memory to the memory pointed to by pSrcData
	File.read(reinterpret_cast<char*>(pSrcData), FileSize);
	//Close file stream
	File.close();

	//Check MZ header integrity
	if (reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_magic != 0x5A4D) {
		delete[] pSrcData;
		return false;
	}

	pOldNtHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(pSrcData + reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_lfanew); //e_lfanew is at the bottom of the MZ header and contains the offset to the start of the NT header
	pOldOptHeader = &pOldNtHeader->OptionalHeader; //NT header contains the address of the FileHeader and OptionalHeader, together with a 4-byte signature "PE\0\0"
	pOldFileHeader = &pOldNtHeader->FileHeader; 

#ifdef _WIN64
	if (pOldFileHeader->Machine != IMAGE_FILE_MACHINE_AMD64){
		delete[] pSrcData;
		return false;
	}
#else
	if (pOldFileHeader->Machine != IMAGE_FILE_MACHINE_I386) {
		delete[] pSrcData;
		return false;
	}
#endif
	//Attempt to allocate memory for the image in the preferred location, otherwise in a random one (Will require relocation in the latter case)
	pTargetBase = reinterpret_cast<BYTE*>(VirtualAllocEx(hProc, reinterpret_cast<void*>(pOldOptHeader->ImageBase), pOldOptHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	if (!pTargetBase) {
		pTargetBase = reinterpret_cast<BYTE*>(VirtualAllocEx(hProc, nullptr, pOldOptHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
		if (!pTargetBase) {
			delete[] pSrcData;
			return false;
		}
	}

	MANUAL_MAPPING_DATA data{ 0 };
	data.pLoadLibraryA = LoadLibraryA;
	data.pGetProcAddress = reinterpret_cast<f_GetProcAddress>(GetProcAddress);

	//Attempt to write each section into memory manually
	auto* pSectionHeader = IMAGE_FIRST_SECTION(pOldNtHeader);
	for (UINT i = 0; i != pOldFileHeader->NumberOfSections; ++i, ++pSectionHeader) {
		if (pSectionHeader->SizeOfRawData) {
			//Virtual Address is the offset where the section must be loaded in memory from the start of the module
			if (!WriteProcessMemory(hProc, pTargetBase + pSectionHeader->VirtualAddress, pSrcData + pSectionHeader->PointerToRawData, pSectionHeader->SizeOfRawData, nullptr)) {
				delete[] pSrcData;
				VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
				return false;
			}
		}
	}

	//Copy PE headers (they are 0x1000 bytes in size)
	memcpy(pSrcData, &data, sizeof(data));
	WriteProcessMemory(hProc, pTargetBase, pSrcData, 0x1000, nullptr);
	delete[] pSrcData;

	//Write ShellCode to mem and execute
	void* pShellCode = VirtualAllocEx(hProc, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pShellCode) {
		VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
		return false;
	}

	WriteProcessMemory(hProc, pShellCode, ShellCode, 0x1000, nullptr);

	HANDLE hThread = CreateRemoteThread(hProc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(pShellCode), pTargetBase, 0, nullptr);
	if (!hThread) {
		VirtualFreeEx(hProc, pTargetBase, 0, MEM_RELEASE);
		VirtualFreeEx(hProc, pShellCode, 0, MEM_RELEASE);
		return false;
	}

	CloseHandle(hThread);
	//Deallocate shellcode memory
	HINSTANCE hCheck = NULL;
	while (!hCheck) {
		MANUAL_MAPPING_DATA data_checked{ 0 };
		ReadProcessMemory(hProc, pTargetBase, &data_checked, sizeof(data_checked), nullptr);
		hCheck = data_checked.hMod;
		Sleep(10);
	}

	VirtualFreeEx(hProc, pShellCode, 0, MEM_RELEASE);

	return (uintptr_t)pTargetBase;
}

//This function will get the VA of the function, but doesn't work for forwarded exports (means function is imported from another module)
uintptr_t ResolveFunctionPtr(uintptr_t pBase, const wchar_t* szMod)
{
	//Load the export table
	void* procAddr = nullptr;
	auto* pOpt = &reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>((BYTE*)pBase)->e_lfanew)->OptionalHeader;
	auto* pExportTable = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	//Load the function name array (AddressOfNames member)

	unsigned int* NameRVA = (unsigned int*)(pBase + pExportTable->AddressOfNames);

	//Iterate over AddressOfNames
	for (int i = 0; i < pExportTable->NumberOfNames; i++) {
		//Calculate Absolute Address and cast
		char* name = (char*)(pBase + NameRVA[i]);
		wchar_t* wname = CharToWChar_T(name);
		if (!_wcsicmp(wname, szMod)) {
			free(wname);

			//Lookup Ordinal
			unsigned short NameOrdinal = ((unsigned short*)(pBase + pExportTable->AddressOfNameOrdinals))[i];

			//Use Ordinal to Lookup Function Address and Calculate Absolute
			unsigned int addr = ((unsigned int*)(pBase + pExportTable->AddressOfFunctions))[NameOrdinal];

			procAddr = (void*)(pBase + addr);

			break;
		}
		if (wname != nullptr) {
			free(wname);
		}
	}

	return (uintptr_t)procAddr;
}

#define RELOC_FLAG32(RelInfo)((RelInfo >> 0x0C) == IMAGE_REL_BASED_HIGHLOW)
#define RELOC_FLAG64(RelInfo)((RelInfo >> 0x0C) == IMAGE_REL_BASED_DIR64)

#ifdef _WIN64
#define RELOC_FLAG RELOC_FLAG64
#else
#define RELOC_FLAG RELOC_FLAG32
#endif

void __stdcall ShellCode(MANUAL_MAPPING_DATA* pData)
{
	if (!pData) return;

	BYTE* pBase = reinterpret_cast<BYTE*>(pData);
	auto* pOpt = &reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>(pData)->e_lfanew)->OptionalHeader;

	auto _LoadLibraryA = pData->pLoadLibraryA;
	auto _GetProcAddress = pData->pGetProcAddress;
	auto _DllMain = reinterpret_cast<f_DLL_ENTRY_POINT>(pBase + pOpt->AddressOfEntryPoint);

	//Do any eventual relocation, if necessary
	BYTE* LocationDelta = pBase - pOpt->ImageBase; //Difference between the preferred and actual image base
	if (LocationDelta) {
		if (!pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
			return;

		//Each IMAGE_BASE_RELOCATION is an object with a VA offset and a list members that rely on that offset base.
		//Therefore to adjust everything you must go through all the .reloc section and shift every "member" by LocationDelta
		auto* pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
		while (pRelocData->VirtualAddress) {
			UINT AmountOfEntries = (pRelocData->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
			WORD* pRelativeInfo = reinterpret_cast<WORD*>(pRelocData + 1);
			for (UINT i = 0; i != AmountOfEntries; i++, pRelativeInfo++) {
				if (RELOC_FLAG(*pRelativeInfo)) {
					UINT_PTR* pPatch = reinterpret_cast<UINT_PTR*>(pBase + pRelocData->VirtualAddress + ((*pRelativeInfo) & 0xFFF));
					*pPatch += reinterpret_cast<UINT_PTR>(LocationDelta);
				}
			}
			pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<BYTE*>(pRelocData) + pRelocData->SizeOfBlock);
		}
	}

	//Fixing imports
	if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size) {
		auto* pImportDescr = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		while (pImportDescr->Name) {
			char* szMod =  reinterpret_cast<char*>(pBase + pImportDescr->Name); //szMod contains the name of the module to be loaded
			HINSTANCE hDll = _LoadLibraryA(szMod);
			ULONG_PTR* pThunkRef = reinterpret_cast<ULONG_PTR*>(pBase + pImportDescr->OriginalFirstThunk);
			ULONG_PTR* pFuncRef = reinterpret_cast<ULONG_PTR*>(pBase + pImportDescr->FirstThunk);

			if (!pThunkRef) {
				pThunkRef = pFuncRef;
			}

			for (; *pThunkRef; ++pThunkRef, ++pFuncRef) {
				if (IMAGE_SNAP_BY_ORDINAL(*pThunkRef)) {
					//In this case pThunkRef contains the ordinal number which represents the function
					*pFuncRef = _GetProcAddress(hDll, reinterpret_cast<char*>(*pThunkRef & 0xffff));
				}
				else {
					//Else pThunkRef contains an offset that will point to the Import where the name can be taken from for the loading
					auto* pImport = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(pBase + (*pThunkRef));
					*pFuncRef = _GetProcAddress(hDll, pImport->Name);
				}
			}
			++pImportDescr;
		}
	}

	//Execute Thread Local Storage
	if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size) {
		auto* pTLS = reinterpret_cast<IMAGE_TLS_DIRECTORY*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
		auto* pCallback = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(pTLS->AddressOfCallBacks);
		//Call all callbacks
		for (; pCallback && *pCallback; ++pCallback) {
			(*pCallback)(pBase, DLL_PROCESS_ATTACH, nullptr);
		}
	}

	//Call DllMain
	if(pOpt->AddressOfEntryPoint)
		_DllMain(pBase, DLL_PROCESS_ATTACH, nullptr);

	pData->hMod = reinterpret_cast<HINSTANCE>(pBase);
}

/*
* StringLengthA
*
* Use: Retrieve length of char string
* Parameters: char string
* Return: Length of string
*/
int StringLengthA(char* str) {
	int length;

	for (length = 0; str[length] != '\0'; length++) {}
	return length;
}

/*
* CharToWChar_T
*
* Use: Convert char string to wchar_t string - caller responsible for freeing memory
* Parameters: char string
* Return: wchar_t string
*/
wchar_t* CharToWChar_T(char* str) {
	int length = StringLengthA(str);

	if (str == nullptr) {
		return nullptr;
	}

	wchar_t* wstr_t = (wchar_t*)malloc(sizeof(wchar_t) * length + 2);

	for (int i = 0; i < length; i++) {
		wstr_t[i] = str[i];
	}
	wstr_t[length] = '\0';
	return wstr_t;
}