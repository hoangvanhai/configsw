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
    checkCharg = new QCheckBox;
    btnSetAll = new QPushButton("SET ALL");
    btnGetAllCfg = new QPushButton("GET CFG");
    btnDefault = new QPushButton("GEN DEF");

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

        // for set this is padding layout
        gridLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 0, 3);

        gridLayout->addWidget(new QLabel("FLOAT VOLTAGE (V)"), 1, 0);
        gridLayout->addWidget(spinFloatVolt, 1, 1);

        gridLayout->addWidget(new QLabel("BOOST VOLTAGE (V)"), 2, 0);
        gridLayout->addWidget(spinBoostVolt,2, 1);

        gridLayout->addWidget(new QLabel("BOOST CURRENT (A)"), 3, 0);
        gridLayout->addWidget(spinBoostCurrent, 3, 1);

        gridLayout->addWidget(new QLabel("BOOST TIME (min)"), 4, 0);
        gridLayout->addWidget(spinBoostTime, 4, 1);

        gridLayout->addWidget(new QLabel("CHARGER ENABLE"), 5, 0);
        gridLayout->addWidget(checkCharg, 5, 1);

        gridLayout->addWidget(new QLabel("VUSB ENABLE"), 6, 0);
        gridLayout->addWidget(checkVusb, 6, 1);

        gridLayout->addWidget(new QLabel("DATA VISUALIZATION"), 7, 0);
        gridLayout->addWidget(checkEnableLog, 7, 1);

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
    checkVusb->setChecked(setting.batt.vUsb);
    checkCharg->setChecked(setting.batt.charg);
    checkEnableLog->setChecked(setting.batt.dataLog);
}

void ControlDock::createConnection()
{

    connect(btnSetAll, SIGNAL(clicked(bool)), this, SLOT(onBtnSetAll()));
    connect(btnGetAllCfg, SIGNAL(clicked(bool)), this, SLOT(onBtnGetAll()));
    connect(btnDefault, SIGNAL(clicked(bool)), this, SLOT(onBtnSetDef()));    
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

void ControlDock::sendAllParam(int id, double fvolt,
                               double bvolt, double bcurr,
                               double btime, bool vusb,
                               bool charg, bool log)
{
    std::string vusbstr = vusb == true ? "on" : "off";
    std::string chargstr = charg == true ? "on" : "off";
    std::string logstr = log == true ? "on" : "off";

    sendCommand("config " + QString::number(id).toStdString() +
                " " + QString::number(fvolt * 1000).toStdString() +
                " " + QString::number(bvolt * 1000).toStdString() +
                " " + QString::number(bcurr * 1000).toStdString() +
                " " + QString::number(btime).toStdString() +
                " " + vusbstr +
                " " + chargstr +
                " " + logstr);
}

void ControlDock::sendCommand(const std::string &cmd)
{
    std::string command = cmd + "\r\n";
    emit sigSendData(command);
}


void ControlDock::onBtnSetAll()
{


    bool ok;
    QString text = QInputDialog::getText(this, "Admin access",
                             "Issue password:", QLineEdit::Password,
                             "", &ok, Qt::MSWindowsFixedSizeDialogHint);
    if (ok) {
        if(text == "1") {
        app::appsetting setting = app::config::instance()->get_app_setting();
        setting.batt.id = spinBoardID->text().toInt();
        setting.batt.floatVolt = spinFloatVolt->text().toDouble();
        setting.batt.boostVolt = spinBoostVolt->text().toDouble();
        setting.batt.boostCurr = spinBoostCurrent->text().toDouble();
        setting.batt.boostTime = spinBoostTime->text().toDouble();
        setting.batt.vUsb = checkVusb->isChecked();
        setting.batt.dataLog = checkEnableLog->isChecked();
        setting.batt.charg = checkCharg->isChecked();

        app::config::instance()->save_config_all(setting);

        // send parameter to board now
        sendAllParam(setting.batt.id,
                     setting.batt.floatVolt,
                     setting.batt.boostVolt,
                     setting.batt.boostCurr,
                     setting.batt.boostTime,
                     setting.batt.vUsb,
                     setting.batt.charg,
                     setting.batt.dataLog);

        } else {
            QMessageBox::warning(this, "Wrong", "The password is incorrect !");
        }
    }
}

void ControlDock::onBtnGetAll()
{
    sendCommand("scfg");
}

void ControlDock::onBtnSetDef()
{
    app::appsetting setting = app::config::instance()->get_app_setting();    
    setting.batt = setting.battdef;
    setting.batt.conf_name = "batt_charg";
    spinBoardID->setValue(setting.batt.id);
    spinFloatVolt->setValue(setting.batt.floatVolt);
    spinBoostVolt->setValue(setting.batt.boostVolt);
    spinBoostCurrent->setValue(setting.batt.boostCurr);
    spinBoostTime->setValue(setting.batt.boostTime);
    checkVusb->setChecked(setting.batt.vUsb);
    checkCharg->setChecked(setting.batt.charg);
    checkEnableLog->setChecked(setting.batt.dataLog);

    app::config::instance()->save_config_all(setting);
    qDebug() << "generate default config\r\n";
}

void ControlDock::onRecvData(const QString &str)
{
    QString temp = "";
    int pos = 0;
    controlString += str;
    if(str.contains("\n") || str.contains("\r")) {
        QString string = controlString;
        //qDebug() << "control string" << controlString;
        controlString = "";

        if(string.contains("command success")) {
            QMessageBox::information(this, "Information",
                                 "Command successful !");

        } else if(string.contains("status")) {
            QString cont = string;
            QString contdisp, substring;
            QStringList list;
            double pi, pv;
            cont.remove("\r"); cont.remove("\n"); cont.remove("status: ");            
            cont.remove("mA ");cont.remove("mV "); cont.remove("mW ");
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
        } else if(string.contains("config")) {
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
                    substring = list.at(i+1);
                    checkCharg->setChecked(substring == "on" ? true : false);
                } else if(var == "fvolt:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinFloatVolt->setValue(substring.toDouble() / 1000);
                } else if(var == "bvolt:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinBoostVolt->setValue(substring.toDouble() / 1000);
                } else if(var == "btime:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinBoostTime->setValue(substring.toDouble());
                }  else if(var == "bcurr:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    spinBoostCurrent->setValue(substring.toDouble() / 1000);
                } else if(var == "vusb:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    checkEnableLog->setChecked(substring == "on" ? true : false);
                }
                i++;
            }

            showStatusBar("Get configuration success");
        }

        pos = str.lastIndexOf("\r");
        if(pos > 0 && pos < str.length()) {
            temp = str.mid(pos + str.length());
            controlString = temp;
        }
    }

}




