#ifndef TOOL_H
#define TOOL_H

#include <QtWidgets>

class Tool : public QMainWindow
{
    Q_OBJECT
public:
    explicit Tool(QWidget *parent = 0);
    void initVariable();
    void createElement();
    void createContent();
    void createLayout();
    void createConnection();

signals:

public slots:
};

#endif // TOOL_H
