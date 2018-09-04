#ifndef PROGRAM_H
#define PROGRAM_H

#include <QtWidgets>

class Program : public QMainWindow
{
    Q_OBJECT
public:
    explicit Program(QWidget *parent = 0);
    void initVariable();
    void createElement();
    void createContent();
    void createLayout();
    void createConnection();

signals:

public slots:
};

#endif // PROGRAM_H
