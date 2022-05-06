# Warmup: Introduction to C++

## About this exercise

In this exercise, you will be writing some simple C++ code to familiarize
yourself with C++ and some of the STL (Standard Template Library) classes
that will be used throughout the course, including maps and sets.

This exercise is targeted at those who are comfortable with programming languages
including C, Java or Python, but who have not worked with C++.

In this exercise, you will complete the skeleton code provided in `src/main.cpp`
to build a miniature todo list.
Even though you have to only edit `src/main.cpp`,
please read the code from the other files under the `src` and `include` directories
and understand the role that the classes `User`, `Task`, `Bill` and `Misc` play,
as well as various functions that you can use
to initialize their objects and access their properties.

## Building the code

Please read the comments in the `src/main.cpp` file to proceed with the exercise.
This exercise uses CMake, a tool that will be used in almost every lab, to build
the C++ code. Read more about CMake here: https://cmake.org/.

To get started, perform the following commands inside this  directory.

```sh
mkdir build
cd build
cmake ..
```

The above set of commands ensures that any required dependencies are met,
and produces a set of files that can be used to build the program.

In particular, it generates a Makefile that is used to actually build the program.
You can run `make` from the build directory to make your program:

```sh
make
```

On a successful build make should output something along the following lines:

```
Scanning dependencies of target main
[ 25%] Building CXX object CMakeFiles/main.dir/src/main.cpp.o
[ 50%] Linking CXX executable main
[100%] Built target main
```

And you will see an executable file `build/main`.
Run it by using the following command from the `build` directory:

```sh
./main
```

## Debugging your code (optional)

In case you wish to debug your code,
or even experiment with debuggers like GDB provided in the docker image,
you will need to do the following:

1. Remove all files from the build directory and
generate the build files again using CMake with the debug option enabled.
To achieve this you can run the following from the build directory:

```sh
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

2. Build your code using `make`:

```sh
make
```

3. Either use the debugging interface provided by VSCode
(instructions [here](https://www.coursera.org/learn/cis547-2021c/lecture/vZpDf/walkthrough-set-up-the-course-vm)),
or use the command line interface for GDB:

```sh
gdb ./main
```

If you are using the command line interface for GDB,
we expect you to either know how to use it,
or to go through a quick tutorial for the same.
