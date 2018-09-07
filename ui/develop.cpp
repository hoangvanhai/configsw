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
    chart = new ChartDock(tr("BATTERY I-V CHARACTERICS VISUALIZATION"), this);
    toolBar = addToolBar(tr("Tool Bar"));

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
    editStatus = new QPlainTextEdit;
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
    addDockWidget(Qt::RightDockWidgetArea, chart);

    {
        toolBar->addSeparator();
        QGroupBox *groupBox = new QGroupBox("connection");
        groupBox->setAccessibleName("noborder");
        QHBoxLayout *hLayout = new QHBoxLayout;
        groupBox->setLayout(hLayout);
        hLayout->setAlignment(Qt::AlignTop);

        btnOpenChart->setFixedSize(50, 50);
        btnOpenChart->setIcon(QIcon("://icon/play-button (2).png"));
        btnOpenChart->setIconSize(QSize(35,35));
        btnOpenChart->setToolTip("open chart port");
        hLayout->addWidget(btnOpenChart);

        btnCloseChart->setIcon(QIcon("://icon/stop.png"));
        btnCloseChart->setFixedSize(50, 50);
        btnCloseChart->setIconSize(QSize(35,35));
        btnCloseChart->setToolTip("close data chart");
        btnCloseChart->setEnabled(false);
        hLayout->addWidget(btnCloseChart);

        btnShowChart->setIcon(QIcon("://icon/monitor (1).png"));
        btnShowChart->setFixedSize(50, 50);
        btnShowChart->setIconSize(QSize(35,35));
        btnShowChart->setToolTip("open data chart");
        hLayout->addWidget(btnShowChart);

        toolBar->addWidget(groupBox);
        toolBar->addSeparator();
    }

    QGroupBox *groupControl = new QGroupBox("CONTROL");
    QVBoxLayout *vControlLayout = new QVBoxLayout;
    vControlLayout->setSpacing(10);
    groupControl->setLayout(vControlLayout);
    {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addWidget(new QLabel("BOARD ID"), 1);
        hLayout->addWidget(spinBoardID);
        hLayout->addWidget(btnBoardID);
//        hLayout->addStretch(1);
        vControlLayout->addLayout(hLayout);
    }

    {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addWidget(new QLabel("FLOAT VOLTAGE"), 1);
        hLayout->addWidget(spinFloatVolt);
        hLayout->addWidget(btnFloatVolt);
//        hLayout->addStretch(1);
        vControlLayout->addLayout(hLayout);
    }
    {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addWidget(new QLabel("BOOST VOLTAGE"), 1);
        hLayout->addWidget(spinBoostVolt);
        hLayout->addWidget(btnBoostVolt);
//        hLayout->addStretch(1);
        vControlLayout->addLayout(hLayout);
    }

    {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addWidget(new QLabel("BOOST CURRENT"), 1);
        hLayout->addWidget(spinBoostCurrent);
        hLayout->addWidget(btnBoostCurr);
//        hLayout->addStretch(1);
        vControlLayout->addLayout(hLayout);
    }

    {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addWidget(new QLabel("BOOST TIME"), 1);
        hLayout->addWidget(spinBoostTime);
        hLayout->addWidget(btnBoostTime);
//        hLayout->addStretch(1);
        vControlLayout->addLayout(hLayout);
    }

    {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addWidget(new QLabel("VUSB ON/OFF"), 1);
        hLayout->addWidget(checkVusb);
        hLayout->addWidget(btnVusb);
//        hLayout->addStretch(1);
        vControlLayout->addLayout(hLayout);
    }

    {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addStretch(1);
        hLayout->addWidget(btnSetAll);
//        hLayout->addStretch(1);
        vControlLayout->addLayout(hLayout);
    }


    mainLayout->addWidget(groupControl);
    mainLayout->addWidget(editStatus, 1);

}

void Develop::createConnection()
{        
    connect(btnOpenChart, SIGNAL(clicked(bool)), this, SLOT(openChart()));
    connect(btnCloseChart, SIGNAL(clicked(bool)), this, SLOT(closeChart()));
    connect(btnShowChart, SIGNAL(clicked(bool)), this, SLOT(showChart()));

    connect(this, SIGNAL(signalChartEvent(int)), this, SLOT(recvChartEvent(int)));
    connect(this, SIGNAL(signalChartData(uint8_t*,int)), chart, SLOT(recvUpdateChart(uint8_t*,int)));
}


