#include "LinksModule.h"
#include "Logger.h"
#include "FileManager.h"
#include <algorithm>

namespace BookProcessor {

LinksModule::LinksModule() {
    LOG_INFO(L"LinksModule", L"Модуль обработки ссылок инициализирован");
}

ProcessResult LinksModule::processText(wstring& text, const LinkOptions& options) {
    ProcessResult result;
    result.success = true;
    last_stats_ = LinkStats();

    LOG_INFO(L"LinksModule", L"Начало обработки ссылок");

    bool processing_enabled = false;
    
    // Ищем <head>, после которого начинаем обработку
    if (options.process_after_head) {
        size_t head_pos = text.find(L"<head>");
        if (head_pos != wstring::npos) {
            processing_enabled = true;
        }
    } else {
        processing_enabled = true;
    }

    if (!processing_enabled && options.process_after_head) {
        result.message = L"Тег <head> не найден, обработка пропущена";
        LOG_WARNING(L"LinksModule", result.message);
        return result;
    }

    size_t pos = 0;
    while ((pos = findNextProtocol(text, pos, options)) != wstring::npos) {
        last_stats_.total_found++;

        // Проверяем, не внутри ли мы уже ссылки
        if (options.skip_existing_links && isInsideLink(text, pos)) {
            last_stats_.skipped++;
            pos++;
            continue;
        }

        // Извлекаем URL
        wstring url = extractLink(text, pos, options);
        
        if (url.empty()) {
            last_stats_.errors++;
            pos++;
            continue;
        }

        // Валидация URL
        if (!isValidUrl(url)) {
            last_stats_.skipped++;
            pos += url.length();
            continue;
        }

        // Создаём кликабельную ссылку
        wstring clickable = makeClickable(url, options.css_class);
        
        // Заменяем в тексте
        text.replace(pos, url.length(), clickable);
        
        last_stats_.converted++;
        result.operations_count++;
        
        // Сдвигаемся после вставленной ссылки
        pos += clickable.length();
    }

    result.message = L"Обработано ссылок: " + std::to_wstring(last_stats_.converted) +
                     L" из " + std::to_wstring(last_stats_.total_found);
    
    result.details.push_back(L"Найдено: " + std::to_wstring(last_stats_.total_found));
    result.details.push_back(L"Преобразовано: " + std::to_wstring(last_stats_.converted));
    result.details.push_back(L"Пропущено: " + std::to_wstring(last_stats_.skipped));
    result.details.push_back(L"Ошибок: " + std::to_wstring(last_stats_.errors));

    LOG_INFO(L"LinksModule", result.message);

    return result;
}

ProcessResult LinksModule::processFile(const wstring& input_path,
                                       const wstring& output_path,
                                       const LinkOptions& options) {
    ProcessResult result;
    
    FileManager fm;
    wstring content;
    
    // Читаем файл
    if (!fm.readFile(input_path, content)) {
        result.success = false;
        result.message = L"Не удалось прочитать входной файл";
        LOG_ERROR(L"LinksModule", result.message);
        return result;
    }
    
    // Обрабатываем
    result = processText(content, options);
    
    // Записываем
    if (result.success && fm.writeFile(output_path, content)) {
        result.message += L"\nФайл сохранён: " + output_path;
    } else {
        result.success = false;
        result.message = L"Ошибка записи файла";
        LOG_ERROR(L"LinksModule", result.message);
    }
    
    return result;
}

wstring LinksModule::extractLink(const wstring& text, size_t start_pos, const LinkOptions& options) {
    wstring link;
    
    for (size_t i = start_pos; i < text.length(); ++i) {
        wchar_t ch = text[i];
        
        // Проверяем терминаторы
        if (isTerminator(ch, options)) {
            // Специальный случай: точка перед '<'
            if (ch == L'.' && i + 1 < text.length() && text[i + 1] == L'<') {
                break;
            } else if (ch == L'.') {
                // Обычная точка - проверяем контекст
                if (i + 1 < text.length() && !std::isalnum(text[i + 1])) {
                    break; // Точка в конце предложения
                }
            } else {
                break;
            }
        }
        
        link += ch;
    }
    
    // Удаляем завершающую точку, если она есть
    if (!link.empty() && link.back() == L'.') {
        link.pop_back();
    }
    
    return link;
}

bool LinksModule::isTerminator(wchar_t ch, const LinkOptions& options) {
    return options.terminators.find(ch) != options.terminators.end();
}

bool LinksModule::isValidUrl(const wstring& url) {
    // Простая валидация
    if (url.length() < 10) return false; // Минимальная длина URL
    
    // Должен начинаться с протокола
    if (url.find(L"://") == wstring::npos) return false;
    
    // Не должен содержать пробелов
    if (url.find(L' ') != wstring::npos) return false;
    
    return true;
}

bool LinksModule::isInsideLink(const wstring& text, size_t pos) {
    // Ищем ближайший открывающий и закрывающий тег <a>
    size_t open_tag = text.rfind(L"<a ", pos);
    size_t close_tag = text.rfind(L"</a>", pos);
    
    // Если открывающий тег ближе, чем закрывающий, мы внутри ссылки
    if (open_tag != wstring::npos) {
        if (close_tag == wstring::npos || open_tag > close_tag) {
            return true;
        }
    }
    
    // Также проверяем, не внутри ли мы href=""
    size_t href_pos = text.rfind(L"href=\"", pos);
    if (href_pos != wstring::npos) {
        size_t quote_close = text.find(L"\"", href_pos + 6);
        if (quote_close != wstring::npos && pos < quote_close) {
            return true;
        }
    }
    
    return false;
}

size_t LinksModule::findNextProtocol(const wstring& text, size_t start, const LinkOptions& options) {
    size_t nearest = wstring::npos;
    
    for (const auto& protocol : options.protocols) {
        size_t pos = text.find(protocol, start);
        if (pos != wstring::npos && (nearest == wstring::npos || pos < nearest)) {
            nearest = pos;
        }
    }
    
    return nearest;
}

wstring LinksModule::makeClickable(const wstring& url, const wstring& css_class) {
    wstring result = L"<a";
    
    if (!css_class.empty()) {
        result += L" class=\"" + css_class + L"\"";
    }
    
    result += L" href=\"" + url + L"\">" + url + L"</a>";
    
    return result;
}

} // namespace BookProcessor
