#include "MainWindow.h"
#include "Logger.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>

namespace BookProcessor {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("BookProcessor v2.0");
    resize(1000, 700);
    
    // Инициализация модулей
    file_manager_ = std::make_shared<FileManager>();
    cleaner_ = std::make_shared<CleanerModule>();
    links_ = std::make_shared<LinksModule>();
    notes_ = std::make_shared<NotesModule>();
    extractor_ = std::make_shared<ExtractorModule>();
    
    setupUI();
    setupConnections();
    
    LOG_INFO(L"MainWindow", L"Главное окно инициализировано");
    logMessage("BookProcessor v2.0 готов к работе!");
}

MainWindow::~MainWindow() {
    LOG_INFO(L"MainWindow", L"Главное окно закрыто");
}

void MainWindow::setupUI() {
    central_widget_ = new QWidget(this);
    setCentralWidget(central_widget_);
    
    auto* main_layout = new QVBoxLayout(central_widget_);
    
    // === ВЕРХНЯЯ ПАНЕЛЬ ===
    auto* header_layout = new QHBoxLayout();
    auto* title_label = new QLabel("<h2>BookProcessor v2.0</h2>", central_widget_);
    help_button_ = new QPushButton("Помощь", central_widget_);
    help_button_->setMaximumWidth(120);
    header_layout->addWidget(title_label);
    header_layout->addStretch();
    header_layout->addWidget(help_button_);
    main_layout->addLayout(header_layout);
    
    // === РАБОЧАЯ ПАПКА ===
    auto* dir_group = new QGroupBox("Рабочая папка (опционально)", central_widget_);
    auto* dir_layout = new QVBoxLayout(dir_group);
    
    auto* dir_row = new QHBoxLayout();
    working_dir_edit_ = new QLineEdit(dir_group);
    working_dir_edit_->setPlaceholderText("Оставьте пустым для ручной загрузки файлов");
    working_dir_edit_->setReadOnly(true);
    
    browse_dir_button_ = new QPushButton("Выбрать папку", dir_group);
    refresh_button_ = new QPushButton("Обновить", dir_group);
    
    dir_row->addWidget(working_dir_edit_);
    dir_row->addWidget(browse_dir_button_);
    dir_row->addWidget(refresh_button_);
    dir_layout->addLayout(dir_row);
    
    // Выходная папка
    auto* output_row = new QHBoxLayout();
    auto* output_label = new QLabel("Выходная папка:", dir_group);
    output_label->setMinimumWidth(100);
    output_row->addWidget(output_label);
    
    output_dir_edit_ = new QLineEdit(dir_group);
    output_dir_edit_->setReadOnly(true);
    
    browse_output_button_ = new QPushButton("Выбрать...", dir_group);
    
    output_row->addWidget(output_dir_edit_);
    output_row->addWidget(browse_output_button_);
    dir_layout->addLayout(output_row);
    
    auto* hint_label = new QLabel("💡 При выборе папки будут найдены все .html и .txt файлы в подпапках", dir_group);
    hint_label->setStyleSheet("color: #888; font-size: 9pt;");
    dir_layout->addWidget(hint_label);
    
    main_layout->addWidget(dir_group);
    
    // === ВЕРХНИЙ КОНТЕЙНЕР (файлы + модули + опции + кнопка) ===
    auto* top_container = new QWidget(central_widget_);
    auto* top_layout = new QVBoxLayout(top_container);
    top_layout->setContentsMargins(0, 0, 0, 0);
    
    // === СПИСОК ФАЙЛОВ ===
    auto* files_group = new QGroupBox("📄 Файлы для обработки", top_container);
    auto* files_layout = new QVBoxLayout(files_group);
    
    auto* files_toolbar = new QHBoxLayout();
    file_count_label_ = new QLabel("Файлов: 0", files_group);
    add_files_button_ = new QPushButton("+Добавить файлы", files_group);
    remove_files_button_ = new QPushButton("-Удалить выбранные", files_group);
    files_toolbar->addWidget(file_count_label_);
    files_toolbar->addStretch();
    files_toolbar->addWidget(add_files_button_);
    files_toolbar->addWidget(remove_files_button_);
    files_layout->addLayout(files_toolbar);
    
    file_list_ = new QListWidget(files_group);
    file_list_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    files_layout->addWidget(file_list_);
    top_layout->addWidget(files_group);
    
    // === МОДУЛИ ОБРАБОТКИ ===
    auto* modules_group = new QGroupBox("Модули обработки", top_container);
    auto* modules_layout = new QVBoxLayout(modules_group);
    
    // Cleaner
    auto* cleaner_layout = new QHBoxLayout();
    cleaner_checkbox_ = new QCheckBox("Cleaner - очистка HTML", modules_group);
    cleaner_checkbox_->setChecked(true);
    configure_cleaner_button_ = new QPushButton("Настроить...", modules_group);
    cleaner_layout->addWidget(cleaner_checkbox_);
    cleaner_layout->addWidget(configure_cleaner_button_);
    cleaner_layout->addStretch();
    modules_layout->addLayout(cleaner_layout);
    
    // Links
    auto* links_layout = new QHBoxLayout();
    links_checkbox_ = new QCheckBox("Links - преобразование URL", modules_group);
    links_checkbox_->setChecked(true);
    configure_links_button_ = new QPushButton("Настроить...", modules_group);
    links_layout->addWidget(links_checkbox_);
    links_layout->addWidget(configure_links_button_);
    links_layout->addStretch();
    modules_layout->addLayout(links_layout);
    
    // Notes
    auto* notes_layout = new QHBoxLayout();
    notes_checkbox_ = new QCheckBox("Notes - обработка сносок", modules_group);
    browse_report_button_ = new QPushButton("Файл отчёта...", modules_group);
    browse_report_button_->setEnabled(false);
    notes_layout->addWidget(notes_checkbox_);
    notes_layout->addWidget(browse_report_button_);
    notes_layout->addStretch();
    modules_layout->addLayout(notes_layout);
    
    // Extractor
    auto* extractor_layout = new QHBoxLayout();
    extractor_checkbox_ = new QCheckBox("Extractor - извлечение метаданных", modules_group);
    extractor_layout->addWidget(extractor_checkbox_);
    extractor_layout->addStretch();
    modules_layout->addLayout(extractor_layout);
    
    top_layout->addWidget(modules_group);
    
    // === ОПЦИИ ===
    auto* options_group = new QGroupBox("Опции", top_container);
    auto* options_layout = new QHBoxLayout(options_group);
    
    create_backup_checkbox_ = new QCheckBox("Создать резервные копии", options_group);
    show_diff_checkbox_ = new QCheckBox("Показать diff", options_group);
    verbose_log_checkbox_ = new QCheckBox("Подробное логирование", options_group);
    verbose_log_checkbox_->setChecked(true);
    
    options_layout->addWidget(create_backup_checkbox_);
    options_layout->addWidget(show_diff_checkbox_);
    options_layout->addWidget(verbose_log_checkbox_);
    options_layout->addStretch();
    
    // Кнопка показа/скрытия логов
    toggle_log_button_ = new QPushButton("▼ Скрыть логи", options_group);
    toggle_log_button_->setMaximumWidth(150);
    options_layout->addWidget(toggle_log_button_);
    
    top_layout->addWidget(options_group);
    
    // === КНОПКА ОБРАБОТКИ ===
    process_button_ = new QPushButton("Обработать файлы", top_container);
    process_button_->setStyleSheet("QPushButton { font-size: 14pt; padding: 10px; background-color: #4CAF50; color: white; } QPushButton:hover { background-color: #45a049; }");
    process_button_->setMinimumHeight(50);
    top_layout->addWidget(process_button_);
    
    // === SPLITTER для изменяемых размеров между верхом и логом ===
    auto* splitter = new QSplitter(Qt::Vertical, central_widget_);
    splitter->addWidget(top_container);
    
    // === ЛОГ ===
    log_container_ = new QWidget(splitter);
    auto* log_container_layout = new QVBoxLayout(log_container_);
    log_container_layout->setContentsMargins(0, 0, 0, 0);
    
    auto* log_group = new QGroupBox("Лог", log_container_);
    auto* log_layout = new QVBoxLayout(log_group);
    
    log_text_ = new QTextEdit(log_group);
    log_text_->setReadOnly(true);
    log_text_->setStyleSheet("QTextEdit { font-family: Consolas, monospace; font-size: 9pt; background-color: #1e1e1e; color: #d4d4d4; }");
    
    auto* log_buttons_layout = new QHBoxLayout();
    clear_log_button_ = new QPushButton("Очистить лог", log_group);
    log_buttons_layout->addStretch();
    log_buttons_layout->addWidget(clear_log_button_);
    
    log_layout->addWidget(log_text_);
    log_layout->addLayout(log_buttons_layout);
    
    log_container_layout->addWidget(log_group);
    splitter->addWidget(log_container_);
    
    // Устанавливаем начальные размеры и ограничения
    splitter->setStretchFactor(0, 6);  // Верхняя часть (файлы+модули+кнопка)
    splitter->setStretchFactor(1, 4);  // Нижняя часть (лог)
    splitter->setChildrenCollapsible(false);  // Запрещаем полное схлопывание
    
    // Минимальные размеры для областей
    top_container->setMinimumHeight(400);
    log_container_->setMinimumHeight(100);
    
    main_layout->addWidget(splitter);
}

