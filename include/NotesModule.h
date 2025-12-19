#ifndef NOTESMODULE_H
#define NOTESMODULE_H

#include "Types.h"
#include <vector>

namespace BookProcessor {

struct Note {
    wstring source_file;
    wstring backlink_id;
    wstring display_number;
    wstring footnote_id;
    wstring type; // "footnote" или "endnote"
    bool processed = false;
};

class NotesModule {
public:
    NotesModule();
    
    // Загрузка отчёта
    bool loadReport(const wstring& report_path);
    
    // Обработка
    ProcessResult processFile(const wstring& html_path,
                             const wstring& output_path);
    
    ProcessResult processText(wstring& html_content);
    
    // Доступ к заметкам
    const std::vector<Note>& getNotes() const { return notes_; }
    int getProcessedCount() const;

private:
    std::vector<Note> notes_;
    
    bool processFootnote(wstring& content, Note& note);
    bool processEndnote(wstring& content, Note& note);
    wstring createOutputPath(const wstring& input_path);
};

} // namespace BookProcessor

#endif // NOTESMODULE_H
