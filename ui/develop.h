#ifndef DEVELOP_H
#define DEVELOP_H

#include <QtWidgets>
#include <QtCharts>
#include <xlsx/xlsxdocument.h>
#include <layer2.h>
#include <config/config.h>


class StreamDock;
class ControlDock;

class Develop : public QMainWindow
{
    Q_OBJECT
public:
    explicit Develop(QWidget *parent = 0);
    ~Develop();
    void initVariable();
    void createElement();
    void createContent();
    void createLayout();
    void createConnection();
    void createTableViewHeader();
    void initExcelHeader();
    void appendRow(QStringList &row);
    void writeRow(QStringList &row);    
signals:
    void signalStreamEvent(int event);
    void signalStreamData(uint8_t *data_,
                         int data_len_);
    void signalChartEvent(int event);
    void signalChartData(uint8_t *data,
                         int data_len);
private slots:
    void openChart();
    void closeChart();    
    void showChart();
    void sendChartData(QString);
    void recvChartDataEvent(uint8_t *data_,
                       int data_len_);

    void recvChartEvent(int event);

private:

    QToolBar *toolBar;
    QVBoxLayout *mainLayout;
    QWidget *centerWidget;

    QToolButton *btnOpenChart, *btnCloseChart, *btnShowChart;

    ControlDock *control;

    QToolButton     *btnClear, *btnSave;

    std::shared_ptr<ibc::layer2> ibc_chart_;

    QXlsx::Document *msgFile;
    uint32_t        current_row;
};



class ControlDock : public QDockWidget {
    Q_OBJECT
public:
    explicit ControlDock(const QString &name, QWidget *parent = 0);

    void createElement();
    void createLayout();
    void createConnection();

private:     
    QDoubleSpinBox  *spinFloatVolt, *spinBoostVolt, *spinBoostCurrent,
                    *spinBoostTime, *spinBoardID;

    QPushButton *btnFloatVolt, *btnBoostVolt, *btnBoostCurr,
                *btnBoostTime, *btnBoardID, *btnVusb,
                *btnSetAll, *btnGetAllCfg;

    QCheckBox   *checkVusb;

    QLineEdit   *editChargerStatus, *editBattVolt,
                *editBattCurr, *editBattCap;

    QTextEdit  *editStatus;

    std::shared_ptr<ibc::layer2> ibc_chart_;

    QVBoxLayout *mainLayout;
    QWidget *centerWidget;

private slots:
    void clearChartScreen();
    void saveChartScreen();

public slots:
    void recvUpdateChart(uint8_t *data, int len);

};


#endif // DEVELOP_H