void MainWindow::setupConnections() {
    // Кнопки
    connect(browse_dir_button_, &QPushButton::clicked, this, &MainWindow::on_browseWorkingDir_clicked);
    connect(browse_output_button_, &QPushButton::clicked, this, &MainWindow::on_browseOutputDir_clicked);
    connect(add_files_button_, &QPushButton::clicked, this, &MainWindow::on_addFiles_clicked);
    connect(remove_files_button_, &QPushButton::clicked, this, &MainWindow::on_removeFiles_clicked);
    connect(refresh_button_, &QPushButton::clicked, this, &MainWindow::on_refreshFiles_clicked);
    connect(process_button_, &QPushButton::clicked, this, &MainWindow::on_processButton_clicked);
    connect(toggle_log_button_, &QPushButton::clicked, this, &MainWindow::on_toggleLog_clicked);
    connect(clear_log_button_, &QPushButton::clicked, this, &MainWindow::on_clearLogButton_clicked);
    connect(help_button_, &QPushButton::clicked, this, &MainWindow::on_showHelp_clicked);
    
    // Чекбоксы модулей
    connect(cleaner_checkbox_, &QCheckBox::toggled, this, &MainWindow::on_cleanerCheckBox_toggled);
    connect(links_checkbox_, &QCheckBox::toggled, this, &MainWindow::on_linksCheckBox_toggled);
    connect(notes_checkbox_, &QCheckBox::toggled, this, &MainWindow::on_notesCheckBox_toggled);
    connect(extractor_checkbox_, &QCheckBox::toggled, this, &MainWindow::on_extractorCheckBox_toggled);
    
    // Настройки
    connect(configure_cleaner_button_, &QPushButton::clicked, this, &MainWindow::on_configureCleanerButton_clicked);
    connect(configure_links_button_, &QPushButton::clicked, this, &MainWindow::on_configureLinksButton_clicked);
    connect(browse_report_button_, &QPushButton::clicked, this, &MainWindow::on_browseReportButton_clicked);
    
    // Список файлов
    connect(file_list_, &QListWidget::itemSelectionChanged, this, &MainWindow::on_fileList_itemSelectionChanged);
    
    // Подключаем логгер к GUI
    connect(&Logger::instance(), &Logger::logAdded, this, [this](const QString& msg) {
        logMessage(msg);
    });
}

