# 📦 Создание установщика BookProcessor

## Требования

1. **Inno Setup 6** - скачать с https://jrsoftware.org/isdl.php
2. **Собранный проект** - `BookProcessor.exe` в папке `build/`
3. **Qt 6.9.1 MinGW** - установлен в `D:\Qt\6.9.1\mingw_64\`

## Шаги

### 1. Соберите проект

```powershell
cd D:\CookPrograms\AllProg\BookProcessor\build
cmake --build . --config Release
```

### 2. ⚠️ ОБЯЗАТЕЛЬНО: Разверните Qt DLL

**Это критический шаг!** Без него приложение не запустится на компьютерах пользователей.

```powershell
# Перейдите в папку с exe
cd D:\CookPrograms\AllProg\BookProcessor\build

# Запустите windeployqt для автоматического копирования всех необходимых Qt библиотек
D:\Qt\6.9.1\mingw_64\bin\windeployqt.exe BookProcessor.exe
```

**Что делает windeployqt:**
- Копирует все необходимые Qt DLL (Qt6Core, Qt6Gui, Qt6Widgets, Qt6Network, Qt6Svg)
- Копирует MinGW runtime (libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll)
- Копирует OpenGL библиотеки (opengl32sw.dll, d3dcompiler_47.dll)
- Создаёт папки с плагинами (platforms/, iconengines/, imageformats/, styles/, tls/)
- Копирует переводы Qt (translations/)

**Результат:** Папка `build/` теперь содержит ~50MB файлов вместо 4MB - это нормально!

### 3. Создайте установщик

1. Откройте Inno Setup
2. Выберите `File → Open` и откройте `build\BookProcessor_Installer.iss`
3. Нажмите `Build → Compile`
4. Готовый установщик появится в папке `build\`

## Альтернатива - Портативная версия

Если не нужен установщик, создайте ZIP-архив:

```powershell
# Перейдите в build (где уже выполнен windeployqt)
cd D:\CookPrograms\AllProg\BookProcessor\build

# Скопируйте config
Copy-Item -Recurse ..\config .

# Создайте архив всей папки
Compress-Archive -Path * -DestinationPath ..\BookProcessor_v2.0_Portable.zip
```

Готово! Пользователь просто распакует ZIP и запустит.

## Структура установленной программы

После установки или распаковки:

```
C:\Program Files\BookProcessor\  (или любая папка)
├── BookProcessor.exe              (4.4 MB - главный исполняемый файл)
├── Qt6Core.dll                    (~6 MB)
├── Qt6Gui.dll                     (~7 MB)
├── Qt6Widgets.dll                 (~4 MB)
├── Qt6Network.dll                 (~2 MB)
├── Qt6Svg.dll                     (~1 MB)
├── libgcc_s_seh-1.dll            (MinGW runtime)
├── libstdc++-6.dll               (MinGW C++ standard library)
├── libwinpthread-1.dll           (MinGW threads)
├── opengl32sw.dll                (Software OpenGL)
├── d3dcompiler_47.dll            (DirectX shader compiler)
├── dxcompiler.dll, dxil.dll      (DirectX 12)
├── platforms/
│   └── qwindows.dll              (Windows platform plugin)
├── iconengines/
│   └── qsvgicon.dll              (SVG icon support)
├── imageformats/
│   ├── qgif.dll, qico.dll
│   ├── qjpeg.dll, qsvg.dll
├── styles/
│   └── qmodernwindowsstyle.dll   (Modern Windows style)
├── tls/
│   ├── qcertonlybackend.dll
│   └── qschannelbackend.dll      (Secure connections)
├── translations/                  (Qt UI переводы - 29 языков)
│   ├── qt_ru.qm                  (Русский перевод)
│   ├── qt_en.qm, qt_de.qm, ...
└── config/
    └── default_config.json        (Настройки по умолчанию)
```

**Общий размер:** ~50-60 MB (большую часть занимают Qt библиотеки)

## Устранение проблем

### ❌ Ошибка: "Не удается найти Qt6Core.dll"
**Причина:** Не выполнен шаг 2 (windeployqt)  
**Решение:** Запустите `windeployqt` в папке с exe:
```powershell
D:\Qt\6.9.1\mingw_64\bin\windeployqt.exe BookProcessor.exe
```

### ❌ Ошибка: "Не удалось загрузить платформу Windows"
**Причина:** Отсутствует папка `platforms/` или `qwindows.dll`  
**Решение:** Запустите `windeployqt` - он создаст все нужные папки

### ❌ Приложение запускается в VS Code, но не на другом ПК
**Причина:** VS Code использует launch.json с прописанными путями к Qt DLL  
**Решение:** Для распространения **обязательно** используйте `windeployqt`

### ✅ Проверка правильности развёртывания
После `windeployqt` в папке с exe должно быть:
- ✅ ~15 DLL файлов (Qt6*.dll, lib*.dll, opengl*.dll)
- ✅ 7 папок (platforms, iconengines, imageformats, networkinformation, styles, tls, translations)
- ✅ Общий размер папки ~50-60 MB

## Почему раньше работало без windeployqt?

При запуске через VS Code (F5) используется файл `.vscode/launch.json`, где прописана переменная окружения PATH:

```json
"environment": [
  {
    "name": "PATH",
    "value": "D:\\Qt\\6.9.1\\mingw_64\\bin;D:\\Qt\\Tools\\mingw1310_64\\bin;%PATH%"
  }
]
```

Это позволяет приложению находить Qt DLL во время отладки, но **не работает** при запуске напрямую через Проводник или на другом компьютере.

**Ошибка: "Entry point not found"**
- Убедитесь, что используете MinGW-версии DLL, а не MSVC

**Программа не запускается на другом ПК**
- Убедитесь, что установлены MinGW runtime DLL (libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll)

## Тестирование установщика

1. Запустите `BookProcessorSetup_2.0.exe`
2. Установите в тестовую папку
3. Проверьте запуск из меню Пуск
4. Проверьте обработку файлов
5. Удалите через Панель управления

## Примечания

- InnoSetup бесплатен и не требует дополнительных лицензий
- Размер установщика: ~50MB (благодаря сжатию LZMA)
- Поддержка Windows 7/8/10/11
