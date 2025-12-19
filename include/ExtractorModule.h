#ifndef EXTRACTORMODULE_H
#define EXTRACTORMODULE_H

#include "Types.h"
#include <regex>

namespace BookProcessor {

struct ExtractOptions {
    enum Mode { EXTRACT_MAIN, EXTRACT_METADATA };
    Mode mode = EXTRACT_METADATA;
    
    std::vector<wstring> start_markers = {
        L"Все права защищены",
        L"©.*Издательство",
        L"Правообладатель"
    };
    
    std::vector<wstring> end_markers = {
        L"Главный редактор",
        L"Ответственный редактор",
        L"Корректор",
        L"Вёрстка",
        L"Дизайнер"
    };
};

class ExtractorModule {
public:
    ExtractorModule();
    
    ProcessResult processFile(const wstring& input_path,
                             const wstring& output_path,
                             const ExtractOptions& options = ExtractOptions());
    
    ProcessResult processText(wstring& text, const ExtractOptions& options = ExtractOptions());

private:
    std::pair<size_t, size_t> detectContentBounds(const wstring& text, const ExtractOptions& options);
    bool validateExtraction(const wstring& extracted, const wstring& original);
};

} // namespace BookProcessor

#endif // EXTRACTORMODULE_H
