#include "controldock.h"
#include <Qchart>
#include <StringUtils.h>

ControlDock::ControlDock(const QString &name, QWidget *parent) :
    QDockWidget(name, parent) {
    createElement();
    createLayout();
    createContent();
    createConnection();
    enablePanel(false);
}

void ControlDock::createElement() {
    spinFloatVolt = new QDoubleSpinBox;
    spinBoostVolt = new QDoubleSpinBox;
    spinBoostCurrent = new QDoubleSpinBox;
    spinBoostTime = new QDoubleSpinBox;
    spinBoardID = new QDoubleSpinBox;
    checkVusb = new QCheckBox;
    checkEnableLog = new QCheckBox;

    btnBoardID = new QPushButton("SET");
    btnFloatVolt = new QPushButton("SET");
    btnBoostVolt = new QPushButton("SET");
    btnBoostCurr = new QPushButton("SET");
    btnBoostTime = new QPushButton("SET");
    btnSetAll = new QPushButton("SET ALL");
    btnGetAllCfg = new QPushButton("GET CFG");
    btnDefault = new QPushButton("GEN DEF");
    btnTest = new QPushButton("TEST");


    mainLayout = new QVBoxLayout;
    centerWidget = new QWidget;

    editSTT = new QLineEdit;
    editBV = new QLineEdit;
    editBI = new QLineEdit;
    editBC = new QLineEdit;

    editID = new QLineEdit;
    editPV = new QLineEdit;
    editPI = new QLineEdit;
    editPP = new QLineEdit;


    groupControl = new QGroupBox("CONTROL");
    groupStatus = new QGroupBox("OBSERVE");
}

void ControlDock::createLayout()
{
    centerWidget->setLayout(mainLayout);
    setWidget(centerWidget);
    {
        QVBoxLayout *vlayout = new QVBoxLayout;
        QGridLayout *gridLayout = new QGridLayout;
        QHBoxLayout *hlayout = new QHBoxLayout;
        vlayout->addLayout(gridLayout);
        vlayout->addLayout(hlayout);

        groupControl->setLayout(vlayout);

        gridLayout->addWidget(new QLabel("BOARD ID"), 0, 0);
        gridLayout->addWidget(spinBoardID, 0, 1);
        gridLayout->addWidget(btnBoardID, 0, 2);
        // for set this is padding layout
        gridLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 0, 3);

        gridLayout->addWidget(new QLabel("FLOAT VOLTAGE (V)"), 1, 0);
        gridLayout->addWidget(spinFloatVolt, 1, 1);
        gridLayout->addWidget(btnFloatVolt, 1, 2);

        gridLayout->addWidget(new QLabel("BOOST VOLTAGE (V)"), 2, 0);
        gridLayout->addWidget(spinBoostVolt,2, 1);
        gridLayout->addWidget(btnBoostVolt, 2, 2);

        gridLayout->addWidget(new QLabel("BOOST CURRENT (A)"), 3, 0);
        gridLayout->addWidget(spinBoostCurrent, 3, 1);
        gridLayout->addWidget(btnBoostCurr, 3, 2);

        gridLayout->addWidget(new QLabel("BOOST TIME (min)"), 4, 0);
        gridLayout->addWidget(spinBoostTime, 4, 1);
        gridLayout->addWidget(btnBoostTime, 4, 2);

        gridLayout->addWidget(new QLabel("VUSB ON/OFF"), 5, 0);
        gridLayout->addWidget(checkVusb, 5, 1);

        gridLayout->addWidget(new QLabel("DATA VISUALIZATION"), 6, 0);
        gridLayout->addWidget(checkEnableLog, 6, 1);

        gridLayout->addWidget(btnTest, 7, 2);

        hlayout->addWidget(btnDefault);
        hlayout->addWidget(btnGetAllCfg);
        hlayout->addWidget(btnSetAll);
        hlayout->addStretch(1);

        btnDefault->setToolTip("Generate default configuration");
        btnSetAll->setToolTip("Set all configuration to board");
        btnGetAllCfg->setToolTip("Get all configuration from board");
    }

    {
        QGridLayout *gridLayout = new QGridLayout;
        groupStatus->setLayout(gridLayout);


        gridLayout->addWidget(new QLabel("ID"), 0, 0);
        gridLayout->addWidget(editID, 0, 1);
        gridLayout->addItem(new QSpacerItem(20, 20), 0, 2);
        gridLayout->addItem(new QSpacerItem(20, 20), 0, 3);
        gridLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 0, 4);

        gridLayout->addWidget(new QLabel("STATUS"), 1, 0);
        gridLayout->addWidget(editSTT, 1, 1, 1, 3);


        gridLayout->addWidget(new QLabel("PV-V (V)"), 2, 0);
        gridLayout->addWidget(editPV, 2, 1);
        gridLayout->addWidget(new QLabel("PV-I (A)"), 2, 2);
        gridLayout->addWidget(editPI,2, 3);

        gridLayout->addWidget(new QLabel("PV-P (W)"), 3, 0);
        gridLayout->addWidget(editPP, 3, 1);

        gridLayout->addWidget(new QLabel("B-V (V)"), 4, 0);
        gridLayout->addWidget(editBV, 4, 1);
        gridLayout->addWidget(new QLabel("B-I (A)"), 4, 2);
        gridLayout->addWidget(editBI,4, 3);

        gridLayout->addWidget(new QLabel("B-C (Ah)"), 5, 0);
        gridLayout->addWidget(editBC, 5, 1);

        gridLayout->setColumnStretch(4, 1);

        editID->setMaximumWidth(60);
        //editSTT->setMaximumWidth(80);
        editPV->setMaximumWidth(60);
        editPI->setMaximumWidth(60);
        editPP->setMaximumWidth(60);
        editBV->setMaximumWidth(60);
        editBI->setMaximumWidth(60);
        editBC->setMaximumWidth(60);
    }

    mainLayout->addWidget(groupControl);
    editPlain = new QPlainTextEdit;
