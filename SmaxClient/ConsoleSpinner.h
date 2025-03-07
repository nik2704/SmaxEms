/**
 * @file ConsoleSpinner.h
 * @brief Defines the ConsoleSpinner class for displaying a spinning indicator during long-running operations.
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

/**
 * @class ConsoleSpinner
 * @brief A simple console-based spinner to indicate ongoing operations.
 */
class ConsoleSpinner {
public:
    /**
     * @brief Constructs a ConsoleSpinner with a specified operation name.
     * @param operation The name of the operation being executed.
     */
    ConsoleSpinner(const std::string& operation);

    /**
     * @brief Destructor that stops the spinner and prints the final status.
     */
    ~ConsoleSpinner();

    /**
     * @brief Sets the final status message to display upon completion.
     * @param status The final status message.
     */
    void setStatus(const std::string& status);

private:
    std::atomic<bool> stop_flag_; ///< Flag to indicate when the spinner should stop.
    std::thread spinner_thread_;  ///< Thread running the spinner animation.
    std::string operation_;       ///< Name of the operation being performed.
    std::string status_;          ///< Final status message displayed when the spinner stops.
    static std::mutex cout_mutex_; ///< Mutex to ensure thread-safe console output.

    /**
     * @brief Runs the spinner animation until stopped.
     */
    void run();
};

