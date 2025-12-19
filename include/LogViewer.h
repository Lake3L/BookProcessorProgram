#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QWidget>
#include "Types.h"

namespace BookProcessor {

class LogViewer : public QWidget {
    Q_OBJECT

public:
    explicit LogViewer(QWidget *parent = nullptr);
    
    void addLogEntry(const wstring& message, LogLevel level);
    void clear();
    void exportToFile(const wstring& path);

private:
    // TODO: QTextEdit для отображения логов
};

} // namespace BookProcessor

#endif // LOGVIEWER_H
