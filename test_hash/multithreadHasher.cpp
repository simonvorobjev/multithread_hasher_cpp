#include "multithreadHasher.h"
#include <string>

multithreadHasher::multithreadHasher(int numberOfThreads, std::string inputFilePath, std::string outputFilePath, int chunkSize) :
	_numberOfThreads(numberOfThreads), _currThread(0), _isDone(numberOfThreads, true),
	_result(0), _chunkSize(chunkSize), _threads(numberOfThreads) {
	_inputFileStream.open(inputFilePath);
	_outputFileStream.open(outputFilePath);
}

multithreadHasher::~multithreadHasher()
{
	_inputFileStream.close();
	_outputFileStream.close();
}

void multithreadHasher::CalculateHashThread(std::string buffer, int num_of_thread) {
	std::hash<std::string> hash_fn;
	auto p = hash_fn(buffer);
	_mtx.lock();
	_result ^= p;
	_mtx.unlock();
	_isDone[num_of_thread] = true;
}

size_t multithreadHasher::CalculateHash()
{
	if (!_outputFileStream) {
		throw std::invalid_argument("cannot create output file.");
	}
	if (_inputFileStream.is_open()) {
		_inputFileStream.seekg(0, _inputFileStream.end);
		auto length = _inputFileStream.tellg();
		_inputFileStream.seekg(0, _inputFileStream.beg);
		std::string buffer(_chunkSize, ' ');
		for (auto i = 0; i < length; i = i + _chunkSize) {
			_inputFileStream.read(&buffer[0], _chunkSize);
			while (true) {
				_currThread++;
				if (_currThread == 4) {
					_currThread = 0;
				}
				if (_isDone[_currThread]) {
					if (_threads[_currThread].joinable()) {
						_threads[_currThread].join();
					}
					_isDone[_currThread] = false;
					_threads[_currThread] = std::thread(&multithreadHasher::CalculateHashThread, this, buffer, _currThread);
					break;
				}
			}
		}
		for (auto i = 0; i < _numberOfThreads; i++) {
			if (_threads[i].joinable()) {
				_threads[i].join();
			}
		}
		_outputFileStream << std::hex << _result;
		return _result;
	}
	else {
		throw std::invalid_argument("received bad input file.");
	}
}