void MainWindow::on_browseWorkingDir_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Выберите рабочую папку");
    
    if (!dir.isEmpty()) {
        if (file_manager_->setWorkingDirectory(dir.toStdWString())) {
            working_dir_edit_->setText(dir);
            logMessage("✅ Рабочая папка установлена: " + dir);
            
            // Автоматически устанавливаем выходную папку
            QString output_dir = dir + "/output";
            if (file_manager_->setOutputDirectory(output_dir.toStdWString())) {
                output_dir_edit_->setText(output_dir);
            }
            
            updateFileList();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось установить рабочую папку");
        }
    }
}

void MainWindow::on_browseOutputDir_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Выберите выходную папку");
    
    if (!dir.isEmpty()) {
        if (file_manager_->setOutputDirectory(dir.toStdWString())) {
            output_dir_edit_->setText(dir);
            logMessage("✅ Выходная папка установлена: " + dir);
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось установить выходную папку");
        }
    }
}

void MainWindow::updateFileList() {
    file_list_->clear();
    
    if (!file_manager_->isValidWorkingDir()) {
        file_count_label_->setText("Файлов: 0");
        logMessage("💡 Используйте '+Добавить файлы' для ручной загрузки");
        return;
    }
    
    // Ищем все HTML и TXT файлы рекурсивно
    auto html_files = file_manager_->listFilesInInput(L"*.html");
    auto txt_files = file_manager_->listFilesInInput(L"*.txt");
    
    std::vector<wstring> all_files;
    all_files.insert(all_files.end(), html_files.begin(), html_files.end());
    all_files.insert(all_files.end(), txt_files.begin(), txt_files.end());
    
    logMessage(QString("Найдено файлов: %1 (.html: %2, .txt: %3)")
        .arg(all_files.size()).arg(html_files.size()).arg(txt_files.size()));
    
    for (const auto& file : all_files) {
        auto* item = new QListWidgetItem(QString::fromStdWString(file), file_list_);
        item->setCheckState(Qt::Checked);
        
        // Иконки по типу файла
        if (file.find(L".html") != wstring::npos) {
            item->setForeground(QColor("#6bcf7f"));
        } else {
            item->setForeground(QColor("#ffd93d"));
        }
    }
    
    file_count_label_->setText(QString("Файлов: %1").arg(all_files.size()));
}

