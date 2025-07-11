// MemoryEater.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <signal.h>
#include <format>
#include <thread>
using namespace std;
using std::cout;

bool stop = false;
vector<char*> memoryChunks;
constexpr size_t oneTimeMem = 32 * 1048576; // 32 MiB

static void memoryUser() {
	// This function is to fill memory with some data to ensure it's committed
	char myChar = 0;
	auto myMemoryBuffer = make_unique<char[]>(oneTimeMem * 2);
	for (size_t i = 0; i < oneTimeMem * 2; ++i) {
		myMemoryBuffer[i] = myChar;
		++myChar;
	}
	while (!stop) {
		for (auto i : memoryChunks) {
			if (stop) return; // Check if we need to stop before writing memory
			size_t written = 0;
			if (!WriteProcessMemory(GetCurrentProcess(), i, myMemoryBuffer.get(), oneTimeMem, &written)) {
				cout << "\nFailed to write memory at " << i << "!" << endl;
				Sleep(500);
				break; // 跳出内层循环。因为 memoryChunks 可能已经更改（多线程），所以需要重新检查。
			}
		}
	}
}

int main()
{
    std::cout << "I'm going to eat memory!!" << endl << endl;

	signal(SIGINT, [](int) {
		cout << "\n\nMemory eating interrupted!" << endl;
		stop = true;
	});
	thread userThread(memoryUser);

	size_t ateMemorys = 0;
	char myChar = 0;
	size_t written = 0;
	auto myMemoryBuffer = make_unique<char[]>(oneTimeMem);	
	// Initialize the memory buffer to ensure it's committed
	for (size_t i = 0; i < oneTimeMem; ++i) {
		myMemoryBuffer[i] = myChar;
		++myChar;
	}
	while (!stop) {
		char* memory = (char*)VirtualAlloc(NULL, oneTimeMem, MEM_COMMIT, PAGE_READWRITE);
		if (!memory) {
			cout << "\nI can't eat more memory!" << endl;
			break;
		}
		memoryChunks.push_back(memory);
		ateMemorys += oneTimeMem;
		printf("%s", format("I have eaten {} MiB (aka. {} GiB) of memory so far! Filling...\b\b\b\b\b\b\b\b\b\b",
			size_t((ateMemorys) / 1048576), size_t((ateMemorys) / 1073741824)).c_str());
		fflush(stdout);
		// Fill the memory with some data to ensure it's committed
		WriteProcessMemory(GetCurrentProcess(), memory, myMemoryBuffer.get(), oneTimeMem, &written);
		printf("          \r");
		fflush(stdout);
		Sleep(5); // Sleep for a second to simulate time taken to eat memory
	}
	cout << endl << "Press any key to continue..." << endl;
	(void)_getch();
	stop = true; // Stop the memory user thread
	if (userThread.joinable()) userThread.join(); // Wait for the memory user thread to finis
	cout << endl << "Freeing memory..." << endl;
	for (auto chunk : memoryChunks) {
		VirtualFree(chunk, 0, MEM_RELEASE);
	}
	// Clear the memoryChunks vector
	memoryChunks.clear();
	// end
	cout << "Memory freed!" << endl;
	cout << endl << "Press any key to exit..." << endl;
	return 0;
}
