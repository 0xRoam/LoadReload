#pragma once
#include <windows.h>
#include <stdio.h>

unsigned long hashstr(const char* s);
void* findfunc(void* module, unsigned long hash);

/*-------------[TYPE DEFINITIONS]-------------*/

typedef struct _LSA_UNICODE_STRING { USHORT Length;	USHORT MaximumLength; PWSTR  Buffer; } UNICODE_STRING, * PUNICODE_STRING;
typedef struct _OBJECT_ATTRIBUTES { ULONG Length; HANDLE RootDirectory; PUNICODE_STRING ObjectName; ULONG Attributes; PVOID SecurityDescriptor;	PVOID SecurityQualityOfService; } OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;
typedef struct _CLIENT_ID { PVOID UniqueProcess; PVOID UniqueThread; } CLIENT_ID, * PCLIENT_ID;

/*-------------[FUNCTION TYPES]-------------*/


typedef NTSTATUS(NTAPI* fnLdrCallEnclave)(
	_In_ PENCLAVE_ROUTINE Routine,
	_In_ ULONG Flags, // ENCLAVE_CALL_FLAG_*
	_Inout_ PVOID* RoutineParamReturn
	);

typedef HMODULE(WINAPI* fnLoadLibraryExW)(
	_In_ LPCWSTR lpLibFileName,
	_In_ HANDLE hFile,
	_In_ DWORD dwFlags
	);

typedef NTSTATUS(NTAPI* fnNtProtectVirtualMemory)(
	_In_ HANDLE ProcessHandle,
	_Inout_ PVOID* BaseAddress,
	_Inout_ PSIZE_T RegionSize,
	_In_ ULONG NewProtect,
	_Out_ PULONG OldProtect
	);


typedef NTSTATUS(NTAPI* fnNtCreateSection)(OUT PHANDLE SectionHandle, IN ULONG DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN PLARGE_INTEGER MaximumSize OPTIONAL, IN ULONG PageAttributess, IN ULONG SectionAttributes, IN HANDLE FileHandle OPTIONAL);
typedef NTSTATUS(NTAPI* fnNtMapViewOfSection)(HANDLE SectionHandle, HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, SIZE_T CommitSize, PLARGE_INTEGER SectionOffset, PSIZE_T ViewSize, DWORD InheritDisposition, ULONG AllocationType, ULONG Win32Protect);
typedef NTSTATUS(NTAPI* fnNtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);
typedef NTSTATUS(NTAPI* fnNtAllocateVirtualMemory)(IN HANDLE ProcessHandle, IN OUT PVOID* BaseAddress, IN ULONG_PTR ZeroBits, IN OUT PSIZE_T RegionSize, IN ULONG AllocationType, IN ULONG Protect);


// More definitions needed to load sacrificial DLL
typedef enum _SECTION_INHERIT
{
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT;

#define NtCurrentProcess() ((HANDLE) -1)
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS       ((NTSTATUS)0x00000000L)
#endif
#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif
#define DELETE_HANDLE(h) if (h && h != INVALID_HANDLE_VALUE) { CloseHandle(h); h = NULL; }

