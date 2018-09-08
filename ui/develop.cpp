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
    if(msgFile)
        delete msgFile;
}

void Develop::initVariable()
{    
    ibc_chart_ = std::make_shared<ibc::layer2>();
    msgFile = new QXlsx::Document("default.xlsx", this);
}

void Develop::createElement()
{
    btnOpenChart = new QToolButton(this);
    btnCloseChart = new QToolButton(this);
    btnShowChart = new QToolButton(this);    
    centerWidget = new QWidget(this);
    mainLayout = new QVBoxLayout;
    control = new ControlDock(tr("CHARGER CONTROL"), this);
    toolBar = addToolBar(tr("Tool Bar"));


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

        btnOpenChart->setFixedSize(50, 50);
        btnOpenChart->setIcon(QIcon(":/icon/play-button (2).png"));
        btnOpenChart->setIconSize(QSize(35,35));
        btnOpenChart->setToolTip("open control port");
        hLayout->addWidget(btnOpenChart);

        btnCloseChart->setIcon(QIcon(":/icon/stop.png"));
        btnCloseChart->setFixedSize(50, 50);
        btnCloseChart->setIconSize(QSize(35,35));
        btnCloseChart->setToolTip("close data control");
        btnCloseChart->setEnabled(false);
        hLayout->addWidget(btnCloseChart);

        btnShowChart->setIcon(QIcon(":/icon/monitor (1).png"));
        btnShowChart->setFixedSize(50, 50);
        btnShowChart->setIconSize(QSize(35,35));
        btnShowChart->setToolTip("open data control");
        hLayout->addWidget(btnShowChart);

        toolBar->addWidget(groupBox);
        toolBar->addSeparator();
    }

    btnClear = new QToolButton;    

    QSplineSeries *series = new QSplineSeries();
    series->setName("i-v characteris");
    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
    QChart *control = new QChart();

    control->addSeries(series);
    //control->legend()->hide();
    //control->setTitle("Simple spline control example");
    control->setAnimationOptions(QChart::AllAnimations);
    control->createDefaultAxes();
    control->axisY()->setRange(0, 10);
    QChartView *chartView = new QChartView(control);
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
    connect(btnOpenChart, SIGNAL(clicked(bool)), this, SLOT(openChart()));
    connect(btnCloseChart, SIGNAL(clicked(bool)), this, SLOT(closeChart()));
    connect(btnShowChart, SIGNAL(clicked(bool)), this, SLOT(showChart()));

    connect(this, SIGNAL(signalChartEvent(int)), this, SLOT(recvChartEvent(int)));
    connect(this, SIGNAL(signalChartData(uint8_t*,int)), control, SLOT(recvUpdateChart(uint8_t*,int)));
}


void Develop::openChart()
{
    std::cout << "open control \n"; fflush(stdout);
    app::appsetting setting = app::config::instance()->get_app_setting();
    if(ibc_chart_) {
        ibc_chart_->set_notify_event([&](communication::Status status) {
            emit signalChartEvent(status);
        });

        ibc_chart_->set_ack(false);
        bool proto = setting.protocol == "isc" ? true : false;
        ibc_chart_->set_protocol(proto);


        if(proto) {
            std::cout << "set control protocol\n";
            ibc_chart_->set_callback_frame_recv([&](uint8_t *frame_,
                                                int frame_len_) {
                (void)frame_len_;
                //std::cout << "recv control protocol: " << frame_len_ << std::endl;
                emit signalChartData(frame_ + IDX_FRM_DATA0, frame_[IDX_FRM_DLEN]);
            });
        } else {
            std::cout << "set control raw\n";
            ibc_chart_->set_callback_recv_raw([&](const void *frame_,
                                              int frame_len_) {
              //std::cout << "recv control raw: " << frame_len_ << std::endl;
              emit signalChartData((uint8_t*)frame_, frame_len_);
            });
        }



        std::cout << " port name " << setting.control.port_name.toStdString() << std::endl;
        fflush(stdout);
        int ret = ibc_chart_->start(setting.control.port_name.toStdString(),
                           setting.control.baudrate,
                           setting.control.databits,
                           setting.control.paritybits,
                           setting.control.stopbits);

        std::cout << "ret " << ret << std::endl; fflush(stdout);
    }

}

