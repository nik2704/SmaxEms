#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

class ConsoleSpinner {
public:
    ConsoleSpinner(const std::string& operation)
        : stop_flag_(false), operation_(operation) {
        {
            std::lock_guard<std::mutex> lock(cout_mutex_);
            std::cout << operation_ << "..." << std::flush;
        }
        spinner_thread_ = std::thread(&ConsoleSpinner::run, this);
    }

    ~ConsoleSpinner() {
        stop_flag_ = true;
        if (spinner_thread_.joinable()) {
            spinner_thread_.join();
        }
        {
            std::lock_guard<std::mutex> lock(cout_mutex_);
            std::cout << " " << status_ << std::endl;
        }
    }

    void setStatus(const std::string& status) {
        status_ = status;
    }

private:
    std::atomic<bool> stop_flag_;
    std::thread spinner_thread_;
    std::string operation_;
    std::string status_;
    static std::mutex cout_mutex_;

    void run() {
        while (!stop_flag_) {
            {
                std::lock_guard<std::mutex> lock(cout_mutex_);
                std::cout << "." << std::flush;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
};

std::mutex ConsoleSpinner::cout_mutex_;