void MainWindow::on_processButton_clicked() {
    processFiles();
}

void MainWindow::processFiles() {
    // Получаем отмеченные файлы
    QStringList selected_files;
    QStringList full_paths;
    
    for (int i = 0; i < file_list_->count(); ++i) {
        auto* item = file_list_->item(i);
        if (item->checkState() == Qt::Checked) {
            selected_files << item->text();
            
            // Получаем полный путь
            QString full_path = item->data(Qt::UserRole).toString();
            if (full_path.isEmpty()) {
                // Если UserRole пуст, значит файл из рабочей папки
                if (file_manager_->isValidWorkingDir()) {
                    full_path = QString::fromStdWString(
                        file_manager_->getInputPath() + L"/" + item->text().toStdWString());
                } else {
                    logMessage("  ❌ [ОШИБКА] Не найден путь к файлу: " + item->text(), LogLevel::ERROR);
                    continue;
                }
            }
            full_paths << full_path;
        }
    }
    
    if (selected_files.empty()) {
        QMessageBox::information(this, "Информация", "Нет выбранных файлов для обработки");
        return;
    }
    
    logMessage("════════════════════════════════════");
    logMessage("НАЧАЛО ОБРАБОТКИ");
    logMessage("════════════════════════════════════");
    
    int total_ops = 0;
    int processed_count = 0;
    
    for (int i = 0; i < selected_files.size(); ++i) {
        QString filename_q = selected_files[i];
        QString full_path_q = full_paths[i];
        wstring input_path = full_path_q.toStdWString();
        
        // Генерируем путь вывода
        QFileInfo info(full_path_q);
        wstring output_path;
        if (file_manager_->isValidWorkingDir()) {
            output_path = file_manager_->generateOutputPath(filename_q.toStdWString());
        } else {
            // Сохраняем рядом с оригиналом с суффиксом _processed
            output_path = (info.absolutePath() + "/" + info.baseName() + "_processed." + info.suffix()).toStdWString();
        }
        
        logMessage(QString("📄 Обработка: %1").arg(full_path_q));
        
        // Создаём backup если нужно
        if (create_backup_checkbox_->isChecked()) {
            QFile::copy(full_path_q, full_path_q + ".backup");
            logMessage("  💾 Создана резервная копия");
        }
        
        // Читаем файл
        wstring content;
        if (!file_manager_->readFile(input_path, content)) {
            logMessage("  ❌ [ОШИБКА] Не удалось прочитать файл", LogLevel::ERROR);
            continue;
        }
        
        // Применяем модули
        if (cleaner_checkbox_->isChecked()) {
            auto result = cleaner_->processText(content);
            total_ops += result.operations_count;
            logMessage(QString("  🧹 [Cleaner] %1").arg(QString::fromStdWString(result.message)));
        }
        
        if (links_checkbox_->isChecked()) {
            auto result = links_->processText(content);
            total_ops += result.operations_count;
            logMessage(QString("  🔗 [Links] %1").arg(QString::fromStdWString(result.message)));
        }
        
        if (notes_checkbox_->isChecked()) {
            auto result = notes_->processText(content);
            total_ops += result.operations_count;
            logMessage(QString("  📝 [Notes] %1").arg(QString::fromStdWString(result.message)));
        }
        
        if (extractor_checkbox_->isChecked()) {
            auto result = extractor_->processText(content);
            total_ops += result.operations_count;
            logMessage(QString("  📊 [Extractor] %1").arg(QString::fromStdWString(result.message)));
        }
        
        // Записываем результат
        if (file_manager_->writeFile(output_path, content)) {
            logMessage(QString("  ✅ Сохранено: %1").arg(QString::fromStdWString(output_path)));
            processed_count++;
        } else {
            logMessage("  ❌ [ОШИБКА] Не удалось сохранить файл", LogLevel::ERROR);
        }
    }
    
    logMessage("════════════════════════════════════");
    logMessage(QString("✨ ОБРАБОТКА ЗАВЕРШЕНА"));
    logMessage(QString("   Обработано файлов: %1/%2").arg(processed_count).arg(selected_files.size()));
    logMessage(QString("   Всего операций: %1").arg(total_ops));
    logMessage("════════════════════════════════════");
    
    QMessageBox::information(this, "Готово", 
        QString("✅ Обработано файлов: %1 из %2\n🔧 Всего операций: %3")
        .arg(processed_count).arg(selected_files.size()).arg(total_ops));
}

