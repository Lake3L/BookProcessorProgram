#include "FileManager.h"
#include "Logger.h"
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>

namespace BookProcessor {

FileManager::FileManager() {
    LOG_INFO(L"FileManager", L"Инициализация файлового менеджера");
}

bool FileManager::setWorkingDirectory(const wstring& path) {
    QString qpath = QString::fromStdWString(path);
    QDir dir(qpath);
    
    if (!dir.exists()) {
        LOG_ERROR(L"FileManager", L"Директория не существует: " + path);
        return false;
    }
    
    working_dir_ = path;
    // Не создаём автоматически подпапки
    
    LOG_INFO(L"FileManager", L"Рабочая директория установлена: " + path);
    return true;
}

wstring FileManager::getWorkingDirectory() const {
    return working_dir_;
}

bool FileManager::setOutputDirectory(const wstring& path) {
    QString qpath = QString::fromStdWString(path);
    QDir dir;
    
    // Создаём выходную папку если не существует
    if (!dir.exists(qpath)) {
        if (!dir.mkpath(qpath)) {
            LOG_ERROR(L"FileManager", L"Не удалось создать выходную папку: " + path);
            return false;
        }
    }
    
    output_dir_ = path;
    LOG_INFO(L"FileManager", L"Выходная директория установлена: " + path);
    return true;
}

std::vector<wstring> FileManager::listFilesInInput(const wstring& pattern) const {
    std::vector<wstring> files;
    
    if (working_dir_.empty()) {
        return files; // Нет рабочей директории
    }
    
    QDir dir(QString::fromStdWString(working_dir_));
    QStringList filters;
    filters << QString::fromStdWString(pattern);
    
    // Рекурсивный поиск файлов
    QDirIterator it(dir.absolutePath(), filters, QDir::Files, QDirIterator::Subdirectories);
    
    while (it.hasNext()) {
        QString filePath = it.next();
        // Возвращаем абсолютный путь
        files.push_back(filePath.toStdWString());
    }
    
    return files;
}

wstring FileManager::generateOutputPath(const wstring& input_filename, const wstring& suffix) const {
    QString qname = QString::fromStdWString(input_filename);
    QFileInfo info(qname);
    
    QString basename = info.completeBaseName();
    QString extension = info.suffix();
    
    QString output_filename = basename + QString::fromStdWString(suffix) + "." + extension;
    
    return output_dir_ + L"/" + output_filename.toStdWString();
}

bool FileManager::readFile(const wstring& path, wstring& content) const {
    QString qpath = QString::fromStdWString(path);
    QFile file(qpath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(L"FileManager", L"Не удалось открыть файл для чтения: " + path);
        return false;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString text = in.readAll();
    content = text.toStdWString();
    
    file.close();
    LOG_INFO(L"FileManager", L"Файл прочитан: " + path);
    return true;
}

bool FileManager::writeFile(const wstring& path, const wstring& content) {
    QString qpath = QString::fromStdWString(path);
    QFile file(qpath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(L"FileManager", L"Не удалось открыть файл для записи: " + path);
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << QString::fromStdWString(content);
    file.close();
    
    LOG_INFO(L"FileManager", L"Файл записан: " + path);
    emit fileProcessed(QString::fromStdWString(path));
    return true;
}

bool FileManager::createBackup(const wstring& file_path) const {
    QFile file(QString::fromStdWString(file_path));
    
    if (!file.exists()) return false;
    
    QString backup_path = QString::fromStdWString(file_path) + ".backup";
    
    // Удаляем старый backup, если есть
    if (QFile::exists(backup_path)) {
        QFile::remove(backup_path);
    }
    
    bool success = file.copy(backup_path);
    
    if (success) {
        LOG_INFO(L"FileManager", L"Создана резервная копия: " + file_path);
    }
    
    return success;
}

bool FileManager::fileExists(const wstring& path) const {
    return QFile::exists(QString::fromStdWString(path));
}

bool FileManager::isValidWorkingDir() const {
    return !working_dir_.empty() && 
           QDir(QString::fromStdWString(working_dir_)).exists();
}

wstring FileManager::getInputPath() const { return input_dir_; }
wstring FileManager::getOutputPath() const { return output_dir_; }
wstring FileManager::getConfigPath() const { return config_dir_; }
wstring FileManager::getLogsPath() const { return logs_dir_; }

wstring FileManager::toAbsolutePath(const wstring& relative_path) const {
    QFileInfo info(QString::fromStdWString(working_dir_ + L"/" + relative_path));
    return info.absoluteFilePath().toStdWString();
}

} // namespace BookProcessor
