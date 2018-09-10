#ifndef CONTROLDOCK_H
#define CONTROLDOCK_H


#include <QtWidgets>
#include <QTimer>
#include <QtCharts>
#include <xlsx/xlsxdocument.h>
#include <layer2.h>
#include <config/config.h>
#include <QSerialPort>

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
    void showStatusBar(const QString &status);

private:
    void sendBoarId(int id);
    void sendFloatVolt(double value);
    void sendBoostVolt(double value);
    void sendBoostCurr(double value);
    void sendBoostTime(double value);
    void sendVusb(bool value);
    void sendLog(bool log);
    void sendAllParam(int id, double fvolt, double bvolt,
                      double bcurr, double btime, bool vusb);
    void sendCommand(const QString &cmd);


public slots:
    void onBtnSetId();
    void onBtnSetFloatVolt();
    void onBtnSetBoostVolt();
    void onBtnSetBoostCurr();
    void onBtnSetBoostTime();
    void onBtnSetVusb();
    void onBtnEnableLog();
    void onBtnSetAll();
    void onBtnGetAll();
    void onBtnSetDef();
    void onRecvData(const QString &string);
    void enablePanel(bool en);
    void clearStatus();

    void onBtnTest();
public:
    std::shared_ptr<ibc::layer2> ibc_obj_;
private:
    QGroupBox *groupControl;
    QGroupBox *groupStatus;
    QDoubleSpinBox  *spinFloatVolt, *spinBoostVolt, *spinBoostCurrent,
                    *spinBoostTime, *spinBoardID;

    QPushButton *btnFloatVolt, *btnBoostVolt, *btnBoostCurr,
                *btnBoostTime, *btnBoardID,
                *btnSetAll, *btnGetAllCfg, *btnDefault,
                *btnTest;

    QCheckBox   *checkVusb, *checkEnableLog;

    QLineEdit   *editSTT, *editID,
                *editPV, *editPI, *editPP,
                *editBV, *editBI, *editBC;

    QVBoxLayout *mainLayout;
    QWidget *centerWidget;
    QPlainTextEdit *editPlain ;
    QString testString;
    QString controlString;
};
#endif // CONTROLDOCK_H