void MainWindow::logMessage(const QString& msg, LogLevel level) {
    QString color;
    switch (level) {
        case LogLevel::ERROR: color = "#ff6b6b"; break;
        case LogLevel::WARNING: color = "#ffd93d"; break;
        case LogLevel::INFO: color = "#6bcf7f"; break;
        default: color = "#d4d4d4"; break;
    }
    
    log_text_->append(QString("<span style='color: %1;'>%2</span>").arg(color, msg));
}

void MainWindow::on_refreshFiles_clicked() {
    updateFileList();
}

void MainWindow::on_toggleLog_clicked() {
    if (log_container_->isVisible()) {
        // Скрываем логи
        log_container_->hide();
        toggle_log_button_->setText("▲ Показать логи");
    } else {
        // Показываем логи
        log_container_->show();
        toggle_log_button_->setText("▼ Скрыть логи");
    }
}

void MainWindow::on_clearLogButton_clicked() {
    log_text_->clear();
    logMessage("🔄 Лог очищен");
}

void MainWindow::on_fileList_itemSelectionChanged() {}

void MainWindow::on_cleanerCheckBox_toggled(bool checked) {
    configure_cleaner_button_->setEnabled(checked);
}

void MainWindow::on_linksCheckBox_toggled(bool checked) {
    configure_links_button_->setEnabled(checked);
}

void MainWindow::on_notesCheckBox_toggled(bool checked) {
    browse_report_button_->setEnabled(checked);
}

void MainWindow::on_extractorCheckBox_toggled(bool) {}

void MainWindow::on_configureCleanerButton_clicked() {
    QMessageBox::information(this, "Настройка Cleaner", "Диалог настройки правил\n(в разработке)");
}

