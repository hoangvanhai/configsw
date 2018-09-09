#include "develop.h"
#include <Qchart>

#include <StringUtils.h>

Develop::Develop(QWidget *parent) : QMainWindow(parent)
{
    initVariable();
    createElement();
    createContent();
    createLayout();
    createConnection();
}

Develop::~Develop()
{

}

void Develop::initVariable()
{    
    ibc_obj_ = std::make_shared<ibc::layer2>();
    hTimerControl.setInterval(1000);
    hTimerControl.start();
}

void Develop::createElement()
{
    btnOpenConnection = new QToolButton(this);
    btnCloseConnection = new QToolButton(this);
    btnShowControlPanel = new QToolButton(this);
    centerWidget = new QWidget(this);
    mainLayout = new QVBoxLayout;
    control = new ControlDock(tr("CHARGER CONTROL"), this);
    toolBar = addToolBar(tr("Tool Bar"));
    btnClear = new QToolButton;
    series = new QSplineSeries();
    ivChart = new QChart();
    ivXAxis = new QValueAxis;
}

void Develop::createContent()
{
    statusBar()->showMessage(tr("Ready"));
    statusBar()->setStyleSheet("QStatusBar { color: green;}");    
}

void Develop::createLayout()
{    
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);    
    addDockWidget(Qt::LeftDockWidgetArea, control);

    {
        toolBar->addSeparator();
        QGroupBox *groupBox = new QGroupBox("connection");
        groupBox->setAccessibleName("noborder");
        QHBoxLayout *hLayout = new QHBoxLayout;
        groupBox->setLayout(hLayout);
        hLayout->setAlignment(Qt::AlignTop);

        btnOpenConnection->setFixedSize(50, 50);
        btnOpenConnection->setIcon(QIcon(":/icon/play-button (2).png"));
        btnOpenConnection->setIconSize(QSize(35,35));
        btnOpenConnection->setToolTip("open connection port");
        hLayout->addWidget(btnOpenConnection);

        btnCloseConnection->setIcon(QIcon(":/icon/stop.png"));
        btnCloseConnection->setFixedSize(50, 50);
        btnCloseConnection->setIconSize(QSize(35,35));
        btnCloseConnection->setToolTip("close connection port");
        btnCloseConnection->setEnabled(false);
        hLayout->addWidget(btnCloseConnection);

        btnShowControlPanel->setIcon(QIcon(":/icon/setup.png"));
        btnShowControlPanel->setFixedSize(50, 50);
        btnShowControlPanel->setIconSize(QSize(35,35));
        btnShowControlPanel->setToolTip("show charger control");
        hLayout->addWidget(btnShowControlPanel);

        toolBar->addWidget(groupBox);
        toolBar->addSeparator();
    }


    series->setName("i-v characteris");
    ivChart->addSeries(series);
    //control->legend()->hide();
    //control->setTitle("Simple spline control example");
    ivChart->setAnimationOptions(QChart::SeriesAnimations);

    ivChart->createDefaultAxes();
    ivChart->setAxisX(ivXAxis);

    QChartView *chartView = new QChartView(ivChart);
    chartView->chart()->setTheme(QChart::ChartThemeBlueCerulean);
    chartView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;
    btnClear->setIcon(QIcon("://icon/clear.png"));
    btnClear->setToolTip("clear control screen");
    hLayout->addStretch(1);
    hLayout->addWidget(new QLabel("I-V CHARGER VISUALIZATION"));
    hLayout->addStretch(1);
    hLayout->addWidget(btnClear);
    vLayout->addWidget(chartView);
    vLayout->addLayout(hLayout);
    mainLayout->addLayout(vLayout);
}

