#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>


void setTheme(QApplication *app, QString path) {
    QFile styleSheet(path);
    if (!styleSheet.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "could not open file";
    }
    QTextStream in(&styleSheet);
    QString setSheet = in.readAll();
    app->setStyleSheet(setSheet);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //setTheme(&a, ":/blackstyle.css");
    MainWindow w;
    w.show();
    return a.exec();
}
