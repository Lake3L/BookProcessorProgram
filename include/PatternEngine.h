#ifndef PATTERNENGINE_H
#define PATTERNENGINE_H

#include "Types.h"
#include <regex>
#include <map>

namespace BookProcessor {

class PatternEngine {
public:
    PatternEngine();
    
    // Загрузка правил
    bool loadRulesFromConfig(const wstring& config_path);
    void addRule(const ReplacementRule& rule);
    void removeRule(const wstring& rule_id);
    void updateRule(const wstring& rule_id, const ReplacementRule& updated_rule);
    
    // Получение правил
    std::vector<ReplacementRule> getRules() const;
    std::vector<ReplacementRule> getRulesByCategory(const wstring& category) const;
    ReplacementRule* getRule(const wstring& rule_id);
    
    // Применение правил
    ProcessResult applyRules(wstring& text, const std::vector<wstring>& rule_ids = {});
    ProcessResult applyRule(wstring& text, const wstring& rule_id);
    
    // Тестирование правила
    struct TestResult {
        bool syntax_valid = false;
        int match_count = 0;
        std::vector<wstring> matches;
        wstring error_message;
    };
    TestResult testRule(const wstring& pattern, const wstring& sample_text, bool use_regex);
    
    // Валидация
    std::vector<wstring> validateRules() const;
    
    // Сохранение в конфиг
    bool saveRulesToConfig(const wstring& config_path) const;

private:
    void replaceSimple(wstring& text, const wstring& from, const wstring& to, int& count);
    void replaceRegex(wstring& text, const wstring& pattern, const wstring& replacement, int& count);
    
    std::vector<ReplacementRule> rules_;
    std::map<wstring, std::wregex> compiled_patterns_;
};

} // namespace BookProcessor

#endif // PATTERNENGINE_H