void MainWindow::on_addFiles_clicked() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Добавить файлы", 
        QString(), "Все поддерживаемые (*.html *.txt);;HTML Files (*.html);;Text Files (*.txt)");
    
    if (files.isEmpty()) return;
    
    for (const QString& file_path : files) {
        // Проверяем, не добавлен ли уже
        bool exists = false;
        for (int i = 0; i < file_list_->count(); ++i) {
            if (file_list_->item(i)->data(Qt::UserRole).toString() == file_path) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            QFileInfo info(file_path);
            auto* item = new QListWidgetItem(info.fileName(), file_list_);
            item->setData(Qt::UserRole, file_path); // Сохраняем полный путь
            item->setCheckState(Qt::Checked);
            item->setToolTip(file_path);
            
            // Цвет по типу
            if (file_path.endsWith(".html", Qt::CaseInsensitive)) {
                item->setForeground(QColor("#6bcf7f"));
            } else {
                item->setForeground(QColor("#ffd93d"));
            }
        }
    }
    
    file_count_label_->setText(QString("Файлов: %1").arg(file_list_->count()));
    logMessage(QString("✅ Добавлено файлов: %1").arg(files.size()));
}

void MainWindow::on_removeFiles_clicked() {
    auto selected = file_list_->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Информация", "Выберите файлы для удаления");
        return;
    }
    
    for (auto* item : selected) {
        delete item;
    }
    
    file_count_label_->setText(QString("Файлов: %1").arg(file_list_->count()));
    logMessage(QString("🗑️ Удалено файлов: %1").arg(selected.size()));
}

