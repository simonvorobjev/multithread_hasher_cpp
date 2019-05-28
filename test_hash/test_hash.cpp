#include <iostream>
#include "fileHasher.h"

int main(int argc, char* argv[])
{
	if (argc != 4) {
		std::cout << "Parameters should be: test_hash.exe <input file> <output file> <block size>" << '\n';
		return 1;
	}
	if (!IsNumeric(argv[3])) {
		std::cout << "Parameter 3 should be integer <block size>" << '\n';
		return 1;
	}
	std::string input_file = argv[1];
	std::string output_file = argv[2];
	int block_size = atoi(argv[3]);

	fileHasher file_hasher(input_file, output_file, block_size);
	try {
		size_t result = file_hasher.CalculateHashMultithreadWithClass();
		std::cout << std::hex << result << '\n';
	}
	catch (const std::invalid_argument& e) {
		std::cout << "Bad argument: " << e.what() << '\n';
	}

	return 0;
}
