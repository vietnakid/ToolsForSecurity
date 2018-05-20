# ToolsForSecurity

What this project can do ?
  - Show all information of PE files header.
  - Show all sections information of PE files.
  - Show which functions are inmported and exported of PE files.
  - Inject a user's custome ASM code to a PE file
  
How to use it ?
   - pass file path from argument and then choose option (inside code)
   - Uncomment lines of code to use it's functions
   
+ Show information: in file `getInfoOfFile.cpp`
+ Add code to file: in file `addCodeToFile.cpp` (just edit the following function and it works)
  - Add import function: `initImportFunctions()`
  - Add data: `addUserDataToDataSection()`
  - Add code: `getUserCode()`