void Develop::createConnection()
{        
    connect(btnOpenConnection, SIGNAL(clicked(bool)), this, SLOT(openConnection()));
    connect(btnCloseConnection, SIGNAL(clicked(bool)), this, SLOT(closeConnection()));
    connect(btnShowControlPanel, SIGNAL(clicked(bool)), this, SLOT(showControlPanel()));
    connect(this, SIGNAL(signalConnectionEvent(int)), this, SLOT(recvConnectionEvent(int)));
    connect(this, SIGNAL(signalChargerData(uint8_t*,int)), this, SLOT(recvChargerDataEvent(uint8_t*,int)));
    connect(this, SIGNAL(signalChargerData(uint8_t*,int)), control, SLOT(onRecvData(uint8_t*,int)));
    connect(control, SIGNAL(sigSendData(QString)), this, SLOT(onSendData(QString)));

    connect(&hTimerControl, SIGNAL(timeout()), this, SLOT(onTimerClb()));
}


void Develop::openConnection()
{
    std::cout << "open control \n"; fflush(stdout);
    app::appsetting setting = app::config::instance()->get_app_setting();
    if(ibc_obj_) {
        ibc_obj_->set_notify_event([&](communication::Status status) {
            emit signalConnectionEvent(status);
        });

        ibc_obj_->set_ack(false);
        bool proto = setting.protocol == "isc" ? true : false;
        ibc_obj_->set_protocol(proto);


        if(proto) {
            std::cout << "set control protocol\n";
            ibc_obj_->set_callback_frame_recv([&](uint8_t *frame_,
                                                int frame_len_) {
                (void)frame_len_;
                //std::cout << "recv control protocol: " << frame_len_ << std::endl;
                emit signalChargerData(frame_ + IDX_FRM_DATA0, frame_[IDX_FRM_DLEN]);
            });
        } else {
            std::cout << "set control raw\n";
            ibc_obj_->set_callback_recv_raw([&](const void *frame_,
                                              int frame_len_) {
              //std::cout << "recv control raw: " << frame_len_ << std::endl;
              emit signalChargerData((uint8_t*)frame_, frame_len_);
            });
        }



        std::cout << " port name " << setting.control.port_name.toStdString() << std::endl;
        fflush(stdout);
        int ret = ibc_obj_->start(setting.control.port_name.toStdString(),
                           setting.control.baudrate,
                           setting.control.databits,
                           setting.control.paritybits,
                           setting.control.stopbits);

        std::cout << "ret " << ret << std::endl; fflush(stdout);
    }

}

void Develop::recvConnectionEvent(int event)
{
    switch(event ){
    case communication::Status_Connected:
        btnOpenConnection->setEnabled(false);
        btnCloseConnection->setEnabled(true);
        statusBar()->setStyleSheet("QStatusBar { color: green;}");
        statusBar()->showMessage("control port connected to " +
                                 app::config::instance()->get_app_setting().control.port_name);
        break;
    case communication::Status_Disconnected:
        btnOpenConnection->setEnabled(true);
        btnCloseConnection->setEnabled(false);
        if(ibc_obj_->is_start()) {
            statusBar()->setStyleSheet("QStatusBar { color: red;}");
            statusBar()->showMessage(tr("Can not open control on %1 check port in system ").
                                     arg(app::config::instance()->get_app_setting().control.port_name));
        } else {
            statusBar()->showMessage(tr("control port is closed"));
        }
        break;
    case communication::Status_Device_Disconnected:
        btnOpenConnection->setEnabled(true);
        btnCloseConnection->setEnabled(false);
        ibc_obj_->stop();
        statusBar()->setStyleSheet("QStatusBar { color: red;}");
        statusBar()->showMessage("Lost connection with " +
                                 app::config::instance()->get_app_setting().control.port_name +
                                 " stop control");
        break;
    default:
        std::cout << "def state\n";
        break;
    }
}

void Develop::onTimerClb()
{
//    static qreal count = 0;
//    count+=0.1;
//    series->append(count, count);

//    qDebug() << "set data " << count << " axis max "<< ivXAxis->max();
//    if(count >= ivXAxis->max()) {
//        ivChart->axisX()->setMax(count * 2);
//    }
}

