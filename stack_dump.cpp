#include <iostream>
#include <execinfo.h>
#include <cxxabi.h>

void stack_dump(int depth, std::ostream& out) {
    void* frames[50];
    int size = backtrace(frames, 50);
    char** names = backtrace_symbols(frames, size);

    for (int i = 1; i < size && i <= depth; i++) {
        std::string name = names[i];
        size_t start = name.find('(');
        size_t end = name.find('+', start);
        std::string mangled = name.substr(start + 1, end - start - 1);
        int status;
        char* realname = abi::__cxa_demangle(mangled.c_str(), 0, 0, &status);
        if (status == 0) {
            out << realname << "\n";
            free(realname);
        } else {
            out << name << "\n";
        }
    }
    free(names);
}

void function_0() {
    stack_dump(3, std::cout);
}

void function_1() {
    function_0();
}

int main() {
    function_1();
    return 0;
}


//backtrace: Gets raw addresses (like GPS coordinates).
// backtrace_symbols: Turns those into readable (but still coded) names (like street signs).


// Program Start:
// The program starts in main, which calls function_1.
// Call Stack Build-Up:
// function_1 calls function_0.
// function_0 calls stack_dump(3, std::cout).
// At this point, the call stack (from top to bottom) is:
// stack_dump
// function_0
// function_1
// main
// (some system-level functions like _start or libc_start_main, depending on the platform)
// Stack Trace Capture:
// stack_dump calls backtrace(frames, 50), capturing the stack frames. Suppose it captures 6 frames:
// Frame 0: stack_dump
// Frame 1: function_0
// Frame 2: function_1
// Frame 3: main
// Frame 4: _start (or similar)
// Frame 5: libc_start_main (or similar)
// size = 6 (6 frames captured).
// Convert Frames to Strings:
// backtrace_symbols(frames, size) converts the frames into strings, e.g.:
// Frame 0: ./a.out(_Z10stack_dumpiRSo+0x40) [0x400b23]
// Frame 1: ./a.out(_Z9function_0v+0x10) [0x400b50]
// Frame 2: ./a.out(_Z9function_1v+0x10) [0x400b70]
// Frame 3: ./a.out(main+0x10) [0x400b90]
// Frame 4: /lib/x86_64-linux-gnu/libc.so.6(_start+0x29) [0x7f8b1c0]
// Frame 5: /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf0) [0x7f8b1d0]
// Loop Through Frames:
// The loop runs from i = 1 to i <= depth (3), so it processes frames 1, 2, and 3 (skipping frame 0, which is stack_dump itself).
// For each frame:
// Extracts the mangled name (e.g., _Z9function_0v for frame 1).
// Demangles it using abi::__cxa_demangle (e.g., _Z9function_0v becomes function_0()).
// Prints the demangled name to std::cout.
// Output: Assuming successful demangling, the output will be:
// text

// Collapse

// Wrap

// Copy
// function_0()
// function_1()
// main
// Frame 1: function_0 (the caller of stack_dump).
// Frame 2: function_1 (the caller of function_0).
// Frame 3: main (the caller of function_1).
// Clean Up:
// free(names) releases the memory allocated by backtrace_symbols.
// Detailed Explanation of Key Concepts
// What is a Call Stack?
// The ca