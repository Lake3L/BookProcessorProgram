#include "ExtractorModule.h"
#include "Logger.h"
#include "FileManager.h"

namespace BookProcessor {

ExtractorModule::ExtractorModule() {
    LOG_INFO(L"ExtractorModule", L"Модуль извлечения инициализирован");
}

ProcessResult ExtractorModule::processText(wstring& text, const ExtractOptions& options) {
    ProcessResult result;
    result.success = true;
    
    // Список должностей для поиска
    std::vector<wstring> job_titles = {
        L"Главный редактор",
        L"Редактор",
        L"Корректор",
        L"Верстальщик",
        L"Дизайнер",
        L"Фотограф",
        L"Иллюстратор",
        L"Автор",
        L"Переводчик"
    };
    
    wstring extracted_content = L"<html><head><meta charset=\"utf-8\"></head><body>\n";
    int found_count = 0;
    
    // Ищем каждую должность
    for (const auto& title : job_titles) {
        size_t pos = 0;
        while ((pos = text.find(title, pos)) != wstring::npos) {
            // Ищем начало абзаца
            size_t para_start = text.rfind(L"<p", pos);
            if (para_start == wstring::npos) {
                pos++;
                continue;
            }
            
            // Ищем конец абзаца
            size_t para_end = text.find(L"</p>", pos);
            if (para_end == wstring::npos) {
                pos++;
                continue;
            }
            
            // Извлекаем абзац целиком
            wstring paragraph = text.substr(para_start, para_end - para_start + 4);
            extracted_content += paragraph + L"\n";
            found_count++;
            
            pos = para_end;
        }
    }
    
    extracted_content += L"</body></html>";
    
    if (found_count > 0) {
        text = extracted_content;
        result.operations_count = found_count;
        result.message = L"Извлечено абзацев с должностями: " + std::to_wstring(found_count);
        LOG_INFO(L"ExtractorModule", result.message);
    } else {
        result.success = false;
        result.message = L"Не найдено абзацев с должностями";
        LOG_WARNING(L"ExtractorModule", result.message);
    }
    
    return result;
}

ProcessResult ExtractorModule::processFile(const wstring& input_path,
                                          const wstring& output_path,
                                          const ExtractOptions& options) {
    FileManager fm;
    wstring content;
    
    ProcessResult result;
    
    if (!fm.readFile(input_path, content)) {
        result.success = false;
        result.message = L"Ошибка чтения файла";
        return result;
    }
    
    result = processText(content, options);
    
    if (result.success && !fm.writeFile(output_path, content)) {
        result.success = false;
        result.message = L"Ошибка записи файла";
    }
    
    return result;
}

std::pair<size_t, size_t> ExtractorModule::detectContentBounds(const wstring& text, 
                                                                const ExtractOptions& options) {
    size_t start = wstring::npos;
    size_t end = wstring::npos;
    
    // Поиск начала основного текста
    for (const auto& marker : options.start_markers) {
        size_t pos = text.find(marker);
        if (pos != wstring::npos) {
            // Ищем конец строки/абзаца после маркера
            size_t line_end = text.find(L"</p>", pos);
            if (line_end != wstring::npos) {
                start = line_end + 4;
                break;
            }
        }
    }
    
    // Поиск конца основного текста (начало должностей)
    for (const auto& marker : options.end_markers) {
        size_t pos = text.find(marker);
        if (pos != wstring::npos) {
            // Ищем начало абзаца с маркером
            size_t para_start = text.rfind(L"<p", pos);
            if (para_start != wstring::npos) {
                end = para_start;
                break;
            }
        }
    }
    
    return {start, end};
}

bool ExtractorModule::validateExtraction(const wstring& extracted, const wstring& original) {
    // Проверка: извлечённый текст не должен быть слишком маленьким
    if (extracted.length() < original.length() * 0.01) { // Менее 1%
        return false;
    }
    
    // Проверка: должна сохраниться структура HTML
    if (extracted.find(L"<html") == wstring::npos && original.find(L"<html") != wstring::npos) {
        return false;
    }
    
    return true;
}

} // namespace BookProcessor