void Develop::recvChartEvent(int event)
{
    switch(event ){
    case communication::Status_Connected:
        btnOpenChart->setEnabled(false);
        btnCloseChart->setEnabled(true);
        statusBar()->setStyleSheet("QStatusBar { color: green;}");
        statusBar()->showMessage("control port connected to " +
                                 app::config::instance()->get_app_setting().control.port_name);
        break;
    case communication::Status_Disconnected:
        btnOpenChart->setEnabled(true);
        btnCloseChart->setEnabled(false);
        if(ibc_chart_->is_start()) {
            statusBar()->setStyleSheet("QStatusBar { color: red;}");
            statusBar()->showMessage(tr("Can not open control on %1 check port in system ").
                                     arg(app::config::instance()->get_app_setting().control.port_name));
        } else {
            statusBar()->showMessage(tr("control port is closed"));
        }
        break;
    case communication::Status_Device_Disconnected:
        btnOpenChart->setEnabled(true);
        btnCloseChart->setEnabled(false);
        ibc_chart_->stop();
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

void Develop::recvChartDataEvent(uint8_t *data_, int data_len_)
{
    (void)data_;
    (void)data_len_;
    if(data_len_ <= 0)
        return;
}


void Develop::closeChart()
{    
    if(ibc_chart_) {
        ibc_chart_->stop();
    }
}


void Develop::showChart()
{
    statusBar()->showMessage(tr("show control window"));
    if(control) {
        control->show();
    }
}


void Develop::sendChartData(QString)
{

}



void ControlDock::clearChartScreen()
{

}

void ControlDock::saveChartScreen()
{

}

void ControlDock::recvUpdateChart(uint8_t *data, int len)
{

}

ControlDock::ControlDock(const QString &name, QWidget *parent) :
    QDockWidget(name, parent) {

    createElement();
    createLayout();
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

        gridLayout->addWidget(new QLabel("FLOAT VOLTAGE (mV)"), 1, 0);
        gridLayout->addWidget(spinFloatVolt, 1, 1);
        gridLayout->addWidget(btnFloatVolt, 1, 2);

        gridLayout->addWidget(new QLabel("BOOST VOLTAGE (mV)"), 2, 0);
        gridLayout->addWidget(spinBoostVolt,2, 1);
        gridLayout->addWidget(btnBoostVolt, 2, 2);

        gridLayout->addWidget(new QLabel("BOOST CURRENT (mA)"), 3, 0);
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
        btnSetAll->setToolTip("Set all board paramenter");
        btnGetAllCfg->setToolTip("Get all board paramenter");
    }

    {
        QGridLayout *gridLayout = new QGridLayout;
        groupStatus->setLayout(gridLayout);

        gridLayout->addWidget(new QLabel("CHARGER STATUS"), 0, 0);
        gridLayout->addWidget(editChargerStatus, 0, 1);
        // for set this is padding layout
        gridLayout->addItem(new QSpacerItem(20, 20), 0, 2);
        gridLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 0, 3);

        gridLayout->addWidget(new QLabel("BATTERY VOLTAGE"), 1, 0);
        gridLayout->addWidget(editBattVolt, 1, 1);
        gridLayout->addWidget(new QLabel("mV"), 1, 2);


        gridLayout->addWidget(new QLabel("BATTERY CURRENT"), 2, 0);
        gridLayout->addWidget(editBattCurr,2, 1);
        gridLayout->addWidget(new QLabel("mA"), 2, 2);


        gridLayout->addWidget(new QLabel("CHARGED CAPACITY"), 3, 0);
        gridLayout->addWidget(editBattCap, 3, 1);
        gridLayout->addWidget(new QLabel("mAh"), 3, 2);
        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 1);

    }



    mainLayout->addWidget(groupControl);
    mainLayout->addWidget(groupStatus);
    //mainLayout->addWidget(editStatus, 0, Qt::AlignTop);
    mainLayout->addStretch(1);

    setFeatures(DockWidgetClosable | DockWidgetMovable);
}

void ControlDock::createConnection()
{

}
