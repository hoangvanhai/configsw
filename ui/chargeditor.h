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
#include <definition.h>


class LineEditor;

struct NodeInfo {
    int     id;
    int     type;
    double  param1;
    double  param2;
    double  param3;
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
    void sendRawData(const std::string &cmd);
    void sendProtoData(uint8_t subcmd, const uint8_t *data, int len, bool ack = false);
    double getParam1Max();
    double getParam2Max();

    void sendReadConfig();
    void sendWriteConfig();



signals:
    void signalConnectionEvent(int event);
    void signalChargerData(const QString &string);    
    void chargerRawData(const uint8_t* data, int len);
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
    void recvRawData(const uint8_t *data, int len);
    void onEnablePanel(bool en);

    void onBtnRead();
    void onBtnWrite();

private:
    QChartView          *chartView;
    QChart              *chart;
    QLineSeries         *iSeries, *vSeries;
    QValueAxis          *axisYCurr, *axisYVolt, *axisX;
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
    explicit LineEditor(int id = 0, int type = 0, double param1 = 0, double param2 = 0,
                        double param3 = 0, QWidget *parent = nullptr);

    void createElement();
    void createContent();
    void setInputHidden(bool hide);


    void    setId(int id);
    int     getId() const;
    void    setType(int type);
    int     getType() const;
    void    setParam1(double value);
    double  getParam1() const;
    void    setParam2(double value);
    double  getParam2() const;
    void    setParam3(double value);
    double  getParam3() const;

signals:
    void    changedValue();

public slots:
    void    onChangedValue();    
    void    updateLayout(int type);

 private:
    QLabel          *lbParam1, *lbParam2, *lbParam3;
    QComboBox       *comType;
    QSpinBox        *rowId;
    QDoubleSpinBox  *spParam1, *spParam2, *spParam3;
    int     id_;
};




#endif // CHARGEDITOR_H
