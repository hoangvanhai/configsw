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
    hTimerControl.setInterval(2000);
    hTimerTest.setInterval(100);
    hTimerTest.start();
    count = 0;
}

void Develop::createElement()
{
    btnOpenConnection = new QToolButton(this);
    btnCloseConnection = new QToolButton(this);    
    hSplitter = new QSplitter(this);

    mainLayout = new QVBoxLayout;
    toolBar = addToolBar(tr("Tool Bar"));

    iSeries = new QSplineSeries();
    vSeries = new QSplineSeries();
    chart = new QChart();
    chartView = new QChartView(chart);

    hSplitter = new QSplitter;

    editID = new QLineEdit;
    editSTT = new QLineEdit;

    editBV = new QDoubleSpinBox;
    editBI = new QDoubleSpinBox;
    editBC = new QDoubleSpinBox;

    editPV = new QDoubleSpinBox;
    editPI = new QDoubleSpinBox;
    editPP = new QDoubleSpinBox;



    btnClear = new QToolButton;
    checkLogData = new QCheckBox;
    checkVusb = new QCheckBox;

    mainGroup = new QGroupBox("CONTROL - OBSERVE");
    groupControl = new QGroupBox("CONTROL");
    groupStatus = new QGroupBox("OBSERVE");

}

void Develop::createContent()
{
    statusBar()->showMessage(tr("Ready"));
    statusBar()->setStyleSheet("QStatusBar { color: green;}");    
}

void Develop::createLayout()
{
    setCentralWidget(hSplitter);
    {
        toolBar->addSeparator();
        QGroupBox *groupBox = new QGroupBox("connection");
        groupBox->setAccessibleName("noborder");
        QHBoxLayout *hLayout = new QHBoxLayout;
        groupBox->setLayout(hLayout);
        hLayout->setAlignment(Qt::AlignTop);
        btnOpenConnection->setText("   CONNECT   ");
        btnOpenConnection->setToolTip("open connection port");
        hLayout->addWidget(btnOpenConnection);

        btnCloseConnection->setText("DISCONNECT");
        btnCloseConnection->setToolTip("close connection port");
        btnCloseConnection->setEnabled(false);
        hLayout->addWidget(btnCloseConnection);

        toolBar->addWidget(groupBox);
        toolBar->addSeparator();
    }


    hSplitter->addWidget(mainGroup);
    hSplitter->addWidget(chartView);
    QList<int> widths;
    widths.push_back(200);
    widths.push_back(600);
    hSplitter->setSizes(widths);

    {
        iSeries->setName("current");
        vSeries->setName("voltage");
        chart->addSeries(iSeries);
        chart->addSeries(vSeries);
        //control->legend()->hide();
        chart->setTitle("Battery current - voltage");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        chart->createDefaultAxes();
        chart->axisX()->setRange(0, 20);
        chart->axisY()->setRange(0, 16);
        span = static_cast<QValueAxis*>(chart->axisX())->max();

        chartView->chart()->setTheme(QChart::ChartThemeBlueCerulean);
        chartView->setRenderHint(QPainter::Antialiasing);
    }


    mainGroup->setLayout(mainLayout);
    QGridLayout *gLayout = new QGridLayout;

    groupControl->setLayout(gLayout);
    checkVusb->setText("5V VUSB");
    checkLogData->setText("Realtime log");
    btnClear->setText("Clear chart data");

    gLayout->addWidget(checkLogData, 0, 0);
    gLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 0, 1);
    gLayout->addWidget(checkVusb, 1, 0);
    gLayout->addWidget(btnClear, 2, 0);

    gLayout->setRowStretch(1, 1);

    QGridLayout *gridLayout = new QGridLayout;
    groupStatus->setLayout(gridLayout);
    gridLayout->addWidget(new QLabel("Device ID"), 0, 0);
    gridLayout->addWidget(editID, 0, 1);
    gridLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 0, 2);

    gridLayout->addWidget(new QLabel("Device status"), 1, 0);
    gridLayout->addWidget(editSTT, 1, 1);

    gridLayout->addWidget(new QLabel("PV voltage"), 2, 0);
    gridLayout->addWidget(editPV, 2, 1);

    gridLayout->addWidget(new QLabel("PV current"), 3, 0);
    gridLayout->addWidget(editPI,3, 1);

    gridLayout->addWidget(new QLabel("PV power"), 4, 0);
    gridLayout->addWidget(editPP, 4, 1);

    gridLayout->addWidget(new QLabel("Battery voltage"), 5, 0);
    gridLayout->addWidget(editBV, 5, 1);

    gridLayout->addWidget(new QLabel("Battery current"), 6, 0);
    gridLayout->addWidget(editBI,6, 1);

    gridLayout->addWidget(new QLabel("Capacity charged"), 7, 0);
    gridLayout->addWidget(editBC, 7, 1);

    gridLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 8, 0);

    gridLayout->setColumnStretch(2, 1);
    gridLayout->setRowStretch(8, 1);

    editPV->setSuffix(" V");
    editPI->setSuffix(" A");
    editPP->setSuffix(" W");
    editBV->setSuffix(" V");
    editBI->setSuffix(" A");
    editBC->setSuffix(" Ah");

    editPV->setReadOnly(true);
    editPI->setReadOnly(true);
    editPP->setReadOnly(true);
    editBV->setReadOnly(true);
    editBI->setReadOnly(true);
    editBC->setReadOnly(true);

    editPV->setButtonSymbols(QAbstractSpinBox::NoButtons);
    editPI->setButtonSymbols(QAbstractSpinBox::NoButtons);
    editPP->setButtonSymbols(QAbstractSpinBox::NoButtons);
    editBV->setButtonSymbols(QAbstractSpinBox::NoButtons);
    editBI->setButtonSymbols(QAbstractSpinBox::NoButtons);
    editBC->setButtonSymbols(QAbstractSpinBox::NoButtons);



    mainLayout->addWidget(groupControl);
    mainLayout->addWidget(groupStatus);
    mainLayout->addStretch(1);

    onEnablePanel(false);

}

