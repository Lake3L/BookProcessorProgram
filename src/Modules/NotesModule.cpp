#include "NotesModule.h"
#include "Logger.h"
#include "FileManager.h"
#include <QFile>
#include <QTextStream>
#include <sstream>

namespace BookProcessor {

NotesModule::NotesModule() {
    LOG_INFO(L"NotesModule", L"Модуль обработки сносок инициализирован");
}

bool NotesModule::loadReport(const wstring& report_path) {
    QString qpath = QString::fromStdWString(report_path);
    QFile file(qpath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(L"NotesModule", L"Не удалось открыть файл отчёта: " + report_path);
        return false;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    QString line = in.readLine(); // Пропускаем заголовок
    
    notes_.clear();
    
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.isEmpty()) continue;
        
        wstring wline = line.toStdWString();
        
        // Парсинг CSV: source_file,line,backlink_id,display_number,target,footnote_id,exists,...
        std::vector<wstring> fields;
        std::wstringstream ss(wline);
        wstring field;
        
        while (std::getline(ss, field, L',')) {
            fields.push_back(field);
        }
        
        if (fields.size() < 6) continue;
        
        Note note;
        note.source_file = fields[0];
        note.backlink_id = fields[2];
        note.display_number = fields[3];
        note.footnote_id = fields[5];
        
        // Определяем тип
        if (note.footnote_id.find(L"footnote") != wstring::npos) {
            note.type = L"footnote";
        } else if (note.footnote_id.find(L"endnote") != wstring::npos) {
            note.type = L"endnote";
        } else {
            continue; // Пропускаем неизвестные типы
        }
        
        notes_.push_back(note);
    }
    
    file.close();
    LOG_INFO(L"NotesModule", L"Загружено сносок: " + std::to_wstring(notes_.size()));
    
    return true;
}

ProcessResult NotesModule::processText(wstring& html_content) {
    ProcessResult result;
    result.success = true;
    
    for (auto& note : notes_) {
        if (note.processed) continue;
        
        bool success = false;
        if (note.type == L"footnote") {
            success = processFootnote(html_content, note);
        } else if (note.type == L"endnote") {
            success = processEndnote(html_content, note);
        }
        
        if (success) {
            result.operations_count++;
            LOG_INFO(L"NotesModule", L"Обработана сноска: " + note.footnote_id);
        }
    }
    
    result.message = L"Обработано сносок: " + std::to_wstring(result.operations_count);
    return result;
}

ProcessResult NotesModule::processFile(const wstring& html_path, const wstring& output_path) {
    FileManager fm;
    wstring content;
    
    ProcessResult result;
    
    if (!fm.readFile(html_path, content)) {
        result.success = false;
        result.message = L"Ошибка чтения файла";
        return result;
    }
    
    result = processText(content);
    
    if (!fm.writeFile(output_path, content)) {
        result.success = false;
        result.message = L"Ошибка записи файла";
    }
    
    return result;
}

bool NotesModule::processFootnote(wstring& content, Note& note) {
    wstring search_pattern = L"href=\"#" + note.backlink_id + L"\">" + note.display_number + L"</a>";
    wstring replace_pattern = L"href=\"" + note.source_file + L"#" + note.backlink_id + L"\">" + note.display_number + L"</a>";
    
    size_t pos = content.find(search_pattern);
    if (pos != wstring::npos) {
        content.replace(pos, search_pattern.length(), replace_pattern);
        note.processed = true;
        return true;
    }
    
    return false;
}

bool NotesModule::processEndnote(wstring& content, Note& note) {
    wstring search_pattern = L"href=\"#" + note.footnote_id + L"\">" + note.display_number + L"</a>";
    wstring replace_pattern = L"href=\"" + note.source_file + L"#" + note.footnote_id + L"\">" + note.display_number + L"</a>";
    
    size_t pos = content.find(search_pattern);
    if (pos != wstring::npos) {
        content.replace(pos, search_pattern.length(), replace_pattern);
        note.processed = true;
        return true;
    }
    
    return false;
}

int NotesModule::getProcessedCount() const {
    return std::count_if(notes_.begin(), notes_.end(), 
                        [](const Note& n) { return n.processed; });
}

} // namespace BookProcessor
