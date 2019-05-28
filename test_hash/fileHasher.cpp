#include "fileHasher.h"
#include "multithreadHasher.h"
#include <fstream>
#include <stdexcept>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <iostream>

const int NUM_THREAD = 4;
std::atomic<bool> is_done[NUM_THREAD] = { true, true, true, true };
std::mutex mtx;

fileHasher::fileHasher(std::string filePathIn, std::string filePathOut, int chunkSize)
{
	_filePathIn = filePathIn;
	_filePathOut = filePathOut;
	_chunkSize = chunkSize;
}

fileHasher::~fileHasher()
{
}

size_t fileHasher::CalculateHash()
{
	std::hash<std::string> hash_fn;
	std::ifstream fileStream;
	fileStream.open(_filePathIn);
	if (fileStream.is_open()) {
		fileStream.seekg(0, fileStream.end);
		auto length = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg);
		std::string buffer(_chunkSize, ' ');
		size_t result = 0;
		for (auto i = 0; i < length; i = i + _chunkSize) {
			fileStream.read(&buffer[0], _chunkSize);
			result ^= hash_fn(buffer);
		}
		return result;
	}
	else {
		throw std::invalid_argument("received bad input file.");
	}
}

void CalculateHashThread(size_t &result, std::string buffer, int num_of_thread) {
	std::hash<std::string> hash_fn;
	auto p = hash_fn(buffer);
	mtx.lock();
	result ^= p;
	mtx.unlock();
	is_done[num_of_thread] = true;
}

size_t fileHasher::CalculateHashMultithread()
{
	std::ifstream fileStream;
	fileStream.open(_filePathIn);
	if (fileStream.is_open()) {
		fileStream.seekg(0, fileStream.end);
		auto length = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg);
		std::string buffer(_chunkSize, ' ');
		size_t result = 0;
		std::thread threads[NUM_THREAD];
		int curr_thread = 0;
		for (auto i = 0; i < length; i = i + _chunkSize) {
			fileStream.read(&buffer[0], _chunkSize);
			while (true) {
				curr_thread++;
				if (curr_thread == 4) {
					curr_thread = 0;
				}
				if (is_done[curr_thread]) {
					if (threads[curr_thread].joinable()) {
						threads[curr_thread].join();
					}
					is_done[curr_thread] = false;
					threads[curr_thread] = std::thread(CalculateHashThread, std::ref(result), buffer, curr_thread);
					break;
				}
			}
		}
		for (auto i = 0; i < NUM_THREAD; i++) {
			if (threads[i].joinable()) {
				threads[i].join();
			}
		}
		return result;
	}
	else {
		throw std::invalid_argument("received bad input file.");
	}
}


size_t fileHasher::CalculateHashMultithreadWithClass() {
	multithreadHasher multithreadHasherInstance(NUM_THREAD, _filePathIn, _filePathOut, _chunkSize);
	return multithreadHasherInstance.CalculateHash();
}
