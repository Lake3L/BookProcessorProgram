#ifndef LOGGER_H
#define LOGGER_H

#include "Types.h"
#include <fstream>
#include <mutex>
#include <QObject>

namespace BookProcessor {

class Logger : public QObject {
    Q_OBJECT

public:
    static Logger& instance();
    
    void log(LogLevel level, const wstring& module, const wstring& message);
    void setLogFile(const wstring& path);
    void setMinLevel(LogLevel level);
    
    std::vector<LogEntry> getEntries() const;
    std::vector<LogEntry> filterByLevel(LogLevel min_level) const;
    std::vector<LogEntry> filterByModule(const wstring& module) const;
    
    void clear();
    bool exportLog(const wstring& path) const;

signals:
    void logAdded(const QString& message);

private:
    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    wstring getCurrentTimestamp() const;
    wstring levelToString(LogLevel level) const;

    std::vector<LogEntry> entries_;
    std::wofstream log_file_;
    LogLevel min_level_ = LogLevel::INFO;
    mutable std::mutex mutex_;
};

// Удобные макросы для логирования
#define LOG_DEBUG(module, msg) Logger::instance().log(LogLevel::DEBUG, module, msg)
#define LOG_INFO(module, msg) Logger::instance().log(LogLevel::INFO, module, msg)
#define LOG_WARNING(module, msg) Logger::instance().log(LogLevel::WARNING, module, msg)
#define LOG_ERROR(module, msg) Logger::instance().log(LogLevel::ERROR, module, msg)
#define LOG_CRITICAL(module, msg) Logger::instance().log(LogLevel::CRITICAL, module, msg)

} // namespace BookProcessor

#endif // LOGGER_H
