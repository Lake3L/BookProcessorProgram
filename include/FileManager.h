#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Types.h"
#include <QObject>
#include <QString>
#include <QDir>

namespace BookProcessor {

class FileManager : public QObject {
    Q_OBJECT

public:
    FileManager();
    
    // Управление рабочей директорией
    bool setWorkingDirectory(const wstring& path);
    wstring getWorkingDirectory() const;
    
    // Установка выходной директории
    bool setOutputDirectory(const wstring& path);
    
    // Работа с файлами
    std::vector<wstring> listFilesInInput(const wstring& pattern = L"*.html") const;
    wstring generateOutputPath(const wstring& input_filename, const wstring& suffix = L"_processed") const;
    
    // Чтение/запись с UTF-8
    bool readFile(const wstring& path, wstring& content) const;
    bool writeFile(const wstring& path, const wstring& content);
    
    // Резервные копии
    bool createBackup(const wstring& file_path) const;
    
    // Проверки
    bool fileExists(const wstring& path) const;
    bool isValidWorkingDir() const;
    
    // Пути к подпапкам
    wstring getInputPath() const;
    wstring getOutputPath() const;
    wstring getConfigPath() const;
    wstring getLogsPath() const;

signals:
    void fileProcessed(const QString& filename);
    void errorOccurred(const QString& error);

private:
    wstring working_dir_;
    wstring input_dir_;
    wstring output_dir_;
    wstring config_dir_;
    wstring logs_dir_;
    
    wstring toAbsolutePath(const wstring& relative_path) const;
};

} // namespace BookProcessor

#endif // FILEMANAGER_H