//    mainLayout->addWidget(editPlain);
    mainLayout->addWidget(groupStatus);
    mainLayout->addStretch(1);

    setFeatures(DockWidgetMovable);
    //setMinimumWidth(400);
}

void ControlDock::createContent()
{
    spinBoardID->setMaximum(255);
    spinBoardID->setMinimum(0);
    spinBoardID->setDecimals(0);

    spinFloatVolt->setMaximum(14);
    spinFloatVolt->setMinimum(10);
    spinFloatVolt->setDecimals(3);

    spinBoostVolt->setMaximum(16);
    spinBoostVolt->setMinimum(12);
    spinBoostVolt->setDecimals(3);

    spinBoostCurrent->setMaximum(20);
    spinBoostCurrent->setMinimum(1);
    spinBoostCurrent->setDecimals(3);

    spinBoostTime->setMaximum(720);
    spinBoostTime->setMinimum(1);
    spinBoostTime->setDecimals(1);

    app::appsetting setting = app::config::instance()->get_app_setting();
    spinBoardID->setValue(setting.batt.id);
    spinFloatVolt->setValue(setting.batt.floatVolt);
    spinBoostVolt->setValue(setting.batt.boostVolt);
    spinBoostCurrent->setValue(setting.batt.boostCurr);
    spinBoostTime->setValue(setting.batt.boostTime);
    checkVusb->setChecked(setting.batt.vUsb == 1 ? true : false);
}

void ControlDock::createConnection()
{
    connect(btnBoardID, SIGNAL(clicked(bool)), this, SLOT(onBtnSetId()));
    connect(btnFloatVolt, SIGNAL(clicked(bool)), this, SLOT(onBtnSetFloatVolt()));
    connect(btnBoostVolt, SIGNAL(clicked(bool)), this, SLOT(onBtnSetBoostVolt()));
    connect(btnBoostCurr, SIGNAL(clicked(bool)), this, SLOT(onBtnSetBoostCurr()));
    connect(btnBoostTime, SIGNAL(clicked(bool)), this, SLOT(onBtnSetBoostTime()));
    connect(checkVusb, SIGNAL(stateChanged(int)), this, SLOT(onBtnSetVusb()));
    connect(checkEnableLog, SIGNAL(stateChanged(int)), this, SLOT(onBtnEnableLog()));
    connect(btnSetAll, SIGNAL(clicked(bool)), this, SLOT(onBtnSetAll()));
    connect(btnGetAllCfg, SIGNAL(clicked(bool)), this, SLOT(onBtnGetAll()));
    connect(btnDefault, SIGNAL(clicked(bool)), this, SLOT(onBtnSetDef()));
    connect(btnTest, SIGNAL(clicked(bool)), this, SLOT(onBtnTest()));
}

void ControlDock::enablePanel(bool en)
{
    groupControl->setEnabled(en);
}

void ControlDock::clearStatus()
{
    editID->clear();
    editBC->clear();
    editBI->clear();
    editBV->clear();
    editPV->clear();
    editPI->clear();
    editPP->clear();
    editSTT->clear();
}

void ControlDock::onBtnTest()
{
    qDebug() << "onbtnTest";

    //testString = "status ID 100 ST 0 PV 16723 PI 2456 PP 41071688 BV 13612 BI 3345 BC 12345\r\n\n";
    testString = "pass ";

    onRecvData(testString);
}

void ControlDock::sendBoarId(int id)
{
    sendCommand("bid " + QString::number(id));
}