void Develop::onSendData(QString data)
{
    int err = ibc_obj_->send_raw_data(data.data(), data.length());
    qDebug() << "control send data: " << data << " len " << err;
}

void Develop::recvChargerDataEvent(uint8_t *data_, int data_len_)
{
    (void)data_;
    (void)data_len_;
    if(data_len_ <= 0)
        return;

    QString cont = QString((const char *)data_);
    qDebug() << "chart recv: " << cont;
}


void Develop::closeConnection()
{
    if(ibc_obj_) {
        ibc_obj_->stop();
    }
}


void Develop::showControlPanel()
{
    statusBar()->showMessage(tr("show control window"));
    if(control) {
        control->show();
    }
}



ControlDock::ControlDock(const QString &name, QWidget *parent) :
    QDockWidget(name, parent) {
    createElement();
    createLayout();
    createContent();
    createConnection();
}

void ControlDock::createElement() {
    spinFloatVolt = new QDoubleSpinBox;
    spinBoostVolt = new QDoubleSpinBox;
    spinBoostCurrent = new QDoubleSpinBox;
    spinBoostTime = new QDoubleSpinBox;
    spinBoardID = new QDoubleSpinBox;
    checkVusb = new QCheckBox;

    btnBoardID = new QPushButton("SET");
    btnFloatVolt = new QPushButton("SET");
    btnBoostVolt = new QPushButton("SET");
    btnBoostCurr = new QPushButton("SET");
    btnBoostTime = new QPushButton("SET");
    btnVusb = new QPushButton("SET");
    btnSetAll = new QPushButton("SET ALL");
    btnGetAllCfg = new QPushButton("GET ALL");
    btnDefault = new QPushButton("GEN DEF");
    editChargerStatus = new QLineEdit;
    editStatus = new QTextEdit;
    mainLayout = new QVBoxLayout;
    centerWidget = new QWidget;

    editChargerStatus = new QLineEdit;
    editBattCurr = new QLineEdit;
    editBattVolt = new QLineEdit;
    editBattCap = new QLineEdit;
}

void ControlDock::createLayout()
{
    centerWidget->setLayout(mainLayout);
    setWidget(centerWidget);
    QGroupBox *groupControl = new QGroupBox("CONTROL");
    QGroupBox *groupStatus = new QGroupBox("STATUS");

    {
        QGridLayout *gridLayout = new QGridLayout;
        groupControl->setLayout(gridLayout);

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
        gridLayout->addWidget(btnVusb, 5, 2);

        gridLayout->addWidget(btnSetAll, 6, 2);
        gridLayout->addWidget(btnGetAllCfg, 7,2);
        gridLayout->addWidget(btnDefault, 8, 2);
        btnDefault->setToolTip("Reset all board parameter");
        btnSetAll->setToolTip("Set all board paramenter");
        btnGetAllCfg->setToolTip("Get all board paramenter");
    }

    {
        QGridLayout *gridLayout = new QGridLayout;
        groupStatus->setLayout(gridLayout);

        gridLayout->addWidget(new QLabel("CHARGER STATUS"), 0, 0);
        gridLayout->addWidget(editChargerStatus, 0, 1);
        gridLayout->addItem(new QSpacerItem(20, 20), 0, 2);
        gridLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 0, 3);

        gridLayout->addWidget(new QLabel("BATTERY VOLTAGE"), 1, 0);
        gridLayout->addWidget(editBattVolt, 1, 1);
        gridLayout->addWidget(new QLabel("V"), 1, 2);

        gridLayout->addWidget(new QLabel("BATTERY CURRENT"), 2, 0);
        gridLayout->addWidget(editBattCurr,2, 1);
        gridLayout->addWidget(new QLabel("A"), 2, 2);

        gridLayout->addWidget(new QLabel("CHARGED CAPACITY"), 3, 0);
        gridLayout->addWidget(editBattCap, 3, 1);
        gridLayout->addWidget(new QLabel("Ah"), 3, 2);
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 1);
    }

    mainLayout->addWidget(groupControl);
    mainLayout->addWidget(groupStatus);
    mainLayout->addStretch(1);

    setFeatures(DockWidgetClosable | DockWidgetMovable);
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
    connect(btnVusb, SIGNAL(clicked(bool)), this, SLOT(onBtnSetVusb()));
    connect(btnSetAll, SIGNAL(clicked(bool)), this, SLOT(onBtnSetAll()));
    connect(btnGetAllCfg, SIGNAL(clicked(bool)), this, SLOT(onBtnGetAll()));
    connect(btnDefault, SIGNAL(clicked(bool)), this, SLOT(onBtnSetDef()));
}

