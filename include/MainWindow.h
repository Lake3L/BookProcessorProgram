#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "Types.h"
#include "CleanerModule.h"
#include "LinksModule.h"
#include "NotesModule.h"
#include "ExtractorModule.h"
#include "FileManager.h"
#include <memory>

namespace BookProcessor {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_browseWorkingDir_clicked();
    void on_browseOutputDir_clicked();
    void on_addFiles_clicked();
    void on_removeFiles_clicked();
    void on_refreshFiles_clicked();
    void on_processButton_clicked();
    void on_fileList_itemSelectionChanged();
    void on_showHelp_clicked();
    
    // Модули
    void on_cleanerCheckBox_toggled(bool checked);
    void on_linksCheckBox_toggled(bool checked);
    void on_notesCheckBox_toggled(bool checked);
    void on_extractorCheckBox_toggled(bool checked);
    
    // Настройки
    void on_configureCleanerButton_clicked();
    void on_configureLinksButton_clicked();
    void on_browseReportButton_clicked();
    void on_toggleLog_clicked();
    void on_clearLogButton_clicked();

private:
    // GUI элементы
    QWidget* central_widget_;
    QLineEdit* working_dir_edit_;
    QLineEdit* output_dir_edit_;
    QPushButton* browse_dir_button_;
    QPushButton* browse_output_button_;
    QPushButton* add_files_button_;
    QPushButton* remove_files_button_;
    QPushButton* refresh_button_;
    QPushButton* help_button_;
    QLabel* file_count_label_;
    QListWidget* file_list_;
    
    QCheckBox* cleaner_checkbox_;
    QCheckBox* links_checkbox_;
    QCheckBox* notes_checkbox_;
    QCheckBox* extractor_checkbox_;
    
    QPushButton* configure_cleaner_button_;
    QPushButton* configure_links_button_;
    QPushButton* browse_report_button_;
    
    QCheckBox* create_backup_checkbox_;
    QCheckBox* show_diff_checkbox_;
    QCheckBox* verbose_log_checkbox_;
    
    QPushButton* process_button_;
    QTextEdit* log_text_;
    QPushButton* toggle_log_button_;
    QPushButton* clear_log_button_;
    QWidget* log_container_;
    
    // Модули
    std::shared_ptr<FileManager> file_manager_;
    std::shared_ptr<CleanerModule> cleaner_;
    std::shared_ptr<LinksModule> links_;
    std::shared_ptr<NotesModule> notes_;
    std::shared_ptr<ExtractorModule> extractor_;
    
    void setupUI();
    void setupConnections();
    void updateFileList();
    void logMessage(const QString& msg, LogLevel level = LogLevel::INFO);
    void processFiles();
};

} // namespace BookProcessor

#endif // MAINWINDOW_H
