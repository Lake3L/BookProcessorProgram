#include <QApplication>
#include "MainWindow.h"
#include "Logger.h"

using namespace BookProcessor;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Установка иконки приложения
    app.setWindowIcon(QIcon(":/app_icon.ico"));

    // Инициализация логгера
    Logger::instance().setLogFile(L"logs/bookprocessor.log");
    Logger::instance().setMinLevel(LogLevel::INFO);

    LOG_INFO(L"Main", L"BookProcessor v2.0 запущен");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