void ControlDock::sendFloatVolt(double value)
{
    sendCommand("bid " + QString::number(value * 1000));
}

void ControlDock::sendBoostVolt(double value)
{
    sendCommand("bvolt " + QString::number(value * 1000));
}

void ControlDock::sendBoostCurr(double value)
{
    sendCommand("bcurr " + QString::number(value * 1000));
}

void ControlDock::sendBoostTime(double value)
{
    sendCommand("btime " + QString::number(value));
}

void ControlDock::sendVusb(bool log)
{
    QString cont = log == true ? "on" : "off";
    sendCommand("vusb " + cont);
}

void ControlDock::sendLog(bool value)
{
    QString cont = value == true ? "on" : "off";
    sendCommand("log " + cont);
}


void ControlDock::sendAllParam(int id, double fvolt,
                               double bvolt, double bcurr,
                               double btime, bool vusb)
{
    QString cont = vusb == true ? "on" : "off";
    sendCommand("bid " + QString::number(id) +
                " fvolt " + QString::number(fvolt) +
                " bvolt " + QString::number(bvolt) +
                " bcurr " + QString::number(bcurr) +
                " btime " + QString::number(btime) +
                " vusb " + cont);
}

void ControlDock::sendCommand(const QString &cmd)
{
    QString command = cmd + "\n";
    emit sigSendData(command);
}

void ControlDock::onBtnSetId()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    int id = spinBoardID->text().toInt();
    if(setting.batt.id != id) {
        setting.batt.id = id;
        app::config::instance()->save_config_all(setting);
        qDebug() << "save new board id = " << id;
    }
    sendBoarId(id);

}

void ControlDock::onBtnSetFloatVolt()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    double val = spinFloatVolt->text().toDouble();
    if(setting.batt.floatVolt != val) {
        setting.batt.floatVolt = val;
        app::config::instance()->save_config_all(setting);
        qDebug() << "save new float volt = " << val;
    }
    sendFloatVolt(val);

}

void ControlDock::onBtnSetBoostVolt()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    double val = spinBoostVolt->text().toDouble();
    if(setting.batt.boostVolt != val) {
        setting.batt.boostVolt = val;
        app::config::instance()->save_config_all(setting);
        qDebug() << "save new boost volt = " << val;
    }
    sendBoostVolt(val);
}

void ControlDock::onBtnSetBoostCurr()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    double val = spinBoostCurrent->text().toDouble();
    if(setting.batt.boostCurr != val) {
        setting.batt.boostCurr = val;
        app::config::instance()->save_config_all(setting);
        qDebug() << "save new boost curr = " << val;
    }
    sendBoostCurr(val);

}

void ControlDock::onBtnSetBoostTime()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    double val = spinBoostTime->text().toDouble();
    if(setting.batt.boostTime != val) {
        setting.batt.boostTime = val;
        app::config::instance()->save_config_all(setting);
        qDebug() << "save new boost time = " << val;
    }
    sendBoostTime(val);
}

void ControlDock::onBtnSetVusb()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    bool val = checkVusb->isChecked() ? true : false;
    if(setting.batt.vUsb != val) {
        setting.batt.vUsb = val;
        app::config::instance()->save_config_all(setting);
        qDebug() << "save new vusb ctrl = " << val;
    }
    sendVusb(val);
}

void ControlDock::onBtnEnableLog()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    bool val = checkEnableLog->isChecked() ? true : false;
    if(setting.batt.dataLog != val) {
        setting.batt.dataLog = val;
        app::config::instance()->save_config_all(setting);
        qDebug() << "save new data log = " << val;
    }
    sendLog(val);
}

void ControlDock::onBtnSetAll()
{
    app::appsetting setting = app::config::instance()->get_app_setting();

    setting.batt.id = spinBoardID->text().toInt();
    setting.batt.floatVolt = spinFloatVolt->text().toDouble();
    setting.batt.boostVolt = spinBoostVolt->text().toDouble();
    setting.batt.boostCurr = spinBoostCurrent->text().toDouble();
    setting.batt.boostTime = spinBoostTime->text().toDouble();
    setting.batt.vUsb = checkVusb->isChecked() ? true : false;

    app::config::instance()->save_config_all(setting);

    // send parameter to board now
    sendAllParam(setting.batt.id,
                 setting.batt.floatVolt,
                 setting.batt.boostVolt,
                 setting.batt.boostCurr,
                 setting.batt.boostTime,
                 setting.batt.vUsb);
}

void ControlDock::onBtnGetAll()
{
    sendCommand("scfg");
}

