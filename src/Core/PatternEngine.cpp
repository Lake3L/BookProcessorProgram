#include "PatternEngine.h"
#include "Logger.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <locale>

namespace BookProcessor {

PatternEngine::PatternEngine() {
    LOG_INFO(L"PatternEngine", L"Инициализация движка паттернов");
}

void PatternEngine::addRule(const ReplacementRule& rule) {
    // Проверка на дубликаты
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&rule](const ReplacementRule& r) { return r.id == rule.id; });
    
    if (it != rules_.end()) {
        LOG_WARNING(L"PatternEngine", L"Правило с ID '" + rule.id + L"' уже существует, обновляем");
        *it = rule;
    } else {
        rules_.push_back(rule);
        LOG_INFO(L"PatternEngine", L"Добавлено правило: " + rule.id);
    }

    // Компиляция regex, если нужно
    if (rule.use_regex) {
        try {
            compiled_patterns_[rule.id] = std::wregex(rule.pattern);
        } catch (const std::regex_error& e) {
            LOG_ERROR(L"PatternEngine", L"Ошибка компиляции regex для '" + rule.id + L"'");
        }
    }
}

void PatternEngine::removeRule(const wstring& rule_id) {
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&rule_id](const ReplacementRule& r) { return r.id == rule_id; });
    
    if (it != rules_.end()) {
        rules_.erase(it);
        compiled_patterns_.erase(rule_id);
        LOG_INFO(L"PatternEngine", L"Удалено правило: " + rule_id);
    }
}

void PatternEngine::updateRule(const wstring& rule_id, const ReplacementRule& updated_rule) {
    removeRule(rule_id);
    addRule(updated_rule);
}

std::vector<ReplacementRule> PatternEngine::getRules() const {
    return rules_;
}

std::vector<ReplacementRule> PatternEngine::getRulesByCategory(const wstring& category) const {
    std::vector<ReplacementRule> filtered;
    std::copy_if(rules_.begin(), rules_.end(), std::back_inserter(filtered),
        [&category](const ReplacementRule& r) { return r.category == category; });
    return filtered;
}

ReplacementRule* PatternEngine::getRule(const wstring& rule_id) {
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&rule_id](const ReplacementRule& r) { return r.id == rule_id; });
    
    return (it != rules_.end()) ? &(*it) : nullptr;
}

ProcessResult PatternEngine::applyRules(wstring& text, const std::vector<wstring>& rule_ids) {
    ProcessResult result;
    result.success = true;

    // Сортировка правил по приоритету
    auto rules_to_apply = rules_;
    if (!rule_ids.empty()) {
        rules_to_apply.clear();
        for (const auto& id : rule_ids) {
            auto* rule = getRule(id);
            if (rule) rules_to_apply.push_back(*rule);
        }
    }

    std::sort(rules_to_apply.begin(), rules_to_apply.end(),
        [](const ReplacementRule& a, const ReplacementRule& b) {
            return a.priority < b.priority;
        });

    // Применение правил
    for (const auto& rule : rules_to_apply) {
        if (!rule.enabled) continue;

        int count = 0;
        try {
            if (rule.use_regex) {
                replaceRegex(text, rule.pattern, rule.replacement, count);
            } else {
                replaceSimple(text, rule.pattern, rule.replacement, count);
            }

            if (count > 0) {
                result.operations_count += count;
                wstring detail = rule.id + L": " + std::to_wstring(count) + L" замен";
                result.details.push_back(detail);
                LOG_INFO(L"PatternEngine", detail);
            }
        } catch (const std::exception& e) {
            result.errors_count++;
            wstring error = L"Ошибка при применении правила '" + rule.id + L"'";
            result.details.push_back(error);
            LOG_ERROR(L"PatternEngine", error);
        }
    }

    result.message = L"Применено правил: " + std::to_wstring(rules_to_apply.size()) +
                     L", выполнено замен: " + std::to_wstring(result.operations_count);
    
    return result;
}

ProcessResult PatternEngine::applyRule(wstring& text, const wstring& rule_id) {
    return applyRules(text, {rule_id});
}

void PatternEngine::replaceSimple(wstring& text, const wstring& from, const wstring& to, int& count) {
    if (from.empty()) return;
    
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != wstring::npos) {
        text.replace(pos, from.length(), to);
        pos += to.length();
        count++;
    }
}

void PatternEngine::replaceRegex(wstring& text, const wstring& pattern, 
                                  const wstring& replacement, int& count) {
    try {
        std::wregex regex(pattern);
        wstring result;
        
        auto begin = std::wsregex_iterator(text.begin(), text.end(), regex);
        auto end = std::wsregex_iterator();
        
        count = std::distance(begin, end);
        
        if (count > 0) {
            result = std::regex_replace(text, regex, replacement);
            text = result;
        }
    } catch (const std::regex_error&) {
        throw;
    }
}

PatternEngine::TestResult PatternEngine::testRule(const wstring& pattern, 
                                                   const wstring& sample_text, 
                                                   bool use_regex) {
    TestResult result;
    
    if (use_regex) {
        try {
            std::wregex regex(pattern);
            result.syntax_valid = true;
            
            auto begin = std::wsregex_iterator(sample_text.begin(), sample_text.end(), regex);
            auto end = std::wsregex_iterator();
            
            for (auto it = begin; it != end; ++it) {
                result.matches.push_back(it->str());
                result.match_count++;
            }
        } catch (const std::regex_error& e) {
            result.syntax_valid = false;
            result.error_message = L"Ошибка regex: " + std::wstring(e.what(), e.what() + strlen(e.what()));
        }
    } else {
        result.syntax_valid = true;
        size_t pos = 0;
        while ((pos = sample_text.find(pattern, pos)) != wstring::npos) {
            result.matches.push_back(pattern);
            result.match_count++;
            pos += pattern.length();
        }
    }
    
    return result;
}

std::vector<wstring> PatternEngine::validateRules() const {
    std::vector<wstring> errors;
    
    for (const auto& rule : rules_) {
        if (rule.pattern.empty()) {
            errors.push_back(L"Правило '" + rule.id + L"': пустой паттерн");
        }
        
        if (rule.use_regex) {
            try {
                std::wregex test(rule.pattern);
            } catch (const std::regex_error&) {
                errors.push_back(L"Правило '" + rule.id + L"': некорректный regex");
            }
        }
    }
    
    return errors;
}

} // namespace BookProcessor
