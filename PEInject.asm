.386 
.model flat,stdcall 

option casemap:none 

include C:\masm32\include\windows.inc 
include C:\masm32\include\kernel32.inc
includelib C:\masm32\lib\kernel32.lib
include C:\masm32\include\masm32.inc 
includelib C:\masm32\lib\masm32.lib
include C:\masm32\include\user32.inc
includelib C:\masm32\lib\user32.lib

include c:\masm32\macros\macros.asm

.data
    errorNotPEFileMsg   db  'This file is not a PEFile', 0Ah, 0
    errorOpenFileMsg   db  'Cannt Open File', 0Ah, 0
    errorFindCodeSectionMsg db  'There are some error when finding for code section', 0

    filePath    db  'C:\Users\KiDctf\Desktop\TestPEFileInject\BASECALC.EXE', 0
    ; filePath    db  'C:\Users\KiDctf\Desktop\TestPEFileInject\test.txt', 0

    fileHandle  dd  ?
    BytesRead   dd ?

    PEOffset    dd  ?
    MZOffset  dd  ?
    PEHeaderOffset  dd  ?
    originalEntryPoint   dd  ?
    imagePage   dd  ?
    numberOfSection dd  ?
    codeSectionOffset   dd  ?
    codeSectionVA   dd  ?
    sectionVirtualSize   dd  ?
    sectionVirtualAddress   dd  ?

    lpBuffer    dd  0
.code

main:1

getFileHandle:
    push 0                              ; hTemplateFile
    push FILE_ATTRIBUTE_NORMAL          ; dwFlagsAndAttributes
    push OPEN_EXISTING                  ; dwCreationDisposition
    push 0                              ; lpSecurityAttributes
    push 0                              ; dwShareMode
    push GENERIC_READ + GENERIC_WRITE   ; dwDesiredAccess
    push offset filePath                ; lpFileName
    call CreateFile
    cmp eax, INVALID_HANDLE_VALUE
    je errorOpenFile
    mov fileHandle, eax                 ; Now we get the handle of the file
    

checkPEFile:
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesRead
    push 2                              ; nNumberOfBytesToRead
    push offset MZOffset                ; lpBuffer
    push fileHandle                     ; hFile
    call ReadFile
    cmp MZOffset, 5A4Dh     ; MZ
    jne errorNotPEFile

    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push 3Ch                            ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesRead
    push 4                              ; nNumberOfBytesToRead
    push offset PEHeaderOffset          ; lpBuffer
    push fileHandle                     ; hFile
    call ReadFile
    
    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push PEHeaderOffset                 ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesRead
    push 4                              ; nNumberOfBytesToRead
    push offset PEOffset                ; lpBuffer
    push fileHandle                     ; hFile
    call ReadFile
    cmp PEOffset, 00004550h     ; PE00
    jne errorNotPEFile

getOriginalEntryPoint:
    mov ebx, PEHeaderOffset
    add ebx, 28h
    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push ebx                            ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesRead
    push 4                              ; nNumberOfBytesToRead
    push offset originalEntryPoint      ; lpBuffer
    push fileHandle                     ; hFile
    call ReadFile

getImageBase:
    mov ebx, PEHeaderOffset
    add ebx, 34h
    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push ebx                            ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesRead
    push 4                              ; nNumberOfBytesToRead
    push offset imagePage               ; lpBuffer
    push fileHandle                     ; hFile
    call ReadFile

getNumberOfSection:
    mov ebx, PEHeaderOffset
    add ebx, 6h
    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push ebx                            ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesRead
    push 2                              ; nNumberOfBytesToRead
    push offset numberOfSection         ; lpBuffer
    push fileHandle                     ; hFile
    call ReadFile

getOffsetCodeSection:
    mov ecx, numberOfSection
    mov ebx, PEHeaderOffset
    add ebx, 248    ; Offset of first section
    sub ebx, 40
    push ebx    ; save current offset of section
    loopSections:
        dec ecx
        cmp ecx, 0
        je errorFindCodeSection
        pop ebx
        add ebx, 40
        push ebx
        add ebx, 8  ; Virutal Size
        push FILE_BEGIN                     ; dwMoveMethod
        push NULL                           ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push fileHandle                     ; hFile
        call SetFilePointer
        push 0                              ; lpOverlapped
        push offset BytesRead               ; lpNumberOfBytesRead
        push 4                              ; nNumberOfBytesToRead
        push offset sectionVirtualSize      ; lpBuffer
        push fileHandle                     ; hFile
        call ReadFile
        add ebx, 4  ; Virutal Address

        push FILE_BEGIN                     ; dwMoveMethod
        push NULL                           ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push fileHandle                     ; hFile
        call SetFilePointer
        push 0                              ; lpOverlapped
        push offset BytesRead               ; lpNumberOfBytesRead
        push 4                              ; nNumberOfBytesToRead
        push offset sectionVirtualAddress   ; lpBuffer
        push fileHandle                     ; hFile
        call ReadFile

        mov edx, originalEntryPoint
        cmp edx, sectionVirtualAddress
        jb loopSections
        mov eax, sectionVirtualAddress
        add eax, sectionVirtualSize
        cmp edx, eax
        ja loopSections
        xchg codeSectionVA, sectionVirtualAddress
        
        add ebx, 8  ; Raw Address
        push FILE_BEGIN                     ; dwMoveMethod
        push NULL                           ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push fileHandle                     ; hFile
        call SetFilePointer
        push 0                              ; lpOverlapped
        push offset BytesRead               ; lpNumberOfBytesRead
        push 4                              ; nNumberOfBytesToRead
        push offset codeSectionOffset       ; lpBuffer
        push fileHandle                     ; hFile
        call ReadFile
    

EverythingOk:
    jmp closeFileHandle

errorOpenFile:
    push offset errorOpenFileMsg
    call StdOut
    jmp closeFileHandle

errorNotPEFile:
    push offset errorNotPEFileMsg
    call StdOut

errorFindCodeSection:
    push offset errorFindCodeSectionMsg
    call StdOut

closeFileHandle:
    push fileHandle
    call CloseHandle

exit:
    push 0
    call ExitProcess
END main