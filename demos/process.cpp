#include <libfilezilla/process.hpp>

#include <iostream>
#include <string>

/// \file
/// \brief A simple demonstration of using fz::process

int main()
{
	fz::process p;
	if (!p.spawn(fzT("timer_fizzbuzz"))) {
		std::cerr << "Could not spawn process" << std::endl;
		return 1;
	}
	std::cout << "Spawned process" << std::endl;

	// Send a line to the process
	std::string cmd = "6\n";
	if (!p.write(cmd.c_str(), cmd.size())) {
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
	cmd = "0\n";
	if (!p.write(cmd.c_str(), cmd.size())) {
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
