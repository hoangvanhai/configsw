#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QtWidgets>
#include <develop.h>
#include <program.h>
#include <setting.h>
#include <tool.h>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initVariable();
    void createElement();
    void createContent();
    void createLayout();
    void createConnection();
    void createTitleButton();

public slots:
    void close() {qApp->quit();}
private:
    QTabWidget *tabWidget;
    QWidget *centerWidget;
    QVBoxLayout *mainLayout;


    Setting     *setting;
    Develop     *develop;
    Program     *program;
    Tool        *tool;
};

#endif // MAINWINDOW_H