void Develop::openChart()
{
    std::cout << "open chart \n"; fflush(stdout);
    app::appsetting setting = app::config::instance()->get_app_setting();
    if(ibc_chart_) {
        ibc_chart_->set_notify_event([&](communication::Status status) {
            emit signalChartEvent(status);
        });

        ibc_chart_->set_ack(false);
        bool proto = setting.protocol == "isc" ? true : false;
        ibc_chart_->set_protocol(proto);


        if(proto) {
            std::cout << "set chart protocol\n";
            ibc_chart_->set_callback_frame_recv([&](uint8_t *frame_,
                                                int frame_len_) {
                (void)frame_len_;
                //std::cout << "recv chart protocol: " << frame_len_ << std::endl;
                emit signalChartData(frame_ + IDX_FRM_DATA0, frame_[IDX_FRM_DLEN]);
            });
        } else {
            std::cout << "set chart raw\n";
            ibc_chart_->set_callback_recv_raw([&](const void *frame_,
                                              int frame_len_) {
              //std::cout << "recv chart raw: " << frame_len_ << std::endl;
              emit signalChartData((uint8_t*)frame_, frame_len_);
            });
        }



        std::cout << " port name " << setting.chart.port_name.toStdString() << std::endl;
        fflush(stdout);
        int ret = ibc_chart_->start(setting.chart.port_name.toStdString(),
                           setting.chart.baudrate,
                           setting.chart.databits,
                           setting.chart.paritybits,
                           setting.chart.stopbits);

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
        statusBar()->showMessage("chart port connected to " +
                                 app::config::instance()->get_app_setting().chart.port_name);
        break;
    case communication::Status_Disconnected:
        btnOpenChart->setEnabled(true);
        btnCloseChart->setEnabled(false);
        if(ibc_chart_->is_start()) {
            statusBar()->setStyleSheet("QStatusBar { color: red;}");
            statusBar()->showMessage(tr("Can not open chart on %1 check port in system ").
                                     arg(app::config::instance()->get_app_setting().chart.port_name));
        } else {
            statusBar()->showMessage(tr("chart port is closed"));
        }
        break;
    case communication::Status_Device_Disconnected:
        btnOpenChart->setEnabled(true);
        btnCloseChart->setEnabled(false);
        ibc_chart_->stop();
        statusBar()->setStyleSheet("QStatusBar { color: red;}");
        statusBar()->showMessage("Lost connection with " +
                                 app::config::instance()->get_app_setting().chart.port_name +
                                 " stop chart");
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
    statusBar()->showMessage(tr("show chart window"));
    if(chart) {
        chart->show();
    }
}


void Develop::saveMessage()
{
    QString filter="MS Excel files (*.xlsx)";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to excel file"),
                                    QDir::homePath(),
                                    "MS Excel files (*.xlsx)",
                                    &filter, QFileDialog::DontUseNativeDialog);

    if(!fileName.isEmpty()) {
        if(!fileName.endsWith(".xlsx")) fileName += ".xlsx";        
        msgFile->saveAs(fileName);
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
        statusBar()->showMessage("message saved to file " + fileName);
    }
}



void Develop::writeRow(QStringList &row)
{
    if(msgFile) {
        int i = 1; // xlsx must start from column 1
        for(auto item : row) {
            msgFile->write(current_row, i++, item);
        }
        current_row++;
    }
}

void Develop::sendChartData(QString)
{

}



void ChartDock::clearChartScreen()
{

}

void ChartDock::saveChartScreen()
{

}

void ChartDock::recvUpdateChart(uint8_t *data, int len)
{

}

ChartDock::ChartDock(const QString &name, QWidget *parent) :
    QDockWidget(name, parent) {

    createElement();
    createConnection();
}

void ChartDock::createElement() {
    btnClear = new QToolButton;
    btnSave = new QToolButton;
    checkProChart = new QCheckBox;

    QSplineSeries *series = new QSplineSeries();
    series->setName("i-v characteris");
    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
    QChart *chart = new QChart();

    chart->addSeries(series);
    //chart->legend()->hide();
    //chart->setTitle("Simple spline chart example");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->createDefaultAxes();
    chart->axisY()->setRange(0, 10);
    QChartView *chartView = new QChartView(chart);
    chartView->chart()->setTheme(QChart::ChartThemeBlueCerulean);
    chartView->setRenderHint(QPainter::Antialiasing);


    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;
    btnClear->setIcon(QIcon("://icon/clear.png"));
    btnClear->setToolTip("clear chart screen");
    btnSave->setIcon(QIcon("://icon/save (1).png"));
    btnSave->setToolTip("save chart screen to text file");
    hLayout->addWidget(btnClear, 0, Qt::AlignLeft);
    hLayout->addSpacing(10);
    hLayout->addWidget(btnSave, 0, Qt::AlignLeft);
    hLayout->addStretch(1);
    vLayout->addLayout(hLayout);

    vLayout->addWidget(chartView);
    QWidget *centerWidget = new QWidget(this);
    centerWidget->setLayout(vLayout);
    setWidget(centerWidget);
    setFeatures(AllDockWidgetFeatures);

    centerWidget->setMinimumWidth(600);
    //centerWidget->resize(600, height());
}

void ChartDock::createConnection()
{
    connect(btnSave, SIGNAL(clicked(bool)), this, SLOT(saveChartScreen()));
    connect(btnClear, SIGNAL(clicked(bool)), this, SLOT(clearChartScreen()));
}