void ControlDock::onBtnSetDef()
{
    app::appsetting setting = app::config::instance()->get_app_setting();
    setting.batt = setting.battdef;
    spinBoardID->setValue(setting.batt.id);
    spinFloatVolt->setValue(setting.batt.floatVolt);
    spinBoostVolt->setValue(setting.batt.boostVolt);
    spinBoostCurrent->setValue(setting.batt.boostCurr);
    spinBoostTime->setValue(setting.batt.floatVolt);
    checkVusb->setChecked(setting.batt.vUsb);

    setting.batt.id = spinBoardID->text().toInt();
    setting.batt.floatVolt = spinFloatVolt->text().toDouble();
    setting.batt.boostVolt = spinBoostVolt->text().toDouble();
    setting.batt.boostCurr = spinBoostCurrent->text().toDouble();
    setting.batt.boostTime = spinBoostTime->text().toDouble();
    setting.batt.vUsb = checkVusb->isChecked() ? true : false;
    app::config::instance()->save_config_all(setting);
}

void ControlDock::onRecvData(const QString &str)
{
    QString temp = "";
    int pos = 0;
    controlString += str;
    if(str.contains("\n") || str.contains("\r")) {
        QString string = controlString;
        qDebug() << "control string" << controlString;
        controlString = "";

        if(string.contains("invalid value")) {
            QMessageBox::warning(this, "Wrong", string);
        }  else if(string.contains("invalid password")) {
            QMessageBox::warning(this, "Wrong",
                                 "The password is incorrect !");
        } else if(string.contains("command success")) {
            emit showStatusBar("command success");
        } else if(string.contains("pass")) {
            bool ok;
            QString text = QInputDialog::getText(this, "Admin access",
                                     "Issue password:", QLineEdit::Password,
                                     "", &ok, Qt::MSWindowsFixedSizeDialogHint);
            if (ok && !text.isEmpty()) {
                //sendCommand(text);
                emit sigSendData(text + "\n");
            }
        } else if(string.contains("status")) {
            QString cont = string;
            QString contdisp, substring;
            QStringList list;
            double pi, pv;
            cont.remove("\r"); cont.remove("\n"); cont.remove("status: ");
            cont.remove("mA");cont.remove("mV"); cont.remove("mW");
            cont.remove("*");
            list = cont.split(" ");
            int i = 0;
            for(auto var : list) {

                if(var == "ID:" && (list.size() >= i + 1)) {
                    editID->setText(list.at(i+1));
                } else if(var == "ST:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    contdisp = "";
                    if(substring == "0") {
                        contdisp = "IDLE";
                    } else if(substring == "1") {
                        contdisp = "STARTING";
                    } else if(substring == "2") {
                        contdisp = "CONST CURRENT";
                    } else if(substring == "3") {
                        contdisp = "MPPT";
                    } else if(substring == "4") {
                        contdisp = "CONST VOLTAGE";
                    } else if(substring == "5") {
                        contdisp = "FLOAT VOLTAGE";
                    } else if(substring == "6") {
                        contdisp = "TRANSSISION";
                    } else {

                    }

                    if(contdisp != "") {
                        editSTT->setText(contdisp);
                    }

                } else if(var == "PV:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    pv = substring.toDouble() / 1000;
                    editPV->setText(QString::number(pv));
                } else if(var == "PI:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    pi = substring.toDouble() / 1000;
                    editPI->setText(QString::number(pi));
                } else if(var == "PP:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    editPP->setText(QString::number(pi * pv));
                } else if(var == "BV:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    editBV->setText(QString::number(substring.toDouble() / 1000));
                } else if(var == "BI:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    editBI->setText(QString::number(substring.toDouble() / 1000));
                } else if(var == "BC:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    editBC->setText(QString::number(substring.toDouble() / 1000));
                }
                i++;
            }
        } else if(string.contains("show config")) {
            QString cont = string;
            QString substring;
            QStringList list;
            cont.remove("\r"); cont.remove("\n"); cont.remove("show config:");
            cont.remove("mA");cont.remove("mV"); cont.remove("min");
            list = cont.split(" ");
            int i = 0;
            for(auto var : list) {

                if(var == "bid:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinBoardID->setValue(substring.toInt());
                } else if(var == "charg:" && (list.size() >= i + 1)) {

                } else if(var == "fvolt:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinFloatVolt->setValue(substring.toDouble() / 1000);
                } else if(var == "bvolt:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinBoostVolt->setValue(substring.toDouble() / 1000);
                } else if(var == "btime:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinBoostTime->setValue(substring.toDouble() / 1000);
                }  else if(var == "bcurr:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinBoostCurrent->setValue(substring.toDouble() / 1000);
                } else if(var == "vusb:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    checkEnableLog->setChecked(substring == "on" ? true : false);
                }
                i++;
            }
        }

        pos = str.lastIndexOf("\r");
        if(pos > 0 && pos < str.length()) {
            temp = str.mid(pos + str.length());
            controlString = temp;
        }
    }

}




