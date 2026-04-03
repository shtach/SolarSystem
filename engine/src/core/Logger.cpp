#include "engine/core/Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <ios>
#include <iostream>
#include <locale>
#include <mutex>
#include <sstream>
#include <string>

Logger& Logger::get() { // Meyer's Singleton
    static Logger instance;
    return instance;
}

void Logger::setLevel(LogLevel minLevel) {
    m_minLevel = minLevel;
}

void Logger::enableFileOutput(const std::string& file_path) {
    m_file.open(file_path, std::ios::out | std::ios::trunc);
} // starting every time fresh log

std::string Logger::timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t        = std::chrono::system_clock::to_time_t(now);

    std::ostringstream ss;
    ss << std::put_time(std::localtime(&t), "%H:%M:%S");
    return ss.str();
}

const char* Logger::levelName(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO ";
        case LogLevel::Warn:  return "WARN ";
        case LogLevel::Error: return "ERROR";
    }
    return "?????";
}

const char* Logger::levelColor(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "\033[90m";   // dark gray
        case LogLevel::Debug: return "\033[36m";   // cyan
        case LogLevel::Info:  return "\033[32m";   // green
        case LogLevel::Warn:  return "\033[33m";   // yellow
        case LogLevel::Error: return "\033[31m";   // red
    }
    return "\033[0m";
}

void Logger::log(LogLevel level, const std::string& category, const std::string& message) {
    if (level < m_minLevel)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);

    std::ostringstream line;
    line    << levelColor(level)
            << "[" << timestamp() << "] "
            << "[" << levelName(level) << "] "
            << "[" << std::left << std::setw(10) << category << "] "
            << message << "\033[0m";

    std::cout << line.str() << '\n';

    if (m_file.is_open())
        m_file  << "[" << timestamp() << "] "
                << "[" << levelName(level) << "] "
                << "[" << std::left << std::setw(10) << category << "] "
                << message << '\n';
}