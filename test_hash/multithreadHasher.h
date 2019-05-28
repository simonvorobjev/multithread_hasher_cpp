#pragma once
#include <future>
#include <mutex>
#include <fstream>

class multithreadHasher
{
private:
	int _numberOfThreads;
	int _currThread;
	std::vector<bool> _isDone;
	std::vector<std::thread> _threads;
	std::mutex _mtx;
	int _result;
	std::ifstream _inputFileStream;
	std::ofstream _outputFileStream;
	int _chunkSize;
public:
	multithreadHasher(int numberOfThreads, std::string inputFilePath, std::string outputFilePath, int chunkSize);
	~multithreadHasher();
	void CalculateHashThread(std::string buffer, int num_of_thread);
	size_t CalculateHash();
};