void Develop::createConnection()
{        
    connect(btnOpenConnection, SIGNAL(clicked(bool)), this, SLOT(onBtnOpenConnection()));
    connect(btnCloseConnection, SIGNAL(clicked(bool)), this, SLOT(onBtnCloseConnection()));
    connect(this, SIGNAL(signalConnectionEvent(int)), this, SLOT(recvConnectionEvent(int)));
    connect(this, SIGNAL(signalChargerData(QString)), this, SLOT(recvChargerDataEvent(QString)));    
    connect(checkLogData, SIGNAL(toggled(bool)), this, SLOT(onBtnCheckLog()));
    connect(checkVusb, SIGNAL(toggled(bool)), this, SLOT(onBtnCheckVusb()));
    connect(btnClear, SIGNAL(clicked(bool)), this, SLOT(onBtnClear()));
    connect(&hTimerControl, SIGNAL(timeout()), this, SLOT(onTimeout()));
    //connect(&hTimerTest, SIGNAL(timeout()), this, SLOT(updateChart()));
}

void Develop::setVusbEnable(bool en)
{
    std::string cmd = en == true ? "vx 1\r\n" : "vx 0\r\n" ;
    sendData(cmd);
}

void Develop::setRealtimeLog(bool en)
{
    std::string cmd = en == true ? "lx 1\r\n" : "lx 0\r\n" ;
    sendData(cmd);
    if(en == true) {
        hTimerControl.start();
    } else {
        hTimerControl.stop();
        statusBar()->showMessage("Realtime data is now off");
    }
}


