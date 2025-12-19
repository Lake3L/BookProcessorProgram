#include "CleanerModule.h"
#include "Logger.h"
#include "FileManager.h"

namespace BookProcessor {

CleanerModule::CleanerModule() {
    engine_ = std::make_shared<PatternEngine>();
    initializeDefaultRules();
    LOG_INFO(L"CleanerModule", L"Модуль очистки инициализирован");
}

void CleanerModule::initializeDefaultRules() {
    // Все правила из mainwindow.cpp

    // 1. HTML-сущности
    engine_->addRule({
        L"html_nbsp", L"&#160;", L" ", true, false, 1,
        L"html_entities", L"Замена неразрывного пробела (&#160;)", true
    });
    
    engine_->addRule({
        L"html_shy", L"&#173;", L"", true, false, 1,
        L"html_entities", L"Удаление мягкого переноса (&#173;)", true
    });
    
    engine_->addRule({
        L"html_tab", L"&#9;", L"&nbsp;", true, false, 1,
        L"html_entities", L"Замена табуляции (&#9;)", true
    });

    // 2. Сокращения с точками
    engine_->addRule({
        L"abbr_td", L"т. д.", L"т.д.", true, false, 5,
        L"punctuation", L"Сокращение 'т. д.'", true
    });
    
    engine_->addRule({
        L"abbr_tp", L"т. п.", L"т.п.", true, false, 5,
        L"punctuation", L"Сокращение 'т. п.'", true
    });
    
    engine_->addRule({
        L"abbr_te", L"т. е.", L"т.е.", true, false, 5,
        L"punctuation", L"Сокращение 'т. е.'", true
    });
    
    engine_->addRule({
        L"abbr_tn", L"т. н.", L"т.н.", true, false, 5,
        L"punctuation", L"Сокращение 'т. н.'", true
    });
    
    engine_->addRule({
        L"abbr_tk", L"т. к.", L"т.к.", true, false, 5,
        L"punctuation", L"Сокращение 'т. к.'", true
    });
    
    engine_->addRule({
        L"abbr_ne", L"н. э.", L"н.э.", true, false, 5,
        L"punctuation", L"Сокращение 'н. э.'", true
    });

    // 3. CSS-классы
    engine_->addRule({
        L"class_osnova", L" class=\"Osnova\"", L"", true, false, 10,
        L"css_cleanup", L"Удаление класса Osnova", true
    });
    
    engine_->addRule({
        L"class_endnote_link", L" class=\"_idEndnoteLink\"", L"", true, false, 10,
        L"css_cleanup", L"Удаление класса _idEndnoteLink", true
    });
    
    engine_->addRule({
        L"class_footnote_link", L" class=\"_idFootnoteLink\"", L"", true, false, 10,
        L"css_cleanup", L"Удаление класса _idFootnoteLink", true
    });
    
    engine_->addRule({
        L"class_endnote_anchor", L" class=\"_idEndnoteAnchor\"", L"", true, false, 10,
        L"css_cleanup", L"Удаление класса _idEndnoteAnchor", true
    });
    
    engine_->addRule({
        L"class_footnote", L" class=\"_idFootnote\"", L"", true, false, 10,
        L"css_cleanup", L"Удаление класса _idFootnote", true
    });
    
    engine_->addRule({
        L"class_footnote_anchor", L" class=\"_idFootnoteAnchor\"", L"", true, false, 10,
        L"css_cleanup", L"Удаление класса _idFootnoteAnchor", true
    });
    
    engine_->addRule({
        L"class_osnova_minus", L" class=\"OsnovaMINUS\"", L"", true, false, 10,
        L"css_cleanup", L"Удаление класса OsnovaMINUS", true
    });
    
    engine_->addRule({
        L"class_osnova_plus", L" class=\"OsnovaPLUS\"", L"", true, false, 10,
        L"css_cleanup", L"Удаление класса OsnovaPLUS", true
    });

    // 4. HTML-теги
    engine_->addRule({
        L"tag_dash", L"<p>— ", L"<p>—&nbsp;", true, false, 15,
        L"html_tags", L"Тире с неразрывным пробелом", true
    });
    
    engine_->addRule({
        L"tag_br", L"<br />", L" ", true, false, 15,
        L"html_tags", L"Замена <br /> на пробел", true
    });

    // 5. Ссылки на сноски
    engine_->addRule({
        L"link_endnote", L"bookDW.html#endnote", L"link2.html#endnote", true, false, 20,
        L"links", L"Исправление ссылок на endnote", true
    });
    
    engine_->addRule({
        L"link_footnote", L"bookDW.html#footnote", L"link.html#endnote", true, false, 20,
        L"links", L"Исправление ссылок на footnote", true
    });

    // 6. Специальные классы
    engine_->addRule({
        L"otbf_minus", L"OTBFMINUS", L"OTBF", true, false, 25,
        L"special", L"OTBFMINUS → OTBF", true
    });
    
    engine_->addRule({
        L"otbf_plus", L"OTBFPLUS", L"OTBF", true, false, 25,
        L"special", L"OTBFPLUS → OTBF", true
    });
    
    engine_->addRule({
        L"ba_minus", L"BAMINUS", L"BA", true, false, 25,
        L"special", L"BAMINUS → BA", true
    });
    
    engine_->addRule({
        L"ba_plus", L" BAPLUS", L" BA ", true, false, 25,
        L"special", L"BAPLUS → BA", true
    });
    
    LOG_INFO(L"CleanerModule", L"Загружено " + std::to_wstring(engine_->getRules().size()) + L" правил по умолчанию");
}

void CleanerModule::loadRulesFromEngine(std::shared_ptr<PatternEngine> engine) {
    engine_ = engine;
    LOG_INFO(L"CleanerModule", L"Загружен внешний движок правил");
}

ProcessResult CleanerModule::processText(wstring& text, const ProcessOptions& options) {
    ProcessResult result;
    result.success = true;

    LOG_INFO(L"CleanerModule", L"Начало обработки текста");

    // 1. Применяем простые правила через PatternEngine
    auto engine_result = engine_->applyRules(text);
    result.operations_count += engine_result.operations_count;
    result.errors_count += engine_result.errors_count;
    result.details.insert(result.details.end(), 
                         engine_result.details.begin(), 
                         engine_result.details.end());

    // 2. Применяем сложные правила
    int complex_count = 0;
    
    processParaOverride(text, complex_count);
    if (complex_count > 0) {
        result.operations_count += complex_count;
        result.details.push_back(L"ParaOverride: " + std::to_wstring(complex_count) + L" удалений");
    }
    
    complex_count = 0;
    processTextAnchor(text, complex_count);
    if (complex_count > 0) {
        result.operations_count += complex_count;
        result.details.push_back(L"TextAnchor: " + std::to_wstring(complex_count) + L" удалений");
    }
    
    complex_count = 0;
    processIndexMarker(text, complex_count);
    if (complex_count > 0) {
        result.operations_count += complex_count;
        result.details.push_back(L"IndexMarker: " + std::to_wstring(complex_count) + L" удалений");
    }
    
    complex_count = 0;
    processSpanTags(text, complex_count);
    if (complex_count > 0) {
        result.operations_count += complex_count;
        result.details.push_back(L"Span tags: " + std::to_wstring(complex_count) + L" удалений");
    }

    result.message = L"Обработка завершена. Всего операций: " + std::to_wstring(result.operations_count);
    LOG_INFO(L"CleanerModule", result.message);

    return result;
}

ProcessResult CleanerModule::processFile(const wstring& input_path, 
                                        const wstring& output_path,
                                        const ProcessOptions& options) {
    ProcessResult result;
    
    FileManager fm;
    wstring content;
    
    // Читаем файл
    if (!fm.readFile(input_path, content)) {
        result.success = false;
        result.message = L"Не удалось прочитать входной файл";
        LOG_ERROR(L"CleanerModule", result.message);
        return result;
    }
    
    // Создаём backup
    if (options.create_backup) {
        fm.createBackup(input_path);
    }
    
    // Обрабатываем
    result = processText(content, options);
    
    // Записываем
    if (result.success && fm.writeFile(output_path, content)) {
        result.message += L"\nФайл сохранён: " + output_path;
    } else {
        result.success = false;
        result.message = L"Ошибка записи файла";
        LOG_ERROR(L"CleanerModule", result.message);
    }
    
    return result;
}

// Сложные правила из mainwindow.cpp

void CleanerModule::processParaOverride(wstring& text, int& count) {
    // Удаление динамических классов ParaOverride-XXX
    while (text.find(L" ParaOverride-") != wstring::npos) {
        wstring nom = L" ParaOverride-";
        size_t poss = text.find(L" ParaOverride-");
        
        // Читаем до закрывающей кавычки
        for (size_t i = poss + nom.length(); i < poss + 30 && i < text.length() && text[i] != L'\"'; ++i) {
            nom += text[i];
        }
        
        // Заменяем
        size_t pos = text.find(nom);
        if (pos != wstring::npos) {
            text.replace(pos, nom.length(), L"");
            count++;
        } else {
            break; // Защита от бесконечного цикла
        }
    }
}

void CleanerModule::processTextAnchor(wstring& text, int& count) {
    // Удаление <a id="_idTextAnchor...">...</a>
    while (text.find(L"<a id=\"_idTextAnchor") != wstring::npos) {
        wstring nom = L"<a id=\"_idTextAnchor";
        size_t poss = text.find(nom);
        size_t i = poss + nom.length();
        
        // Читаем до закрывающего тега
        while (i < text.length()) {
            nom += text[i];
            if (text[i] == L'>' && (i + 1 >= text.length() || text[i + 1] != L'<')) {
                break;
            }
            ++i;
        }
        
        // Заменяем
        size_t pos = text.find(nom);
        if (pos != wstring::npos && pos == poss) {
            text.replace(pos, nom.length(), L"");
            count++;
        } else {
            break;
        }
    }
}

void CleanerModule::processIndexMarker(wstring& text, int& count) {
    // Удаление <a id="_idIndexMarker...">...</a>
    while (text.find(L"<a id=\"_idIndexMarker") != wstring::npos) {
        wstring nom = L"<a id=\"_idIndexMarker";
        size_t poss = text.find(nom);
        size_t i = poss + nom.length();
        
        // Читаем до нужного закрывающего тега
        while (i < text.length()) {
            nom += text[i];
            
            if (text[i] == L'>' && i + 1 < text.length()) {
                if (text[i + 1] != L'<') {
                    break;
                }
                if (i + 3 < text.length() && text[i + 1] == L'<' && text[i + 2] == L'/' && text[i + 3] != L'a') {
                    break;
                }
            }
            ++i;
        }
        
        // Заменяем
        size_t pos = text.find(nom);
        if (pos != wstring::npos && pos == poss) {
            text.replace(pos, nom.length(), L"");
            count++;
        } else {
            break;
        }
    }
}

void CleanerModule::processSpanTags(wstring& text, int& count) {
    // 1. Удаление простых <span></span>
    while (text.find(L"<span>") != wstring::npos) {
        size_t poss = text.find(L"<span>");
        size_t poss2 = text.find(L"</span>", poss);
        
        if (poss2 != wstring::npos) {
            text.replace(poss2, 7, L"");  // </span>
            text.replace(poss, 6, L"");   // <span>
            count++;
        } else {
            break;
        }
    }
    
    // 2. Удаление <span class="Нет-перенос">
    if (text.find(L"<span class=\"Нет-перенос\">") != wstring::npos) {
        size_t poss = text.find(L"<span class=\"Нет-перенос\">");
        size_t poss2 = text.find(L"</span>", poss);
        
        if (poss2 != wstring::npos) {
            text.replace(poss2, 7, L"");
            text.replace(poss, 27, L"");
            count++;
        }
    }
    
    // 3. Удаление <span class="No-break">
    if (text.find(L"<span class=\"No-break\">") != wstring::npos) {
        size_t poss = text.find(L"<span class=\"No-break\">");
        size_t poss2 = text.find(L"</span>", poss);
        
        if (poss2 != wstring::npos) {
            text.replace(poss2, 7, L"");
            text.replace(poss, 23, L"");
            count++;
        }
    }
}

} // namespace BookProcessor
