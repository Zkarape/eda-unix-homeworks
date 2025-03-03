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