void ControlDock::sendBoarId(int id)
{
    sendCommand("bid " + QString::number(id));
}

void ControlDock::sendFloatVolt(double value)
{
    sendCommand("bid " + QString::number(value));
}

void ControlDock::sendBoostVolt(double value)
{
    sendCommand("bvolt " + QString::number(value));
}

void ControlDock::sendBoostCurr(double value)
{
    sendCommand("bcurr " + QString::number(value));
}

void ControlDock::sendBoostTime(double value)
{
    sendCommand("btime " + QString::number(value));
}

void ControlDock::sendVusb(bool value)
{
    QString cont = value == true ? "1" : "0";
    sendCommand("vusb " + cont);
}

void ControlDock::sendAllParam(int id, double fvolt,
                               double bvolt, double bcurr,
                               double btime, bool vusb)
{
    QString cont = vusb == true ? "1" : "0";
    sendCommand("bid " + QString::number(id) +
                " fvolt " + QString::number(fvolt) +
                " bvolt " + QString::number(bvolt) +
                " bcurr " + QString::number(bcurr) +
                " btime " + QString::number(btime) +
                " vusb " + cont);
}

void ControlDock::sendCommand(const QString &cmd)
{
    QString command = cmd + "\r\n";
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
    spinBoardID->setValue(1);
    spinFloatVolt->setValue(13.8);
    spinBoostVolt->setValue(14.2);
    spinBoostCurrent->setValue(5.0);
    spinBoostTime->setValue(120.0);
    checkVusb->setChecked(true);

    app::appsetting setting = app::config::instance()->get_app_setting();
    setting.batt.id = spinBoardID->text().toInt();
    setting.batt.floatVolt = spinFloatVolt->text().toDouble();
    setting.batt.boostVolt = spinBoostVolt->text().toDouble();
    setting.batt.boostCurr = spinBoostCurrent->text().toDouble();
    setting.batt.boostTime = spinBoostTime->text().toDouble();
    setting.batt.vUsb = checkVusb->isChecked() ? true : false;

    app::config::instance()->save_config_all(setting);
}

void ControlDock::onRecvData(uint8_t *data_, int data_len_)
{

    (void)data_;
    (void)data_len_;
    if(data_len_ <= 0)
        return;

    QString cont = QString((const char *)data_);
    qDebug() << "control recv: " << cont;

    if(cont == "pass ") {
        bool ok;
        QString text = QInputDialog::getText(this, "Admin access",
                                 "Issue password:", QLineEdit::Password,
                                 "", &ok);
        if (ok && !text.isEmpty()) {
            emit sigSendData("load");
        }

    } else if(cont == "invalid value") {
        QMessageBox::warning(this, "Wrong",
                             "The value is invalid !");
    } else if(cont == "wrong password") {
        QMessageBox::warning(this, "Wrong",
                             "The password is incorrect !");
    } else if(cont == "command success") {
        QMessageBox::information(this, "Information", cont);
    } else {
        if(cont.contains("status")) {

        }
    }
}




