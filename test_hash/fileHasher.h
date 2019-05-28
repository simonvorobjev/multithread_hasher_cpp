#pragma once
#include <string>
class fileHasher
{
private:
	std::string _filePathIn, _filePathOut;
	int _chunkSize;
public:
	fileHasher(std::string filePathIn, std::string filePathOut, int chunkSize);
	~fileHasher();
	size_t CalculateHash();
	size_t CalculateHashMultithread();
	size_t CalculateHashMultithreadWithClass();
};