void Develop::onBtnOpenConnection()
{

    app::appsetting setting = app::config::instance()->get_app_setting();
    std::cout << "open control \n"; fflush(stdout);

    if(ibc_obj_) {
        ibc_obj_->set_notify_event([&](communication::Status status) {
            emit signalConnectionEvent(status);
        });

        ibc_obj_->set_ack(false);
        bool proto = setting.gen.protocol == "isc" ? true : false;
        ibc_obj_->set_protocol(proto);


        if(proto) {
            std::cout << "set control protocol\n";
            ibc_obj_->set_callback_frame_recv([&](uint8_t *frame_,
                                                int frame_len_) {
                (void)frame_len_;
                //std::cout << "recv control protocol: " << frame_len_ << std::endl;
                QString msg = QString((const QChar*)(frame_ + IDX_FRM_DATA0), frame_[IDX_FRM_DLEN]);
                emit signalChargerData(msg);
            });
        } else {
            std::cout << "set control raw\n";
            ibc_obj_->set_callback_recv_raw([&](const void *frame_,
                                              int frame_len_) {
                (void)frame_len_;
                QString msg = QString((const char*)frame_);
                qDebug() << "raw " << msg;
                emit signalChargerData(msg);
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
        onEnablePanel(true);
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
        onEnablePanel(false);
        break;
    case communication::Status_Device_Disconnected:
        btnOpenConnection->setEnabled(true);
        btnCloseConnection->setEnabled(false);
        ibc_obj_->stop();
        statusBar()->setStyleSheet("QStatusBar { color: red;}");
        statusBar()->showMessage("Lost connection with " +
                                 app::config::instance()->get_app_setting().control.port_name +
                                 " stop control");
        onEnablePanel(false);
        break;
    default:
        std::cout << "def state\n";
        break;
    }
}




void Develop::sendData(const std::string &data)
{
    if(ibc_obj_->get_status() == communication::Status_Connected) {
        int err = ibc_obj_->send_raw_data(data.data(), data.length());
        (void)err;
        //std::cout << "control send data: " << data << " len " << err;
        fflush(stdout);
    } else {
        QMessageBox::warning(this, "Connection error",
                             "Connection not open, open and try again !");
    }
}

void Develop::setCommunication(std::shared_ptr<ibc::layer2> conn)
{
    ibc_obj_ = conn;
}

void Develop::updateChart(qreal current, qreal voltage)
{
    count += 0.1;
    iSeries->append(count, current);
    vSeries->append(count, voltage);

    if(count >= static_cast<QValueAxis*>(chart->axisX())->max() - 4.0) {
        chart->axisX()->setMax(qreal(count + 4.0 + 0.1));
        chart->axisX()->setMin(qreal(count + 4.0 + 0.1 - span));
    }
}

void Develop::recvChargerDataEvent(const QString &str)
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
            hTimerControl.start();
            statusBar()->showMessage("Realtime data is streaming");
            QString cont = string;
            QString contdisp, substring;
            QStringList list;
            double pi = 0, pv = 0, bv, bi;
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
                    editPV->setValue(pv);
                } else if(var == "PI:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    pi = substring.toDouble() / 1000;
                    editPI->setValue(pi);
                } else if(var == "PP:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    editPP->setValue(pi * pv);
                } else if(var == "BV:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    bv = substring.toDouble() / 1000;
                    editBV->setValue(substring.toDouble() / 1000);
                } else if(var == "BI:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    bi = substring.toDouble() / 1000;
                    editBI->setValue(substring.toDouble() / 1000);                    
                } else if(var == "BC:" && (list.size() >= i + 1)) {
                    substring = list.at(i+1);
                    editBC->setValue(substring.toDouble() / 1000);
                }
                i++;
            }

            qDebug() << "bi = " << bi << " bv = " << bv;
            updateChart(bi, bv);

        } else if(string.contains("config")) {
            QString cont = string;
            QString substring;
            QStringList list;
            cont.remove("\r"); cont.remove("\n"); cont.remove("show config:");
            cont.remove("mA");cont.remove("mV"); cont.remove("min");
            list = cont.split(" ");
        }

        pos = str.lastIndexOf("\r");
        if(pos > 0 && pos < str.length()) {
            temp = str.mid(pos + str.length());
            controlString = temp;
        }
    }
}


void Develop::onBtnCloseConnection()
{
    if(ibc_obj_) {
        ibc_obj_->stop();
    }
}

void Develop::onBtnCheckVusb()
{
    setVusbEnable(checkVusb->isChecked());
}

void Develop::onBtnCheckLog()
{
    setRealtimeLog(checkLogData->isChecked());
}

void Develop::onBtnClear()
{
    count = 0;
    iSeries->clear();
    vSeries->clear();
}



void Develop::onEnablePanel(bool en)
{
    groupControl->setEnabled(en);
}

void Develop::onTimeout()
{
    statusBar()->showMessage("Realtime data is now off");
}
