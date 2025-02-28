#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

class Logger {
    private:
        std::fstream log_file;
        mutable std::mutex file_mutex;
    public:
        explicit Logger(const std::string& filename) 
            : log_file(filename, std::ios::in | std::ios::out | std::ios::app) {
                if(!log_file.is_open()) 
                    throw std::runtime_error("Error open log file");
            }
        ~Logger() {
            log_file.close();
        }
        void writeLog(const std::string& message) {
            std::unique_lock<std::mutex> lock(file_mutex);
            log_file << message << std::endl;
            if(!log_file)
                throw std::runtime_error("Error write in log file");
            file_mutex.unlock();
        }
        std::string readLog() {
            std::unique_lock<std::mutex> lock(file_mutex);
            std::string line;
            if(std::getline(log_file, line)) return line;
            else {
                file_mutex.unlock();
                log_file.clear();
                log_file.seekg(0, std::ios::beg);
                return "";
            }
        }
};