- [sicxe_assembler](#sicxeassembler)
  - [Getting Started](#getting-started)

# sicxe_assembler

- **Make sure GTK is installed**
- **Run the application from within the MSYS2 Terminal when GTK UI is built**

- [Hashmap documentation here](https://developer.gnome.org/glib/2.64/glib-Hash-Tables.html)
- [GLIB Documentation](https://developer.gnome.org/glib/2.64/)
- [IBM Tutorials here](https://developer.ibm.com/technologies/linux/tutorials/l-glib/)

## Getting Started

```bash
# This is run automatically by VSCode CMake Tool
"C:\Program Files\CMake\bin\cmake.EXE" --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=D:\Software\msys64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=D:\Software\msys64\mingw64\bin\g++.exe -Hd:/cmpe220repo/c-lib-example/pthread -Bd:/cmpe220repo/c-lib-example/pthread/build -G Ninja
```

- Run the VSCode CMake tool
  - Select your GCC Kit (`$MSYSPATH/mingw64/bin/gcc.exe`)
  - **Control + Shift + P**
  - **CMake Configure**
- Build your program
  - `cmake --build build`
- Run the program
  - `<application>.exe`
