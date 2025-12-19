# BookProcessor v2.0

Интегрированная система обработки HTML-книг.

## Структура проекта

```
BookProcessor/
├── src/
│   ├── Core/           # Ядро: Logger, PatternEngine, FileManager
│   ├── Modules/        # Модули обработки
│   └── GUI/            # Графический интерфейс
├── include/            # Заголовочные файлы
├── config/             # Конфигурационные JSON
└── CMakeLists.txt
```

## Модули

1. **CleanerModule** - Очистка HTML от лишних символов и тегов
2. **LinksModule** - Преобразование URL в кликабельные ссылки
3. **NotesModule** - Обработка сносок по отчёту
4. **ExtractorModule** - Извлечение метаданных

## Сборка

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Использование

1. Выберите рабочую папку
2. Поместите файлы в `input/`
3. Настройте модули
4. Нажмите "Обработать"
5. Результаты в `output/`

## Возможности

- ✅ Все правила из GUI-версии Replacements
- ✅ Портированный LinkCorrection с Pascal
- ✅ Улучшенная обработка сносок
- ✅ Файловый браузер с корневой папкой
- ✅ Автосоздание output/config/logs/
- ✅ Детальное логирование
- ✅ Модульная архитектура

## Автор

BookProcessor Team, 2025
