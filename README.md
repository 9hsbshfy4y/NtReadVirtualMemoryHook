# NtReadVirtualMemory Hook

Example of a DLL hook on `NtReadVirtualMemory`.  
The main idea is to intercept the system call and reduce the number of bytes actually read.

## Overview

Instead of allowing external tools (e.g. debuggers or memory inspection utilities) to read the full buffer,  
the hook limits the read size and only returns a reduced portion.  

Key points:
- Uses [MinHook](https://github.com/TsudaKageyu/minhook) to install the hook.
- Target function: `NtReadVirtualMemory` from `ntdll.dll`.
- Behavior: the requested read size is divided and truncated.

## Build Instructions

1. Install Visual Studio (x64, C++17 or higher).
3. Compile the project as a DLL (x64 debug mode).

## Usage

The DLL can be injected into a target process using any injector.  
Once loaded, the hook intercepts `NtReadVirtualMemory` and reduces the number of bytes being read.  

This makes memory inspection tools such as **Process Hacker 2** or **System Informer** display incomplete or truncated string results.
