# Changelog

## v2.0.1 (19.12.2025)

### Исправления

**Критическое исправление работы с файлами на Windows:**

- ✅ Заменен `std::wifstream/wofstream` на `QFile` и `QTextStream` во всех модулях
- ✅ Исправлена проблема с путями Windows в `FileManager::readFile()` и `writeFile()`
- ✅ Исправлена проблема с путями в `NotesModule::loadReport()`
- ✅ Исправлена проблема с путями в `Logger::setLogFile()` и `exportLog()`
- ✅ Исправлено отображение полного пути файла в логе обработки

### Проблема

При использовании `wifstream::open(wstring.c_str())` на Windows возникали ошибки при открытии файлов:
- Неправильная обработка путей с дисками (`D:/...`)
- Ошибки кодировки UTF-8
- Проблемы с широкими строками в Windows API

### Решение

Замена всех операций чтения/записи файлов на Qt API:
- `QFile` - для операций с файлами
- `QTextStream` - для работы с текстом
- `QStringConverter::Utf8` - для правильной обработки UTF-8

### Технические детали

**Файлы изменены:**
- `src/Core/FileManager.cpp` - readFile(), writeFile()
- `src/Core/Logger.cpp` - setLogFile(), exportLog(), log()
- `src/Modules/NotesModule.cpp` - loadReport()
- `include/Logger.h` - изменен тип log_file_ с wofstream на QFile*
- `src/GUI/MainWindow.cpp` - исправлено отображение пути в логе

**Преимущества нового подхода:**
1. Правильная работа с Windows путями
2. Корректная обработка UTF-8 на всех платформах
3. Единообразное использование Qt API
4. Отсутствие проблем с locale и codecvt

## v2.0.0 (19.12.2025)

Первый релиз BookProcessor v2.0 с Qt GUI.
