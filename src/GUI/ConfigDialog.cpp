#include "ConfigDialog.h"

namespace BookProcessor {

ConfigDialog::ConfigDialog(std::shared_ptr<PatternEngine> engine, QWidget *parent)
    : QDialog(parent), engine_(engine) {
    setWindowTitle("Настройка правил");
    // TODO: Создание GUI
}

void ConfigDialog::on_addRule_clicked() {
    // TODO: Добавление правила
}

void ConfigDialog::on_removeRule_clicked() {
    // TODO: Удаление правила
}

void ConfigDialog::on_testRule_clicked() {
    // TODO: Тестирование правила
}

void ConfigDialog::on_saveConfig_clicked() {
    // TODO: Сохранение в JSON
}

} // namespace BookProcessor
