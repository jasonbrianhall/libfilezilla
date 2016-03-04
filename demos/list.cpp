#include <libfilezilla/local_filesys.hpp>

#include <iostream>
#include <string.h>

int main(int argc, char *argv[])
{
	fz::native_string path = fzT(".");

	if (argc > 1 && argv[1] && *argv[1] && strlen(argv[1]) < 1000) {
		path = fz::to_native(std::string(argv[1]));
	}

	fz::local_filesys fs;

	// Begin listing
	if (!fs.begin_find_files(path)) {
		std::cerr << "Cannot list " << fz::to_string(path) << std::endl;
		return 1;
	}

	std::cout << "Listing " << fz::to_string(path) << "\n";
	std::cout << "----------------------------------\n";

	fz::native_string name;
	int64_t size;
	fz::datetime time;
	bool is_link;
	bool is_dir;
	int mode;

	// Iterate over it
	while (fs.get_next_file(name, is_link, is_dir, &size, &time, &mode)) {

		// Print results
		std::cout << fz::to_string(name) << "\n";
		std::cout << "    Type: " << (is_link ? "symlinked" : "regular") << (is_dir ? " directory" : " file") << "\n";
		if (!is_dir) {
			if (size >= 0) {
				std::cout << "    Size: " << size << " octets" << "\n";
			}
		}
		if (!time.empty()) {
			std::cout << "    Last modified: " << time.format("%Y-%m-%d %H-%M-%S ", fz::datetime::local) << "\n";
		}
		std::cout << "    Mode: " << mode << "\n" << std::endl;
	}

	return 0;
}
