#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <map>
#include <set>

namespace BookProcessor {

// Основные типы
using wstring = std::wstring;
using string = std::string;

// Уровень логирования
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Результат обработки
struct ProcessResult {
    bool success = false;
    int operations_count = 0;
    int errors_count = 0;
    wstring message;
    std::vector<wstring> details;
};

// Правило замены
struct ReplacementRule {
    wstring id;
    wstring pattern;
    wstring replacement;
    bool enabled = true;
    bool use_regex = false;
    int priority = 0;
    wstring category;
    wstring description;
    bool user_editable = true;
};

// Опции обработки
struct ProcessOptions {
    bool create_backup = true;
    bool show_diff = false;
    bool verbose_logging = true;
    wstring encoding = L"UTF-8";
};

// Запись лога
struct LogEntry {
    LogLevel level;
    wstring module;
    wstring message;
    wstring timestamp;
};

// Опции Links модуля
struct LinkOptions {
    std::set<wchar_t> terminators = {L' ', L',', L'<', L'>', L'"', L')'};
    bool case_sensitive = false;
    std::vector<wstring> protocols = {L"http://", L"https://", L"ftp://", L"mailto:"};
    wstring css_class = L"clickable-link";
    bool skip_existing_links = true;
    bool process_after_head = true;
};

// Режим Extractor
enum class ExtractorMode {
    EXTRACT_MAIN_TEXT,
    EXTRACT_METADATA
};

// Опции Extractor модуля
struct ExtractorOptions {
    ExtractorMode mode = ExtractorMode::EXTRACT_METADATA;
    std::vector<wstring> start_markers = {L"Все права защищены", L"© Издательство"};
    std::vector<wstring> end_markers = {L"Главный редактор", L"Корректор"};
};

} // namespace BookProcessor

#endif // TYPES_H
