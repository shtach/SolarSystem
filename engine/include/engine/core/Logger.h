#pragma once

#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error
};

class Logger {
public:
    static Logger& get();

    void setLevel(LogLevel minLevel);
    void enableFileOutput(const std::string& file_path);

    void log(LogLevel level, const std::string& category, const std::string& message);

private:
    static const char* levelName(LogLevel level);
    static const char* levelColor(LogLevel level);
    static std::string timestamp();


    Logger() = default;

    LogLevel      m_minLevel {LogLevel::Info};
    std::ofstream m_file;
    std::mutex    m_mutex;
};

#define LOG_TRACE(cat, msg) Logger::get().log(LogLevel::Trace, (cat), (msg))
#define LOG_DEBUG(cat, msg) Logger::get().log(LogLevel::Debug, (cat), (msg))
#define LOG_INFO(cat, msg) Logger::get().log(LogLevel::Info, (cat), (msg))
#define LOG_WARN(cat, msg) Logger::get().log(LogLevel::Warn, (cat), (msg))
#define LOG_ERROR(cat, msg) Logger::get().log(LogLevel::Error, (cat), (msg))