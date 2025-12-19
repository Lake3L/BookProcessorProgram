#ifndef LINKSMODULE_H
#define LINKSMODULE_H

#include "Types.h"
#include <set>

namespace BookProcessor {

// LinkOptions определён в Types.h

struct LinkStats {
    int total_found = 0;
    int converted = 0;
    int skipped = 0;
    int errors = 0;
};

class LinksModule {
public:
    LinksModule();
    
    // Обработка
    ProcessResult processFile(const wstring& input_path,
                             const wstring& output_path,
                             const LinkOptions& options = LinkOptions());
    
    ProcessResult processText(wstring& text, const LinkOptions& options = LinkOptions());
    
    // Статистика последней обработки
    LinkStats getLastStats() const { return last_stats_; }

private:
    LinkStats last_stats_;
    
    // Вспомогательные методы
    wstring extractLink(const wstring& text, size_t start_pos, const LinkOptions& options);
    bool isTerminator(wchar_t ch, const LinkOptions& options);
    bool isValidUrl(const wstring& url);
    bool isInsideLink(const wstring& text, size_t pos);
    size_t findNextProtocol(const wstring& text, size_t start, const LinkOptions& options);
    wstring makeClickable(const wstring& url, const wstring& css_class);
};

} // namespace BookProcessor

#endif // LINKSMODULE_H
