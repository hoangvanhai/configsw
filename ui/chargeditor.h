#ifndef CHARGEDITOR_H
#define CHARGEDITOR_H

#include <QWidget>
#include <thread>
#include <mutex>
#include "xlsx/xlsxdocument.h"
#include <QObject>
#include <QDate>
#include <QDir>
#include <QStandardItemModel>
#include <QWidget>
#include <QtWidgets>
#include <QMainWindow>
#include <ibc/framing/layer2.h>
#include <config/config.h>
#include <QtCharts>



class LineEditor;

class ChargEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit ChargEditor(QWidget *parent = nullptr);
    void initVariable();
    void createElement();
    void createLayout();
    void createContent();
    void createConnection();

signals:

public slots:
    void onBtnAddPoint();
    void onBtnRemPoint();


private:
    QChartView          *chartView;
    QChart              *chart;
    QLineSeries         *iSeries, *vSeries;
    QVector<LineEditor*> listEditor;
    QVBoxLayout         *layoutEditor;
    QSplitter           *hSplitter;
    QSplitter           *vSplitter;

    QGroupBox           *groupEditor, *groupControl;

    QPushButton         *btnAddPoint, *btnRemPoint;

    int                 numLine;

};

class LineEditor : public QWidget
{

Q_OBJECT
public:

    explicit LineEditor(int id = 0, QWidget *parent = nullptr);
    QComboBox *comType;
    QDoubleSpinBox  *spCurrent, *spVoltage, *spTimeMax;
};


#endif // CHARGEDITOR_H
