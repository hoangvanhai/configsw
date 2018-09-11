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
    m_serial = new QSerialPort(this);
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
    control = new ControlDock(tr("CHARGER CONTROL-OBSERVE"), this);
    toolBar = addToolBar(tr("Tool Bar"));
    btnClear = new QToolButton;
    series = new QSplineSeries();    
    ivChart = new QChart();
    ivXAxis = new QValueAxis;
    control->ibc_obj_ = ibc_obj_;
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

        btnOpenConnection->setFixedSize(35, 35);
        btnOpenConnection->setIcon(QIcon(":/icon/play-button (2).png"));
        btnOpenConnection->setIconSize(QSize(30,30));
        btnOpenConnection->setToolTip("open connection port");
        hLayout->addWidget(btnOpenConnection);

        btnCloseConnection->setIcon(QIcon(":/icon/stop.png"));
        btnCloseConnection->setFixedSize(35, 35);
        btnCloseConnection->setIconSize(QSize(30,30));
        btnCloseConnection->setToolTip("close connection port");
        btnCloseConnection->setEnabled(false);
        hLayout->addWidget(btnCloseConnection);

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
    connect(this, SIGNAL(signalChargerData(QString)), this, SLOT(recvChargerDataEvent(QString)));
    connect(this, SIGNAL(signalChargerData(QString)), control, SLOT(onRecvData(QString)));
    connect(control, SIGNAL(sigSendData(std::string)), this, SLOT(onSendData(std::string)));
    connect(control, SIGNAL(showStatusBar(QString)), this, SLOT(onShowStatusMsg(QString)));
    connect(this, SIGNAL(signalEnablePanel(bool)), control, SLOT(enablePanel(bool)));

    connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
}


void Develop::openConnection()
{

    app::appsetting setting = app::config::instance()->get_app_setting();
    std::cout << "open control \n"; fflush(stdout);

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
                QString msg = QString((const QChar*)(frame_ + IDX_FRM_DATA0), frame_[IDX_FRM_DLEN]);
                emit signalChargerData(msg);
            });
        } else {
            std::cout << "set control raw\n";
            ibc_obj_->set_callback_recv_raw([&](const void *frame_,
                                              int frame_len_) {
                (void)frame_len_;
                QString msg = QString((const char*)frame_);
                //qDebug() << "raw " << msg;
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
        emit signalEnablePanel(true);
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
        emit signalEnablePanel(false);
        break;
    case communication::Status_Device_Disconnected:
        btnOpenConnection->setEnabled(true);
        btnCloseConnection->setEnabled(false);
        ibc_obj_->stop();
        statusBar()->setStyleSheet("QStatusBar { color: red;}");
        statusBar()->showMessage("Lost connection with " +
                                 app::config::instance()->get_app_setting().control.port_name +
                                 " stop control");
        emit signalEnablePanel(false);
        break;
    default:
        std::cout << "def state\n";
        break;
    }
}

void Develop::onTimerClb()
{
}

void Develop::onShowStatusMsg(const QString &status)
{
    statusBar()->showMessage(status);
}

void Develop::readData()
{
    const QByteArray data = m_serial->readAll();

    qDebug() << "read " << data;
}

void Develop::onSendData(const std::string &data)
{
    if(ibc_obj_->get_status() == communication::Status_Connected) {
        int err = ibc_obj_->send_raw_data(data.data(), data.length());
        //std::cout << "control send data: " << data << " len " << err;
    } else {
        QMessageBox::warning(this, "Connection error",
                             "Connection not open, open and try again !");
    }
}

void Develop::recvChargerDataEvent(const QString &string)
{    

    if(string.contains("atus")) {
        QString cont = string;
        QString substring;
        QStringList list;
        double bv, bi;
        cont.remove("\r"); cont.remove("\n");
        cont.remove("mA ");cont.remove("mV "); cont.remove("mW ");
        list = cont.split(" ");
        int i = 0;
        for(auto var : list) {

            if(var == "PV:" && (list.size() >= i + 1)) {
                substring = list.at(i+1);

            } else if(var == "PI:" && (list.size() >= i + 1)) {
                substring = list.at(i+1);

            } else if(var == "PP:" && (list.size() >= i + 1)) {
                substring = list.at(i+1);

            } else if(var == "BV:" && (list.size() >= i + 1)) {
                substring = list.at(i+1);
                bv = substring.toDouble() / 1000;
                qDebug() << "BV = " << bv;
            } else if(var == "BI:" && (list.size() >= i + 1)) {
                substring = list.at(i+1);
                bi = substring.toDouble() / 1000;
                qDebug() << "BI = " << bi;
            } else if(var == "BC:" && (list.size() >= i + 1)) {
                substring = list.at(i+1);

            }
            i++;
        }
    }
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


