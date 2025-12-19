#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <codecvt>
#include <locale>

namespace BookProcessor {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::log(LogLevel level, const wstring& module, const wstring& message) {
    if (level < min_level_) return;

    std::lock_guard<std::mutex> lock(mutex_);

    LogEntry entry;
    entry.level = level;
    entry.module = module;
    entry.message = message;
    entry.timestamp = getCurrentTimestamp();

    entries_.push_back(entry);

    // Формируем строку лога
    std::wstringstream ss;
    ss << L"[" << entry.timestamp << L"] "
       << L"[" << levelToString(level) << L"] "
       << L"[" << module << L"] "
       << message;

    // Запись в файл
    if (log_file_.is_open()) {
        log_file_ << ss.str() << std::endl;
        log_file_.flush();
    }

    // Эмитируем сигнал для GUI
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    emit logAdded(QString::fromStdWString(ss.str()));
}

void Logger::setLogFile(const wstring& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (log_file_.is_open()) {
        log_file_.close();
    }

    log_file_.open(path.c_str(), std::ios::out | std::ios::app);
    log_file_.imbue(std::locale(log_file_.getloc(), new std::codecvt_utf8<wchar_t>));
}

void Logger::setMinLevel(LogLevel level) {
    min_level_ = level;
}

std::vector<LogEntry> Logger::getEntries() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return entries_;
}

std::vector<LogEntry> Logger::filterByLevel(LogLevel min_level) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<LogEntry> filtered;
    
    for (const auto& entry : entries_) {
        if (entry.level >= min_level) {
            filtered.push_back(entry);
        }
    }
    
    return filtered;
}

std::vector<LogEntry> Logger::filterByModule(const wstring& module) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<LogEntry> filtered;
    
    for (const auto& entry : entries_) {
        if (entry.module == module) {
            filtered.push_back(entry);
        }
    }
    
    return filtered;
}

void Logger::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.clear();
}

bool Logger::exportLog(const wstring& path) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::wofstream file(path.c_str());
    if (!file.is_open()) return false;
    
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>));
    
    for (const auto& entry : entries_) {
        file << L"[" << entry.timestamp << L"] "
             << L"[" << levelToString(entry.level) << L"] "
             << L"[" << entry.module << L"] "
             << entry.message << std::endl;
    }
    
    file.close();
    return true;
}

wstring Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm_buf;
    #ifdef _WIN32
        localtime_s(&tm_buf, &time_t_now);
    #else
        localtime_r(&time_t_now, &tm_buf);
    #endif
    
    std::wstringstream ss;
    ss << std::put_time(&tm_buf, L"%Y-%m-%d %H:%M:%S");
    return ss.str();
}

wstring Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG: return L"DEBUG";
        case LogLevel::INFO: return L"INFO";
        case LogLevel::WARNING: return L"WARN";
        case LogLevel::ERROR: return L"ERROR";
        case LogLevel::CRITICAL: return L"CRITICAL";
        default: return L"UNKNOWN";
    }
}

} // namespace BookProcessor
