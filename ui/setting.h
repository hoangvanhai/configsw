#ifndef SETTING_H
#define SETTING_H

#include <QtWidgets>
#include <slidingstackedwidget.h>
#include <config/config.h>
#include <serialport.h>
#include <hardware.h>
#include <permission.h>


class Setting : public QMainWindow
{
    Q_OBJECT
public:
    explicit Setting(QWidget *parent = 0);
    void initVariable();
    void createElement();
    void createIcons();
    void createLayout();
    void createConnection();

signals:

public slots:
    void showMessage(QString msg);

private:
    QHBoxLayout     *hLayout;
    QVBoxLayout     *mainLayout;
    SlidingStackedWidget *stacker;
    QStatusBar      *status;
    QListWidget     *contentsWidget;

    QListWidgetItem *serial;
    QListWidgetItem *harware;
    QListWidgetItem *permiss;

};



#endif // SETTING_H