void MainWindow::on_showHelp_clicked() {
    QString help_text = R"(
<h2>BookProcessor v2.0 - Руководство пользователя</h2>

<h3>Общее описание</h3>
<p><b>BookProcessor</b> - программа для автоматической обработки HTML-книг перед публикацией. 
Объединяет 4 модуля обработки в единый интерфейс с графическим интерфейсом.</p>

<h3>Быстрый старт</h3>
<ol>
<li><b>Выберите способ работы:</b>
<ul>
<br><b>Рабочая папка:</b> Укажите папку с книгой - программа найдёт все .html/.txt файлы
<br><b>Ручная загрузка:</b> Добавьте файлы по одному через "+Добавить файлы"
</ul>
</li>
<li><b>Укажите выходную папку:</b> Где сохранить обработанные файлы (по умолчанию: рабочая_папка/output)</li>
<li><b>Выберите модули:</b> Отметьте нужные (Cleaner, Links, Notes, Extractor)</li>
<li><b>Настройте опции:</b> Резервные копии, diff, логирование</li>
<li><b>Отметьте файлы:</b> В списке файлов отметьте чекбоксами те, которые нужно обработать</li>
<li><b>Запустите:</b> Нажмите "Обработать файлы"</li>
</ol>

<h3>Работа с файлами</h3>
<p><b>Способ 1 - Рабочая папка:</b></p>
<ul>
<li>Нажмите "Выбрать папку" и укажите папку с книгой</li>
<li>Программа автоматически найдёт все .html и .txt файлы в подпапках</li>
<li>Выходная папка установится автоматически как "рабочая_папка/output"</li>
<li>Можно выбрать другую выходную папку кнопкой "Выбрать..."</li>
</ul>

<p><b>Способ 2 - Ручная загрузка:</b></p>
<ul>
<li>Оставьте поле "Рабочая папка" пустым</li>
<li>Нажмите "+Добавить файлы" и выберите нужные файлы</li>
<li>Результаты сохранятся рядом с оригиналом с суффиксом "_processed"</li>
<li>Выходная папка игнорируется при ручной загрузке</li>
</ul>

<p><b>Обновление списка:</b> Нажмите "Обновить" чтобы пересканировать рабочую папку</p>

<h3>🧹 Модуль Cleaner</h3>
<p><b>Назначение:</b> Очистка HTML от мусора, исправление форматирования</p>
<p><b>Что делает:</b></p>
<ul>
<li>Удаляет лишние пробелы (&#160;, &#173;, &#9;)</li>
<li>Исправляет сокращения (т. д. → т.д., т. п. → т.п.)</li>
<li>Убирает ненужные теги &lt;br /&gt;</li>
<li>Удаляет CSS-классы (Osnova, _idFootnoteLink и т.д.)</li>
<li>Очищает динамические классы (ParaOverride-XXX)</li>
</ul>
<p><b>Настройка:</b> Нажмите "Настроить..." для редактирования правил (в разработке)</p>

<h3>Модуль Links</h3>
<p><b>Назначение:</b> Преобразование голых URL в кликабельные ссылки</p>
<p><b>Что делает:</b></p>
<ul>
<li>Находит URL: http://, https://, ftp://, mailto:</li>
<li>Оборачивает в &lt;a href="..."&gt;...&lt;/a&gt;</li>
<li>Не трогает уже существующие ссылки</li>
<li>Учитывает терминаторы (пробел, запятая, скобки)</li>
</ul>
<p><b>Настройка:</b> Нажмите "Настроить..." для изменения протоколов и терминаторов (в разработке)</p>

<h3>Модуль Notes</h3>
<p><b>Назначение:</b> Вставка сносок (footnotes/endnotes) по отчёту</p>
<p><b>Как использовать:</b></p>
<ol>
<li>Отметьте чекбокс "Notes - обработка сносок"</li>
<li>Нажмите "Файл отчёта..." и выберите CSV/TXT файл отчёта</li>
<li>Формат отчёта: source_file,line,backlink_id,display_number,target,footnote_id,exists</li>
<li>Программа обработает все сноски из отчёта</li>
</ol>
<p><b>Поддерживаемые типы:</b> footnote, endnote</p>

<h3>Модуль Extractor</h3>
<p><b>Назначение:</b> Извлечение метаданных (команда создателей книги)</p>
<p><b>Что делает:</b></p>
<ul>
<li>Находит начало текста (после "Все права защищены")</li>
<li>Находит конец (перед списком должностей)</li>
<li>Извлекает информацию о команде работавших над книгой</li>
</ul>

<h3>⚙️ Опции</h3>
<p><b>Создать резервные копии:</b> Сохраняет оригинал как .backup перед обработкой</p>
<p><b>Показать diff:</b> Отображает построчные изменения (зелёное - добавлено, красное - удалено) - пока не реализовано</p>
<p><b>Подробное логирование:</b> Выводит детальную информацию в лог</p>
<p><b>🔽/🔼 Скрыть/Показать логи:</b> Управление видимостью области логов</p>


<h3>❗ Частые вопросы</h3>
<p><b>Q: Где сохраняются результаты?</b></p>
<p>A: <br>При работе с папкой: в указанную выходную папку (по умолчанию: рабочая_папка/output)
<br>При ручной загрузке: рядом с оригиналом как "имя_processed.расширение"
</p>

<p><b>Q: Почему файл не обрабатывается?</b></p>
<p>A: Убедитесь, что файл отмечен чекбоксом в списке файлов</p>

<p><b>Q: Как отменить изменения?</b></p>
<p>A: Используйте резервные копии (.backup файлы) или восстановите из оригинала</p>

<p><b>Q: Модули настройки не работают?</b></p>
<p>A: Диалоги настройки находятся в разработке. Используйте модули с настройками по умолчанию</p>

<hr>
<p style="text-align: center;"><i>BookProcessor v2.0 © 2025 | Разработано для профессиональной обработки книг</i></p>
)";
    
    QDialog* help_dialog = new QDialog(this);
    help_dialog->setWindowTitle("Помощь - BookProcessor v2.0");
    help_dialog->resize(800, 600);
    
    auto* layout = new QVBoxLayout(help_dialog);
    auto* text_browser = new QTextEdit(help_dialog);
    text_browser->setReadOnly(true);
    text_browser->setHtml(help_text);
    
    auto* close_button = new QPushButton("Закрыть", help_dialog);
    connect(close_button, &QPushButton::clicked, help_dialog, &QDialog::accept);
    
    layout->addWidget(text_browser);
    layout->addWidget(close_button);
    
    help_dialog->exec();
}

void MainWindow::on_configureLinksButton_clicked() {
    QMessageBox::information(this, "Настройка Links", "Диалог настройки протоколов и терминаторов\n(в разработке)");
}

void MainWindow::on_browseReportButton_clicked() {
    QString file = QFileDialog::getOpenFileName(this, "Выберите файл отчёта", QString(), "Text Files (*.txt *.csv)");
    if (!file.isEmpty()) {
        if (notes_->loadReport(file.toStdWString())) {
            logMessage("✅ Отчёт загружен: " + file);
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось загрузить файл отчёта");
        }
    }
}

} // namespace BookProcessor
