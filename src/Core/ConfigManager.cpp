#include "ConfigManager.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>

namespace BookProcessor {

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const wstring& config_path) {
    QFile file(QString::fromStdWString(config_path));
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_WARNING(L"ConfigManager", L"Не удалось открыть конфиг: " + config_path);
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isNull()) {
        LOG_ERROR(L"ConfigManager", L"Ошибка парсинга JSON");
        return false;
    }
    
    QJsonObject j = doc.object();
    
    // Загружаем настройки приложения
    if (j.contains("app_config")) {
        QJsonObject ac = j["app_config"].toObject();
        if (ac.contains("working_directory")) 
            app_config_.working_directory = ac["working_directory"].toString().toStdWString();
        if (ac.contains("create_backups")) 
            app_config_.create_backups = ac["create_backups"].toBool();
        if (ac.contains("show_diff")) 
            app_config_.show_diff = ac["show_diff"].toBool();
        if (ac.contains("verbose_logging")) 
            app_config_.verbose_logging = ac["verbose_logging"].toBool();
        if (ac.contains("last_report_file")) 
            app_config_.last_report_file = ac["last_report_file"].toString().toStdWString();
    }
    
    // Загружаем правила Cleaner
    if (j.contains("cleaner_rules")) {
        cleaner_rules_.clear();
        QJsonArray rules_array = j["cleaner_rules"].toArray();
        for (const QJsonValue& value : rules_array) {
            QJsonObject rule_j = value.toObject();
            ReplacementRule rule;
            rule.id = rule_j["id"].toString().toStdWString();
            rule.pattern = rule_j["pattern"].toString().toStdWString();
            rule.replacement = rule_j["replacement"].toString().toStdWString();
            rule.enabled = rule_j.value("enabled").toBool(true);
            rule.use_regex = rule_j.value("use_regex").toBool(false);
            rule.priority = rule_j.value("priority").toInt(10);
            rule.category = rule_j.value("category").toString("general").toStdWString();
            rule.description = rule_j.value("description").toString("").toStdWString();
            rule.user_editable = rule_j.value("user_editable").toBool(true);
            cleaner_rules_.push_back(rule);
        }
    }
    
    LOG_INFO(L"ConfigManager", L"Конфигурация загружена: " + config_path);
    return true;
}

bool ConfigManager::saveConfig(const wstring& config_path) {
    QJsonObject j;
    
    // Сохраняем настройки приложения
    QJsonObject ac;
    ac["working_directory"] = QString::fromStdWString(app_config_.working_directory);
    ac["create_backups"] = app_config_.create_backups;
    ac["show_diff"] = app_config_.show_diff;
    ac["verbose_logging"] = app_config_.verbose_logging;
    ac["last_report_file"] = QString::fromStdWString(app_config_.last_report_file);
    j["app_config"] = ac;
    
    // Сохраняем правила Cleaner
    QJsonArray rules_array;
    for (const auto& rule : cleaner_rules_) {
        QJsonObject rule_j;
        rule_j["id"] = QString::fromStdWString(rule.id);
        rule_j["pattern"] = QString::fromStdWString(rule.pattern);
        rule_j["replacement"] = QString::fromStdWString(rule.replacement);
        rule_j["enabled"] = rule.enabled;
        rule_j["use_regex"] = rule.use_regex;
        rule_j["priority"] = rule.priority;
        rule_j["category"] = QString::fromStdWString(rule.category);
        rule_j["description"] = QString::fromStdWString(rule.description);
        rule_j["user_editable"] = rule.user_editable;
        rules_array.append(rule_j);
    }
    j["cleaner_rules"] = rules_array;
    
    QJsonDocument doc(j);
    QFile file(QString::fromStdWString(config_path));
    if (!file.open(QIODevice::WriteOnly)) {
        LOG_ERROR(L"ConfigManager", L"Не удалось создать файл: " + config_path);
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    
    LOG_INFO(L"ConfigManager", L"Конфигурация сохранена: " + config_path);
    return true;
}

} // namespace BookProcessor
