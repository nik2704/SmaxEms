#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

class ConsoleSpinner {
public:
    ConsoleSpinner(const std::string& operation)
        : stop_flag_(false), operation_(operation) {
        std::cout << operation_ << "..." << std::flush;
        spinner_thread_ = std::thread(&ConsoleSpinner::run, this);
    }

    ~ConsoleSpinner() {
        stop_flag_ = true;
        if (spinner_thread_.joinable()) {
            spinner_thread_.join();
        }

        std::cout << status_ << std::endl;
    }

    void setStatus(const std::string& status) {
        status_ = status;
    }

private:
    std::atomic<bool> stop_flag_;
    std::thread spinner_thread_;
    std::string operation_;
    std::string status_;

    void run() {
        while (!stop_flag_) {
            std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
};
