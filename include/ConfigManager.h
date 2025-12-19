#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "Types.h"
#include "PatternEngine.h"
#include <vector>

namespace BookProcessor {

struct AppConfig {
    wstring working_directory;
    bool create_backups = false;
    bool show_diff = false;
    bool verbose_logging = true;
    wstring last_report_file;
};

class ConfigManager {
public:
    static ConfigManager& instance();
    
    bool loadConfig(const wstring& config_path);
    bool saveConfig(const wstring& config_path);
    
    // Настройки приложения
    AppConfig& getAppConfig() { return app_config_; }
    void setAppConfig(const AppConfig& config) { app_config_ = config; }
    
    // Правила Cleaner
    std::vector<ReplacementRule> getCleanerRules() const { return cleaner_rules_; }
    void setCleanerRules(const std::vector<ReplacementRule>& rules) { cleaner_rules_ = rules; }
    
    // Настройки Links
    LinkOptions getLinkOptions() const { return link_options_; }
    void setLinkOptions(const LinkOptions& options) { link_options_ = options; }
    
    // Настройки Extractor
    ExtractorOptions getExtractorOptions() const { return extractor_options_; }
    void setExtractorOptions(const ExtractorOptions& options) { extractor_options_ = options; }

private:
    ConfigManager() = default;
    
    AppConfig app_config_;
    std::vector<ReplacementRule> cleaner_rules_;
    LinkOptions link_options_;
    ExtractorOptions extractor_options_;
};

} // namespace BookProcessor

#endif // CONFIGMANAGER_H
