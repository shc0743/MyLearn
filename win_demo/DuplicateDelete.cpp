//: DuplicateDelete.cpp
// Demonstration of C++ duplicate delete.
// {ThrowsException}
#include <Windows.h>
int main() {
    int* ptr = new int;
    delete ptr;
    delete ptr;
    return 0;
} /* Returns: -1073740940 (NTSTATUS 0xC0000374: 堆已损坏。 ) *///:~