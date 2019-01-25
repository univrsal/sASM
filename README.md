## sasm
**S**imple **As**e**m**bler

Uses a mnemonic definition file and a text assembler program to create a rom file,
which can be loaded into [Logisim](https://github.com/LogisimIt/Logisim).

#### Usage:
```
-h print this help screen
-m [path to mnemonic definitions]
-i [path to assembler file]
-o [path to output binary file]
-p print out loaded mnemonics
-d enable debug log
```
See test.txt for an example mnemonic definition, test.asm for an example program and test.rom1 for the resulting rom file.

#### Compiling
Requires [cmake](https://cmake.org).

Linux:
```
$ git clone https://github.com/univrsal/sasm.git
$ cd sasm
$ cmake ./
$ make
```
Windows:
1. Clone the repository
2. Open CMake Gui
3. Set "Where is the source code:" to the repository folder
4. Set "Where to build the binaries:" to any directory, the same as in 3. is fine
5. Click "Configure" and select your version of Visual Studio (x32/x64 is up to you)
6. Click "Generate" and then "Open Project"
