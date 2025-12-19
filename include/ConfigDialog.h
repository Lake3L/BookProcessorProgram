#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "PatternEngine.h"
#include <memory>

namespace BookProcessor {

class ConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConfigDialog(std::shared_ptr<PatternEngine> engine, QWidget *parent = nullptr);
    
private slots:
    void on_addRule_clicked();
    void on_removeRule_clicked();
    void on_testRule_clicked();
    void on_saveConfig_clicked();

private:
    std::shared_ptr<PatternEngine> engine_;
    // TODO: Список правил с checkbox, таблица, кнопки
};

} // namespace BookProcessor

#endif // CONFIGDIALOG_H
