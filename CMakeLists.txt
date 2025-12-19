cmake_minimum_required(VERSION 3.16)
project(BookProcessor VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Указываем путь к Qt6 для MinGW
set(CMAKE_PREFIX_PATH "D:/Qt/6.9.1/mingw_64")

# Поиск Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Исходные файлы ядра
set(CORE_SOURCES
    src/Core/Logger.cpp
    src/Core/PatternEngine.cpp
    src/Core/FileManager.cpp
    src/Core/ConfigManager.cpp
)

set(CORE_HEADERS
    include/Logger.h
    include/PatternEngine.h
    include/FileManager.h
    include/ConfigManager.h
    include/Types.h
)

# Исходные файлы модулей
set(MODULE_SOURCES
    src/Modules/CleanerModule.cpp
    src/Modules/NotesModule.cpp
    src/Modules/LinksModule.cpp
    src/Modules/ExtractorModule.cpp
)

set(MODULE_HEADERS
    include/CleanerModule.h
    include/NotesModule.h
    include/LinksModule.h
    include/ExtractorModule.h
)

# Исходные файлы GUI
set(GUI_SOURCES
    src/GUI/MainWindow.cpp
    src/GUI/ConfigDialog.cpp
    src/GUI/LogViewer.cpp
    src/main.cpp
)

set(GUI_HEADERS
    include/MainWindow.h
    include/ConfigDialog.h
    include/LogViewer.h
)

# Ресурсы
set(RESOURCES
    resources/resources.qrc
)

# Объединение всех файлов
add_executable(${PROJECT_NAME} WIN32
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${MODULE_SOURCES}
    ${MODULE_HEADERS}
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${RESOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Копирование конфигурационных файлов
file(COPY config/ DESTINATION ${CMAKE_BINARY_DIR}/config/)

# Установка кодировки для Windows
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()
