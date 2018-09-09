#ifndef DEVELOP_H
#define DEVELOP_H

#include <QtWidgets>
#include <QTimer>
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

signals:

    void signalConnectionEvent(int event);
    void signalChargerData(uint8_t *data,
                         int data_len);
private slots:
    void openConnection();
    void closeConnection();
    void showControlPanel();
    void recvChargerDataEvent(uint8_t *data_, int data_len_);
    void recvConnectionEvent(int event);
    void onTimerClb();

public slots:
    void onSendData(QString data);

private:

    QToolBar *toolBar;
    QVBoxLayout *mainLayout;
    QWidget *centerWidget;

    QToolButton *btnOpenConnection, *btnCloseConnection,
                *btnShowControlPanel;

    ControlDock *control;

    QToolButton *btnClear, *btnSave;

    QSplineSeries *series;
    QChart *ivChart;
    QValueAxis *ivXAxis;


    std::shared_ptr<ibc::layer2> ibc_obj_;

    QTimer  hTimerControl;

};



class ControlDock : public QDockWidget {
    Q_OBJECT
public:
    explicit ControlDock(const QString &name, QWidget *parent = 0);

    void createElement();
    void createLayout();
    void createContent();
    void createConnection();

signals:
    void sigSendData(QString);

private:
    void sendBoarId(int id);
    void sendFloatVolt(double value);
    void sendBoostVolt(double value);
    void sendBoostCurr(double value);
    void sendBoostTime(double value);
    void sendVusb(bool value);
    void sendAllParam(int id, double fvolt, double bvolt,
                      double bcurr, double btime, bool vusb);
    void sendCommand(const QString &cmd);

private slots:
    void onBtnSetId();
    void onBtnSetFloatVolt();
    void onBtnSetBoostVolt();
    void onBtnSetBoostCurr();
    void onBtnSetBoostTime();
    void onBtnSetVusb();
    void onBtnSetAll();
    void onBtnGetAll();
    void onBtnSetDef();
    void onRecvData(uint8_t *data_, int data_len_);

private:     
    QDoubleSpinBox  *spinFloatVolt, *spinBoostVolt, *spinBoostCurrent,
                    *spinBoostTime, *spinBoardID;

    QPushButton *btnFloatVolt, *btnBoostVolt, *btnBoostCurr,
                *btnBoostTime, *btnBoardID, *btnVusb,
                *btnSetAll, *btnGetAllCfg, *btnDefault;

    QCheckBox   *checkVusb;

    QLineEdit   *editChargerStatus, *editBattVolt,
                *editBattCurr, *editBattCap;

    QTextEdit  *editStatus;
    QVBoxLayout *mainLayout;
    QWidget *centerWidget;
};


#endif // DEVELOP_H
