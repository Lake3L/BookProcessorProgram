#ifndef CLEANERMODULE_H
#define CLEANERMODULE_H

#include "Types.h"
#include "PatternEngine.h"
#include <memory>

namespace BookProcessor {

class CleanerModule {
public:
    CleanerModule();
    
    // Инициализация с правилами
    void initializeDefaultRules();
    void loadRulesFromEngine(std::shared_ptr<PatternEngine> engine);
    
    // Обработка
    ProcessResult processFile(const wstring& input_path, 
                             const wstring& output_path,
                             const ProcessOptions& options = ProcessOptions());
    
    ProcessResult processText(wstring& text, const ProcessOptions& options = ProcessOptions());
    
    // Доступ к движку правил
    std::shared_ptr<PatternEngine> getEngine() const { return engine_; }
    
    // Специальные правила (сложные, не через PatternEngine)
    void processParaOverride(wstring& text, int& count);
    void processTextAnchor(wstring& text, int& count);
    void processIndexMarker(wstring& text, int& count);
    void processSpanTags(wstring& text, int& count);

private:
    std::shared_ptr<PatternEngine> engine_;
    
    // Вспомогательные методы
    void removeSpanPair(wstring& text, const wstring& open_tag, size_t& pos);
};

} // namespace BookProcessor

#endif // CLEANERMODULE_H
