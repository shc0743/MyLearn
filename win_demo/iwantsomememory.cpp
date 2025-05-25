#include <windows.h>
#include <iostream>
#include <conio.h>
using namespace std;
int main() {
    char cmd;
    cout << "IWantSomeMemory.exe -- A demo of C++ memory allocation" << endl;
    cout << "======================================================" << endl;
    char* buffer = nullptr;
    while (1) {
        cout << "\rCommands:\n- a\tAllocate memory.\n- f Free memory.\n- q Quit." << endl << "> ";
        cmd = _getch();
        _putch(cmd);
        if (cmd == 'q') {
            if (buffer) {
                cout << "Memory NOT free, the memory will be released by OS after the program is quit.\n"
                "However, **Never** use this example code in your production environment!" << endl <<
                "To develop you a good habit, we have prevented you from quitting unless you free the memory." << endl;
                continue;
            }
            break;
        }
        if (cmd != 'a' && cmd != 'f') {
            cout << "\a";
            continue;
        }
        cout << endl;
        if (cmd == 'a') {
            if (buffer) {
                cout << "Error: Memory already allocated, please free it first" << endl;
                continue;
            }
            try {
                buffer = new char[1048576 * 256];
                cout << "Successfully allocated 256MiB memory" << endl;
            }
            catch (bad_alloc&) {
                cout << "Cannot allocate. Try to retry?" << endl;
            }
        }
        if (cmd == 'f') {
            if (!buffer) {
                cout << "Error: Memory is not allocated, please allocate it first" << endl;
                continue;
            }
            delete[] buffer;
            buffer = nullptr;
            cout << "Memory has been freeed." << endl;
        }
    };
    cout << "Goodbye!" << endl;
    Sleep(1000);
    return 0;
}