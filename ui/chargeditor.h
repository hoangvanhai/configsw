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
#include <csvfile.h>
#include <QtCharts>



class LineEditor;

struct NodeInfo {
    int     id;
    int     type;
    double  current;
    double  voltage;
    double  time;
};

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
    bool loadExportData(const QString &file);
    void drawChart();
    void importDataFromFile(const QString &file);
    void exportDataToFile(const QString &file);
    void printNodeInfo(const NodeInfo &node);
    bool loadImportData(const QString &file);
    void writeRowToFile(const std::vector<std::string> row);
    void closeFile();    
    void setCommunication(std::shared_ptr<ibc::layer2> conn);
    void sendData(const std::string &cmd);

signals:
    void signalConnectionEvent(int event);
    void signalChargerData(const QString &string);    
public slots:
    void onBtnAddPoint();
    void onBtnRemPoint();
    void onBtnSelectExport();
    void onBtnSelectImport();
    void onBtnImport();
    void onBtnExport();
    void updateChart();
    void onBtnOpenConnection();
    void onBtnCloseConnection();
    void recvConnectionEvent(int event);
    void recvChargerDataEvent(const QString &str);
    void onEnablePanel(bool en);

private:
    QChartView          *chartView;
    QChart              *chart;
    QLineSeries         *iSeries, *vSeries;
    QVector<LineEditor*> listEditor;
    QVector<NodeInfo>   listData;
    QVBoxLayout         *layoutEditor;
    QSplitter           *hSplitter;
    QSplitter           *vSplitter;

    QGroupBox           *groupEditor, *groupControl, *groupComm;
    QPushButton         *btnAddPoint, *btnRemPoint;

    QPushButton         *btnConnect, *btnDisconnect, *btnBrowFileImport,
                        *btnBrowFileExport, *btnImport, *btnExport;
    QToolButton         *btnWrite, *btnRead;

    QLineEdit           *editFileImport, *editFileExport, *editDevInfo,
                        *editDevSerial, *editDevModel, *editImportModel;
    QString             filePath;
    int                 numLine;

    std::shared_ptr<CSVFile> loader;

    std::shared_ptr<ibc::layer2> ibc_obj_;

};

class LineEditor : public QWidget
{

Q_OBJECT
public:

    explicit LineEditor(int id = 0, QWidget *parent = nullptr);
    void setId(int id);
    int getId() const;
    void setType(int idx);
    int getType() const;
    void setCurrent(double value);
    double getCurrent() const;
    void setVoltage(double value);
    double getVoltage() const;
    void setTime(double time);
    double getTime() const;

signals:
    void changedValue();

public slots:
    void onChangedValue();
 private:
    QComboBox *comType;
    QDoubleSpinBox  *spCurrent, *spVoltage, *spTimeMax;    
    int     id_;
};




#endif // CHARGEDITOR_H
