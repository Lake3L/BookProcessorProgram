# Инструкция по установке MSVC версии Qt6

## Вариант 1: Через Qt Maintenance Tool

1. Запустите `D:\Qt\MaintenanceTool.exe`
2. Выберите "Add or remove components"
3. Разверните Qt → Qt 6.9.1
4. Отметьте: 
   - **MSVC 2022 64-bit** (для Visual Studio 2022)
   - или **MSVC 2019 64-bit** (для Visual Studio 2019)
5. Нажмите "Next" и дождитесь установки

После установки путь будет:
- `D:/Qt/6.9.1/msvc2022_64/lib/cmake/Qt6`

## Вариант 2: Использовать MinGW (текущая настройка)

Уже настроено! Просто перезагрузите VS Code и выберите Kit.

---

## Проверка установки Visual Studio

Для MSVC нужна Visual Studio с C++ инструментами:

```powershell
# Проверить наличие cl.exe
where.exe cl.exe

# Если нет, установите:
# Visual Studio Community 2022 + "Desktop development with C++"
```

## Рекомендация

**Используйте MinGW** (уже настроено) - проще и не требует Visual Studio.
