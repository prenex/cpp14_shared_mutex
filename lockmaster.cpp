// Compilation on linux: g++ lockmaster.cpp -o lockmaster.out -std=c++14 -pthread

#include <iostream>
#include <mutex>  // For std::unique_lock
#include <shared_mutex>
#include <thread>
#include <unistd.h>

// Do a lot of arcane database magic according to the opcode
void lotOfOperations(int opCode) {
	// TODO: implement all the real things when I figure out possible threading issues
	sleep(opCode);
}

int main() {
	std::shared_timed_mutex rwMutex;
	std::mutex normalMutex;

	auto t1 = [&rwMutex, &normalMutex] () {
		// Acquire readlock on rwMutex
		std::shared_lock<std::shared_timed_mutex> rLock(rwMutex);
		// Do arcane magic no4 with read mutex
		lotOfOperations(4);
		// Acuire lock on normalMutex
		std::lock_guard<std::mutex> lock(normalMutex);
		// Do arcane magic no3 with normal mutex
		lotOfOperations(3);
	};

	auto t2 = [&rwMutex, &normalMutex] () {
		// Acuire lock on normalMutex
		std::lock_guard<std::mutex> lock(normalMutex);
		// Do arcane magic no3 with normal mutex
		lotOfOperations(3);
		// Acquire readlock on rwMutex
		std::shared_lock<std::shared_timed_mutex> rLock(rwMutex);
		// Do arcane magic no4 with read mutex
		lotOfOperations(4);
	};

	// Start a thread to run the code in t1
	std::thread thread1(t1);
	// Start a thread to run the code in t2
	std::thread thread2(t2);

	// do arcane magic no1
	lotOfOperations(1);

	// Aquire writelock as it is needed for safety of lotOfOperations(5)
	std::unique_lock<std::shared_timed_mutex> wLock(rwMutex);

	// Do the most arcane stuff ever
	lotOfOperations(5);

	// Wait for the threads to finish
	thread1.join();
	thread2.join();

	// Exit
	return 0;
}
