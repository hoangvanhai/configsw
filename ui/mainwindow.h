#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QtWidgets>
#include <develop.h>
#include <setting.h>
#include <chargeditor.h>
#include <layer2.h>

class HelpWindow;

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
    void updateTheme();
private:
    QTabWidget *tabWidget;
    QWidget *centerWidget;
    QVBoxLayout *mainLayout;
    HelpWindow  *helpWindow;

    Setting     *setting;
    Develop     *develop;    
    ChargEditor *editor;

    QComboBox   *comTheme;

    QPushButton *min, *max, *close, *help;

    std::shared_ptr<ibc::layer2> ibc_obj_;

    // frameless windows implement
signals:

    void minimize();
    void maximize();
    void quit();

private slots:
    void openHelp();
    void closeWindow() {QApplication::quit(); }
    void minimizeWindow() {setWindowState(Qt::WindowMinimized);}
    void maximizeWindow();

private:

    bool isMouseDown = false;
    int mouseOffsetX = 0, mouseOffsetY = 0;
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    QPoint mousePoint;
};


class HelpWindow : public QWidget
{
    Q_OBJECT
public:
    explicit HelpWindow(QWidget *parent = 0);
    void closeEvent(QCloseEvent *event);
signals:

public slots:
};

#endif // MAINWINDOW_H
