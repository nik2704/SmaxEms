#include "ConsoleSpinner.h"

std::mutex ConsoleSpinner::cout_mutex_;

ConsoleSpinner::ConsoleSpinner(const std::string& operation)
    : stop_flag_(false), operation_(operation) {
    {
        std::lock_guard<std::mutex> lock(cout_mutex_);
        std::cout << operation_ << "..." << std::flush;
    }
    spinner_thread_ = std::thread(&ConsoleSpinner::run, this);
}

ConsoleSpinner::~ConsoleSpinner() {
    stop_flag_ = true;
    if (spinner_thread_.joinable()) {
        spinner_thread_.join();
    }
    {
        std::lock_guard<std::mutex> lock(cout_mutex_);
        std::cout << " " << status_ << std::endl;
    }
}

void ConsoleSpinner::setStatus(const std::string& status) {
    status_ = status;
}

void ConsoleSpinner::run() {
    while (!stop_flag_) {
        {
            std::lock_guard<std::mutex> lock(cout_mutex_);
            std::cout << "." << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
