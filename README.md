# LLVM Pass : Simple memory profiler

## About

This repository contains a simple memory profiling LLVM pass developed for Clang compiler. During the compilation the pass searches the source code for load and store instructions. Everytime such instruction is encountered it modifies the source code by inserting a call to the C language 'printf' function in order to make the final program print out the address being read or written and in the case of the store instruction the value being written as well.

As the pass was developed within my university studies the complete project assignment can be found in [this file](Sujet.pdf) (in French only).

## Compatibility notice

The pass was tested only using the version 5.0.0 of LLVM and Clang. It may not be compatible with older versions.

## Build

Before you begin make sure you set the correct path to your LLVM installation in provided *CMakeLists* file (see **LLVM_INSTALL** variable). Then follow the instructions below in order to build the pass library:

```
mkdir build
cd build
cmake ..
make
cd ..
```

## Use

Use this command to apply the pass during the compilation process with Clang compiler:

```
clang -o <output binary file> -Xclang -load -Xclang ./build/libLLVMMemoryProfiler.so <C source file(s)>
```

To get only the IR file, do :

```
clang -o <output LLVM IR file> -S -emit-llvm -Xclang -load -Xclang ./build/libLLVMMemoryProfiler.so <C source file(s)>
```

## Example

The test source file provided along with the project assignment was [test.c](test.c). For this file the final binary output is as follows:

```
I am storing 0 at address 0x7ffc200a344c
I am storing 0 at address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am storing 0 at address 0x7ffc200a3430
I am loading address 0x7ffc200a342c
I am storing 1 at address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am storing 1 at address 0x7ffc200a3434
I am loading address 0x7ffc200a342c
I am storing 2 at address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am storing 2 at address 0x7ffc200a3438
I am loading address 0x7ffc200a342c
I am storing 3 at address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am storing 3 at address 0x7ffc200a343c
I am loading address 0x7ffc200a342c
I am storing 4 at address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
I am storing 4 at address 0x7ffc200a3440
I am loading address 0x7ffc200a342c
I am storing 5 at address 0x7ffc200a342c
I am loading address 0x7ffc200a342c
```

## Author

- **Marek Felsoci** - Student at *University of Strasbourg*
