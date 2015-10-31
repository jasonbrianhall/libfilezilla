#include <libfilezilla/process.hpp>

#include <iostream>
#include <string>

namespace {
// Helper function to extract a directory from argv[0] so that the
// demos can be run independent of the current working directory.
fz::native_string get_program_dir(int argc, char ** argv)
{
	std::string path;
	if (argc > 0) {
		path = argv[0];
#ifdef FZ_WINDOWS
		auto delim = path.find_last_of("/\\");
#else
		auto delim = path.find_last_of("/");
#endif
		if (delim == std::string::npos) {
			path.clear();
		}
		else {
			path = path.substr(0, delim + 1);
		}
	}

	return fz::to_native(path);
}

#ifdef FZ_WINDOWS
auto suffix = fzT(".exe");
#else
auto suffix = fzT("");
#endif
}

int main(int argc, char *argv[])
{
	// Start the timer_fizzbuzz demo which should be in the same directory as the process demo
	fz::process p;
	if (!p.spawn(get_program_dir(argc, argv) + fzT("timer_fizzbuzz") + suffix)) {
		std::cerr << "Could not spawn process" << std::endl;
		return 1;
	}
	std::cout << "Spawned process" << std::endl;

	// Send a line to the process
	if (!p.write("6\n")) {
		std::cerr << "Sending data to the process failed. Looks like it could not be started or has quit early." << std::endl;
		return 1;
	}

	std::cout << "Waiting on process to print woof..." << std::endl;


	// We don't want to do anything else while waiting, so we don't need a thread to receive data asynchronously
	std::string input;
	bool done = false;
	while (!done) {
		char buf[100];
		int r = p.read(buf, 100);
		if (!r) {
			std::cerr << "Unexpected EOF from process" << std::endl;
			return 1;
		}
		else if (r < 0) {
			std::cerr << "Could not read from process" << std::endl;
			return 1;
		}

		input += std::string(buf, r);

		// Extract complete lines from the input
		auto delim = input.find_first_of("\r\n");
		while (delim != std::string::npos) {
			std::string line = input.substr(0, delim);
			input = input.substr(delim + 1);
			delim = input.find_first_of("\r\n");

			if (!line.empty()) {
				std::cout << "Received line from process: " << line << std::endl;
				if (line == "woof") {
					done = true;
				}
			}
		}
	}

	// Send a line to the process
	if (!p.write("0\n")) {
		std::cerr << "Sending data to the process failed. Looks like it could not be started or has quit early." << std::endl;
		return 1;
	}

	std::cout << "Told process to quit." << std::endl;

	while (true) {
		char buf[100];
		int r = p.read(buf, 100);
		if (!r) {
			std::cerr << "Received the expected EOF from process" << std::endl;
			break;
		}
		else if (r < 0) {
			std::cerr << "Could not read from process" << std::endl;
			return 1;
		}
	}

	return 0;
}
