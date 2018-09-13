#ifndef DEVELOP_H
#define DEVELOP_H

#include <QtWidgets>
#include <QTimer>
#include <QtCharts>
#include <xlsx/xlsxdocument.h>
#include <layer2.h>
#include <config/config.h>
#include <QSerialPort>

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
    void setVusbEnable(bool en);
    void setRealtimeLog(bool en);
    void sendData(const std::string &cmd);

    void setCommunication(std::shared_ptr<ibc::layer2> conn);

signals:

    void signalConnectionEvent(int event);
    void signalChargerData(const QString &string);    
private slots:
    void updateChart(qreal current, qreal voltage);
    void onBtnOpenConnection();
    void onBtnCloseConnection();
    void onBtnCheckVusb();
    void onBtnCheckLog();
    void onBtnClear();
    void recvChargerDataEvent(const QString &string);
    void recvConnectionEvent(int event);        
    void onEnablePanel(bool en);
    void onTimeout();
private:

    QToolBar *toolBar;
    QVBoxLayout *mainLayout;
    QGroupBox *mainGroup;
    QSplitter *hSplitter;

    QToolButton *btnOpenConnection,
                *btnCloseConnection;



    QSplineSeries   *iSeries;
    QSplineSeries   *vSeries;

    QChart          *chart;
    QChartView      *chartView;

    std::shared_ptr<ibc::layer2> ibc_obj_;

    QTimer  hTimerControl;
    QTimer  hTimerTest;

    QGroupBox *groupStatus, *groupControl;

    QLineEdit       *editSTT, *editID;
    QDoubleSpinBox  *editPV, *editPI, *editPP,
                    *editBV, *editBI, *editBC;

    QToolButton     *btnClear;
    QCheckBox       *checkVusb, *checkLogData;



    QString testString;
    QString controlString;
    qreal       span;
    qreal       count;

};




#endif // DEVELOP_H
