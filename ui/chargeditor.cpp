#include "chargeditor.h"
#include <cpp_lib/string/StringUtils.h>

using namespace cppframework;


ChargEditor::ChargEditor(QWidget *parent) :
    QMainWindow(parent)
{
    initVariable();
    createElement();
    createLayout();
    createContent();
    createConnection();
}

void ChargEditor::initVariable()
{
    ibc_obj_ = std::make_shared<ibc::layer2>();
    app::appsetting setting = app::config::instance()->get_app_setting();
    numLine = 0;
    filePath = setting.gen.filePathImport;
}

void ChargEditor::createElement()
{
    chart = new QChart;
    iSeries = new QLineSeries;
    vSeries = new QLineSeries;
    chartView = new QChartView(chart, this);
    axisYCurr = new QValueAxis;
    axisYVolt = new QValueAxis;
    axisX = new QValueAxis;

    layoutEditor = new QVBoxLayout;

    hSplitter = new QSplitter(Qt::Horizontal);
    vSplitter = new QSplitter(Qt::Vertical);

    groupEditor = new QGroupBox("I-V CHARACTER EDITOR", this);
    groupControl = new QGroupBox("IMPORT/EXPORT", this);

    btnAddPoint = new QPushButton("+");
    btnRemPoint = new QPushButton("-");

    btnConnect = new QPushButton  ("   CONNECT   ");
    btnDisconnect = new QPushButton("DISCONNECT");

    btnBrowFileImport = new QPushButton;
    btnBrowFileImport->setIcon(QIcon(":/icon/open-folder.png"));
    btnBrowFileExport = new QPushButton;
    btnBrowFileExport->setIcon(QIcon(":/icon/open-folder.png"));

    btnWrite = new QToolButton;
    btnRead = new QToolButton;    
    btnWrite->setText("WRITE DATA");
    btnRead->setText(" READ INFO ");

    editFileImport = new QLineEdit;
    editFileExport = new QLineEdit;

    editDevInfo = new QLineEdit;
    editDevModel = new QLineEdit;
    editDevSerial = new QLineEdit;
    editImportModel = new QLineEdit;

    btnImport = new QPushButton;
    btnImport->setIcon(QIcon(":/icon/right.png"));
    btnImport->setIconSize(QSize(50, 20));
    btnExport = new QPushButton;
    btnExport->setIcon(QIcon(":/icon/left.png"));
    btnExport->setIconSize(QSize(50, 20));
    btnImport->setToolTip(tr("import data from file"));
    btnExport->setToolTip(tr("export data to file"));

    groupComm  = new QGroupBox("COMMUNICATION");

}

void ChargEditor::createLayout()
{
    QScrollArea *scroll = new QScrollArea(this);
    QVBoxLayout *vBox = new QVBoxLayout;
    QWidget *scWidget = new QWidget(this);
    QHBoxLayout *hBox = new QHBoxLayout;

    QWidget *ctrlWidget = new QWidget(this);
    QVBoxLayout *ctrlVBox = new QVBoxLayout;
    QGridLayout *commLayout = new QGridLayout;
    groupComm->setLayout(commLayout);
    ctrlWidget->setLayout(ctrlVBox);
    ctrlVBox->addWidget(groupComm);
    ctrlVBox->addWidget(groupControl);
    ctrlVBox->addStretch(1);

    hBox->addWidget(btnAddPoint);
    hBox->addWidget(btnRemPoint);
    hBox->addStretch(1);
    vBox->addLayout(hBox);
    vBox->addWidget(scroll);
    groupEditor->setLayout(vBox);
    scroll->setWidget(scWidget);
    scroll->setWidgetResizable(true);
    scWidget->setLayout(layoutEditor);

    hSplitter->addWidget(vSplitter);
    hSplitter->addWidget(groupEditor);
    vSplitter->addWidget(ctrlWidget);
    vSplitter->addWidget(chartView);    

    //layoutEditor->setContentsMargins(0,0,0,0);

    QList<int> heights;
    heights.push_back(200);
    heights.push_back(600);
    vSplitter->setSizes(heights);
    QList<int> widths;
    widths.push_back(550);
    widths.push_back(550);
    hSplitter->setSizes(widths);
    setCentralWidget(hSplitter);


    QGridLayout *gridLayout = new QGridLayout;
    groupControl->setLayout(gridLayout);

    commLayout->addWidget(btnConnect, 0, 0);
    commLayout->addWidget(btnRead, 0, 1);
    commLayout->addWidget(new QLabel("DEVICE MODEL"), 0, 2);
    commLayout->addWidget(editDevModel, 0, 3);
//    commLayout->addWidget(new QLabel("DEV INFO"), 0, 2);
//    commLayout->addWidget(editDevInfo, 0, 3);
    commLayout->addWidget(new QLabel("SERIAL"), 0, 4);
    commLayout->addWidget(editDevSerial, 0, 5);
    commLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 0, 6);
    commLayout->addWidget(btnDisconnect, 1, 0);
    commLayout->addWidget(btnWrite, 1, 1);
//    commLayout->addWidget(new QLabel("DEV MODEL"), 1, 2);
//    commLayout->addWidget(editDevModel, 1, 3);
    commLayout->addWidget(new QLabel("IMPORT MODEL"), 1, 2);
    commLayout->addWidget(editImportModel, 1, 3);
    commLayout->setColumnStretch(6, 1);


    gridLayout->addWidget(editFileImport, 0, 0);
    gridLayout->addWidget(btnBrowFileImport, 0, 1);
    gridLayout->addWidget(btnImport, 0, 2);
    gridLayout->addWidget(editFileExport, 1, 0);
    gridLayout->addWidget(btnBrowFileExport, 1, 1);
    gridLayout->addWidget(btnExport, 1, 2);



    btnAddPoint->setMaximumWidth(30);
    btnRemPoint->setMaximumWidth(30);



//    btnWrite->setMinimumHeight(50);
//    btnWrite->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//    btnWrite->setIconSize(QSize(30, 30));


    gridLayout->setColumnStretch(0, 1);

    btnConnect->setToolTip(tr("connect serial port to charger"));
    btnDisconnect->setToolTip(tr("disconnect serial port to charger"));
    btnBrowFileImport->setToolTip(tr("select file to import"));
    btnBrowFileExport->setToolTip(tr("select file to export"));
    btnRemPoint->setToolTip(tr("remove the last point on editor screen"));
    btnAddPoint->setToolTip(tr("add to the end of editor screen"));
    btnWrite->setToolTip(tr("download data from IMPORT MODEL and editor "
                            "screen to the device"));
    btnRead->setToolTip(tr("read device info, fill data to DEVICE MODEL and "
                           "SERIAL"));

    {
        chart->addSeries(iSeries);
        chart->addSeries(vSeries);

        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisYVolt, Qt::AlignRight);
        chart->addAxis(axisYCurr, Qt::AlignLeft);

        axisX->setRange(0, 10);
        axisYCurr->setRange(0, 100);
        axisYVolt->setRange(0, 20);

        iSeries->attachAxis(axisX);
        iSeries->attachAxis(axisYCurr);

        vSeries->attachAxis(axisX);
        vSeries->attachAxis(axisYVolt);

        axisX->setTitleText("Time");
        axisYCurr->setTitleText("Ampe");
        axisYVolt->setTitleText("Voltage");

        //chart->setTitle("GRAPH");

        chart->setAnimationOptions(QChart::NoAnimation);

        chartView->setRenderHint(QPainter::Antialiasing, true);
        chartView->chart()->setTheme(QChart::ChartThemeBlueCerulean);

        iSeries->setName("Battery charge current (A)");
        vSeries->setName("Battery charge voltage (V)");
        iSeries->setColor(QColor("red"));


        vSeries->setPointLabelsVisible(true);
        vSeries->setPointLabelsFormat("@yPoint");

        iSeries->setPointLabelsVisible(true);
        iSeries->setPointLabelsFormat("@yPoint");

        axisYCurr->setLinePenColor(iSeries->pen().color());
        axisYVolt->setLinePenColor(vSeries->pen().color());
    }

    onEnablePanel(false);
}

void ChargEditor::createContent()
{
    statusBar()->showMessage("Ready");
    app::appsetting setting = app::config::instance()->get_app_setting();
    editFileImport->setText(setting.gen.filePathImport);
    editFileExport->setText(setting.gen.filePathExport);
    editImportModel->setText(setting.gen.model);
}

void ChargEditor::createConnection()
{
    connect(btnAddPoint, SIGNAL(clicked(bool)), this, SLOT(onBtnAddPoint()));
    connect(btnRemPoint, SIGNAL(clicked(bool)), this, SLOT(onBtnRemPoint()));
    connect(btnBrowFileExport, SIGNAL(clicked(bool)), this, SLOT(onBtnSelectExport()));
    connect(btnBrowFileImport, SIGNAL(clicked(bool)), this, SLOT(onBtnSelectImport()));
    connect(btnImport, SIGNAL(clicked(bool)), this, SLOT(onBtnImport()));
    connect(btnExport, SIGNAL(clicked(bool)), this, SLOT(onBtnExport()));
    connect(btnConnect, SIGNAL(clicked(bool)), this, SLOT(onBtnOpenConnection()));
    connect(btnDisconnect, SIGNAL(clicked(bool)), this, SLOT(onBtnCloseConnection()));
    connect(this, SIGNAL(signalConnectionEvent(int)), this, SLOT(recvConnectionEvent(int)));
    //connect(this, SIGNAL(signalChargerData(QString)), this, SLOT(recvChargerDataEvent(QString)));
    connect(this, SIGNAL(chargerRawData(const uint8_t*,int)), this, SLOT(recvRawData(const uint8_t*,int)));
    connect(btnRead, SIGNAL(clicked(bool)), this, SLOT(onBtnRead()));
    connect(btnWrite, SIGNAL(clicked(bool)), this, SLOT(onBtnWrite()));
}

bool ChargEditor::loadExportData(const QString &file)
{

    if(listEditor.size() > 0) {
        listData.clear();
        for(auto var : listEditor) {
            NodeInfo node;
            node.id = var->getId();
            node.type = var->getType();
            node.param1 = var->getParam1();
            node.param2 = var->getParam2();
            node.param3 = var->getParam3();
            listData.push_back(node);
            printNodeInfo(node);
        }
        (void)file;

        loader = std::make_shared<CSVFile>(editFileExport->text().toStdString());
        int err = loader->open(true);
        if(err == 0) {
            int i = 0;

            std::vector<std::string> row;
            row.push_back("No");
            row.push_back("Type");
            row.push_back("Current");
            row.push_back("Voltage");
            row.push_back("Time");
            row.push_back("Model");
            writeRowToFile(row);

            for(auto var : listData) {
                row.clear();
                row.push_back(QString::number(var.id).toStdString());
                row.push_back(QString::number(var.type).toStdString());
                row.push_back(QString::number(var.param1).toStdString());
                row.push_back(QString::number(var.param2).toStdString());
                row.push_back(QString::number(var.param3).toStdString());
                if(i == 0) {
                    row.push_back(editImportModel->text().toStdString());
                }
                writeRowToFile(row);
                i++;
            }

            closeFile();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void ChargEditor::updateChart()
{
    if(listEditor.size() > 0) {
        listData.clear();
        for(auto var : listEditor) {
            NodeInfo node;
            node.id     = var->getId();
            node.type   = var->getType();
            node.param1 = var->getParam1();
            node.param2 = var->getParam2();
            node.param3 = var->getParam3();
            listData.push_back(node);
        }
        drawChart();
    }
}

void ChargEditor::setCommunication(std::shared_ptr<ibc::layer2> conn)
{
    ibc_obj_ = conn;
}

void ChargEditor::sendRawData(const std::string &cmd)
{
    if(ibc_obj_->get_status() == communication::Status_Connected) {
        int err = ibc_obj_->send_raw_data(cmd.data(), cmd.length());
        (void)err;
        //std::cout << "control send data: " << data << " len " << err;
        fflush(stdout);
    } else {
        QMessageBox::warning(this, "Connection error",
                             "Connection not open, open and try again !");
    }
}

void ChargEditor::sendProtoData(uint8_t subcmd, const uint8_t *data, int len, bool ack)
{
    if(ibc_obj_->get_status() == communication::Status_Connected) {
        uint8_t *sendData = new uint8_t[len + 2];
        sendData[0] = subcmd;
        sendData[1] = len;
        if(len > 0 && data != NULL) {
            memcpy(&sendData[2], data, len);
        }
        int err = ibc_obj_->send(BC_PC_ID, BC_BOARD_ID,
                                 ack == false ? FRM_DATA : FRM_DATA | ACK_REQ,
                                 sendData, len + 2);
        (void)err;
        //std::cout << "control send data: " << data << " len " << err;
        fflush(stdout);
    } else {
        QMessageBox::warning(this, "Connection error",
                             "Connection not open, open and try again !");
    }
}


double ChargEditor::getParam1Max()
{
    double max = 0;
    for(auto &var : listData) {
        if(var.type == 0) {
            if(max < var.param1) max = var.param1;
        }
    }
    return max;
}

double ChargEditor::getParam2Max()
{
    double max = 0;
    for(auto &var : listData) {
        if(var.type == 0) {
            if(max < var.param2) max = var.param2;
        }
    }
    return max;
}

void ChargEditor::sendReadConfig()
{
    sendProtoData(BC_CONFIG | BC_GET, NULL, 0, true);
}

void ChargEditor::sendWriteConfig()
{
    if(listData.size() > 0) {
        int length = BC_MODEL_LENGTH + 1 + (listData.size() * 27); // size of point = 27
        uint8_t *sendData = new uint8_t[length];
        memset(sendData, 0, length);
        // get model
        std::string model = editImportModel->text().toStdString();
        if(model.size() > 8) model.resize(8);
        for(int i = 0; i < model.size(); i++) {
            sendData[i] = (model.data())[i];
        }

        // get size of data point
        sendData[8] = (uint8_t)listData.size();
        uint8_t *dataptr = &sendData[9];

        for(int i = 0; i < listData.size(); i++) {
            dataptr[i] = listData.at(i).id;
            dataptr[i + 1] = listData.at(i).type;
            Convert::doubleToArray(listData.at(i).param1, &dataptr[i + 2]);
            Convert::doubleToArray(listData.at(i).param1, &dataptr[i + 10]);
            Convert::doubleToArray(listData.at(i).param1, &dataptr[i + 18]);
        }
        sendProtoData(BC_CONFIG | BC_SET, sendData, length, true);
        delete[] sendData;
    }

}


void ChargEditor::drawChart()
{
    static int icount = 0, vcount = 0;

    if(listData.size() > 0) {

        axisX->setRange(0, qreal(listData.size()) + 0.5);

        double maxCurr = getParam1Max();
        //double maxVolt = getParam2Max();

        axisYCurr->setRange(0, ceil(maxCurr + 5));


        iSeries->clear();
        vSeries->clear();

        icount = 0;
        vcount = 0;

        for(size_t i = 0; i < listData.size(); i++) {
            switch(listData.at(i).type) {
            case 0:
                vSeries->append(vcount, listData.at(i).param2);
                vcount++;
                vSeries->append(vcount, listData.at(i).param3);

                iSeries->append(icount, listData.at(i).param1);
                icount++;
                iSeries->append(icount, listData.at(i).param1);

                if((i + 1) < listData.size()) {
                    if(listData.at(i + 1).type == 0) {
                        iSeries->append(icount, listData.at(i + 1).param1);
                    } else if(listData.at(i + 1).type == 1){
                        icount++;
                        vcount++;
                        iSeries->append(icount, listData.at(i + 1).param2);
                    }
                }
                break;
            case 1:
                vSeries->append(vcount, listData.at(i).param1);

                if((i + 1) < listData.size()) {
                    if(listData.at(i + 1).type == 2) {
                        vSeries->append(icount, listData.at(i + 1).param1);
                    }
                }

                vcount++;
                iSeries->append(icount, listData.at(i).param2);
                icount++;
                break;
            case 2:
                vSeries->append(vcount, listData.at(i).param1);                                
                vcount++;
                iSeries->append(icount, (listData.at(i - 1).param2) * 0.8);
                icount++;
                break;
            default:
                break;
            }
        }
    }
}

void ChargEditor::printNodeInfo(const NodeInfo &node)
{
    qDebug() << "id "       << node.id <<
                " type "    << node.type <<
                " param1 "  << node.param1 <<
                " param2 "  << node.param2 <<
                " time "    << node.param3;
}

bool ChargEditor::loadImportData(const QString &file)
{
    loader = std::make_shared<CSVFile>(file.toStdString());
    int err = loader->open();
    if(err == 0) {
        if(loader->get_count()) {

            size_t size = listEditor.size();
            for(size_t i = 0; i < size; i++) {
                onBtnRemPoint();
            }

            listEditor.clear();

            //loader->display_all();
            CSVFile::CsvVector data = loader->rows();

            CSVFile::CsvVector::const_iterator iter;
            for(iter = data.begin();
                iter != data.end(); ++iter) {
                std::vector<std::string> row = *iter;
                if(row.size() >= 5) {
                    if(numLine == 0) {
                        LineEditor *line = new LineEditor(layoutEditor->count(), this);
                        line->setType(str::Util::convert<int>(row.at(1), 0));
                        line->setParam1(str::Util::convert<double>(row.at(2), 0));
                        line->setParam2(str::Util::convert<double>(row.at(3), 0));
                        line->setParam3(str::Util::convert<double>(row.at(4), 0));
                        if(row.size() >= 6)
                            editImportModel->setText(QString(row.at(5).c_str()));
                        layoutEditor->addWidget(line);
                        layoutEditor->addStretch(1);                        
                        listEditor.push_back(line);
                        numLine+=2;
                    } else if (numLine <= 20) {
                        LineEditor *line = new LineEditor(numLine - 1, this);
                        line->setType(str::Util::convert<int>(row.at(1), 0));
                        line->setParam1(str::Util::convert<double>(row.at(2), 0));
                        line->setParam2(str::Util::convert<double>(row.at(3), 0));
                        line->setParam3(str::Util::convert<double>(row.at(4), 0));
                        //qDebug() << "insert count = " << numLine;
                        layoutEditor->insertWidget(numLine - 1, line);
                        listEditor.insert(listEditor.size(), line);
                        numLine++;
                    } else {
                        qDebug() << "max line added";
                    }
                    updateChart();
                }
            }


            for(auto &var : listEditor) {
                connect(var, SIGNAL(changedValue()), this, SLOT(updateChart()));
            }

            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void ChargEditor::writeRowToFile(const std::vector<std::string> row)
{
    if(loader) {
         if(loader->is_open()) {
             loader->write_row(row);
         }
    }
}

void ChargEditor::closeFile()
{
    if(loader != nullptr) {
        loader->close();
        loader.reset();
    }
}

void ChargEditor::onBtnAddPoint()
{
    if(numLine == 0) {
        LineEditor *line = new LineEditor(layoutEditor->count(), this);
        layoutEditor->addWidget(line);
        layoutEditor->addStretch(1);
        connect(line, SIGNAL(changedValue()), this, SLOT(updateChart()));
        listEditor.push_back(line);
        numLine+=2;
    } else if (numLine <= 20) {
        LineEditor *line = new LineEditor(numLine - 1, this);
        //qDebug() << "insert count = " << numLine;
        layoutEditor->insertWidget(numLine - 1, line);
        connect(line, SIGNAL(changedValue()), this, SLOT(updateChart()));
        listEditor.insert(listEditor.size(), line);
        numLine++;
    } else {
        //qDebug() << "max line added";
    }
    updateChart();
}

void ChargEditor::onBtnRemPoint()
{
    if(layoutEditor->count() >= 2) {
        //qDebug() << "delete at " << layoutEditor->count() - 2;
        QWidget *widget = layoutEditor->takeAt(layoutEditor->count() - 2)->widget();        
        delete widget;
        //qDebug() << "removed count = " << layoutEditor->count();
        numLine--;
        listEditor.removeLast();
        updateChart();
    } else {
       // qDebug() << "not rem count = " << layoutEditor->count() << " line " << numLine;
    }
}


void ChargEditor::importDataFromFile(const QString &file)
{
    if(file != "" && file.endsWith(".csv")) {
        filePath = QDir::cleanPath(file);

        if(loadImportData(file)) {
            QMessageBox::information(this, "Information",
                                     "Import successful !");
            app::appsetting setting = app::config::instance()->get_app_setting();
            setting.gen.filePathImport = file;
            setting.gen.model = editImportModel->text();
            app::config::instance()->save_config_general(setting.gen);

        } else {
            QMessageBox::warning(this, "Warning",
                                 "Import failed, no data !");
        }
    } else {
        QMessageBox::warning(this, "Warning",
                             "Import failed, invalid file name !");
    }
}

void ChargEditor::exportDataToFile(const QString &file)
{
    if(file != "" && file.endsWith(".csv")) {
        filePath = QDir::cleanPath(file);
        app::appsetting setting = app::config::instance()->get_app_setting();
        setting.gen.filePathExport = file;
        setting.gen.model = editImportModel->text();
        app::config::instance()->save_config_general(setting.gen);

        if(loadExportData(file)) {
            QMessageBox::information(this, "Information",
                                 "Export successful !");
        } else {
            QMessageBox::warning(this, "Warning",
                                 "Export failed, no data !");
        }
    } else {
        QMessageBox::warning(this, "Warning",
                         "Export failed, invalid file name !");
    }
}


void ChargEditor::onBtnSelectExport()
{

    QString fileName =
            QFileDialog::getSaveFileName(this,
                                         tr("Select file to export"),
                                         filePath == "" ? QDir::homePath() : filePath,
                                         tr("CSV file (*.csv)"));

    if(fileName != "") {
        editFileExport->setText(fileName);
        exportDataToFile(fileName);
    }
}

void ChargEditor::onBtnSelectImport()
{

    QString fileName =
            QFileDialog::getOpenFileName(this,
                                         tr("Select file to export"),
                                         filePath == "" ? QDir::homePath() : filePath,
                                         tr("CSV file (*.csv)"));
    if(fileName != "") {
        editFileImport->setText(fileName);
        importDataFromFile(fileName);
    }
}

void ChargEditor::onBtnImport()
{
    importDataFromFile(editFileImport->text());
}

void ChargEditor::onBtnExport()
{
    exportDataToFile(editFileExport->text());
}



void ChargEditor::onBtnOpenConnection()
{

    app::appsetting setting = app::config::instance()->get_app_setting();
    std::cout << "open control \n"; fflush(stdout);

    if(ibc_obj_) {
        ibc_obj_->set_notify_event([&](communication::Status status) {
            emit signalConnectionEvent(status);
        });

        ibc_obj_->set_ack(true);
        bool proto = setting.gen.protocol == "isc" ? true : false;
        ibc_obj_->set_protocol(proto);


        if(proto) {
            std::cout << "set control protocol\n";
            ibc_obj_->set_callback_frame_recv([&](uint8_t *frame_,
                                                int frame_len_) {
                (void)frame_len_;
                emit chargerRawData((const uint8_t*)(frame_ + IDX_FRM_DATA0), frame_[IDX_FRM_DLEN]);
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

void ChargEditor::recvConnectionEvent(int event)
{
    switch(event ){
    case communication::Status_Connected:
        btnConnect->setEnabled(false);
        btnDisconnect->setEnabled(true);
        statusBar()->setStyleSheet("QStatusBar { color: green;}");
        statusBar()->showMessage("control port connected to " +
                                 app::config::instance()->get_app_setting().control.port_name);
        onEnablePanel(true);
        break;
    case communication::Status_Disconnected:
        btnConnect->setEnabled(true);
        btnDisconnect->setEnabled(false);
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
        btnConnect->setEnabled(true);
        btnDisconnect->setEnabled(false);
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


void ChargEditor::recvChargerDataEvent(const QString &str)
{
    std::string raw = str.toStdString();



}

void ChargEditor::recvRawData(const uint8_t *data, int len)
{

}

void ChargEditor::onEnablePanel(bool en)
{
    btnWrite->setEnabled(en);
    btnRead->setEnabled(en);
    btnDisconnect->setEnabled(en);
    if(en)
        btnDisconnect->setFocus();
    else
        btnConnect->setFocus();
}

void ChargEditor::onBtnRead()
{
    sendReadConfig();
}

void ChargEditor::onBtnWrite()
{
    if(listEditor.size() > 0) {
        if(editImportModel->text() == editDevModel->text()) {
            sendWriteConfig();
        } else {
            QMessageBox::warning(this, "Warning",
                             "Device model not match !");
        }
    } else {
        QMessageBox::warning(this, "Warning",
                         "Write failed, no data !");
    }
}


void ChargEditor::onBtnCloseConnection()
{
    if(ibc_obj_) {
        ibc_obj_->stop();
    }
}

LineEditor::LineEditor(int id, QWidget *parent) :
    QWidget(parent), id_(id)
{    
    createElement();
    setId(id);
    setType(0);
    setParam1(0);
    setParam2(0);
    setParam3(0);
}

LineEditor::LineEditor(int id, int type, double param1,
                       double param2, double param3, QWidget *parent) :
    QWidget(parent), id_(id)
{
    createElement();
    setId(id);
    setType(type);
    setParam1(param1);
    setParam2(param2);
    setParam3(param3);
}

void LineEditor::createElement()
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    {
        spParam1 = new QDoubleSpinBox(this);
        spParam2 = new QDoubleSpinBox(this);
        spParam3 = new QDoubleSpinBox(this);
        comType = new QComboBox(this);
        rowId = new QSpinBox(this);

        lbParam1 = new QLabel(this);
        lbParam2 = new QLabel(this);
        lbParam3 = new QLabel(this);

        comType->addItem("Fi(u)");
        comType->addItem("Fu(t|i)");
        comType->addItem("U(float)");
        comType->addItem("idle");
        rowId->setReadOnly(true);
        rowId->setAlignment(Qt::AlignCenter);
        rowId->setButtonSymbols(QAbstractSpinBox::NoButtons);
        //hLayout->addWidget(new QLabel(tr("Id:")));
        hLayout->addWidget(rowId);
        hLayout->addWidget(new QLabel(tr("Type:")));
        hLayout->addWidget(comType);
        hLayout->addWidget(lbParam1);
        hLayout->addWidget(spParam1);
        hLayout->addWidget(lbParam2);
        hLayout->addWidget(spParam2);
        hLayout->addWidget(lbParam3);
        hLayout->addWidget(spParam3);

        connect(spParam1, SIGNAL(valueChanged(double)), this, SLOT(onChangedValue()));
        connect(spParam2, SIGNAL(valueChanged(double)), this, SLOT(onChangedValue()));
        connect(spParam3, SIGNAL(valueChanged(double)), this, SLOT(onChangedValue()));

        connect(comType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLayout(int)));
        connect(comType, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangedValue()));
        hLayout->setMargin(0);
        rowId->setMaximumWidth(30);
        spParam1->setAlignment(Qt::AlignRight);
        spParam2->setAlignment(Qt::AlignRight);
        spParam3->setAlignment(Qt::AlignRight);

        setInputHidden(true);

        //spParam1->setMaximum(1000);
        //spParam2->setMaximum(1000);
        //spParam3->setMaximum(1000);
    }
    hLayout->addStretch(1);
    setLayout(hLayout);
}

void LineEditor::createContent()
{

}

void LineEditor::setInputHidden(bool hide)
{
    if(hide) {
        lbParam1->hide();
        lbParam2->hide();
        lbParam3->hide();
        spParam1->hide();
        spParam2->hide();
        spParam3->hide();
    } else {
        lbParam1->show();
        lbParam2->show();
        lbParam3->show();
        spParam1->show();
        spParam2->show();
        spParam3->show();
    }
}

void LineEditor::updateLayout(int type)
{
    switch(type) {
    case 0:
        lbParam1->setText("Iconst:");
        lbParam2->setText("Umin:");
        lbParam3->setText("Umax:");

        lbParam1->show();
        lbParam2->show();
        lbParam3->show();

        spParam1->setSuffix(" A");
        spParam2->setSuffix(" V");
        spParam3->setSuffix(" V");

        spParam1->show();
        spParam2->show();
        spParam3->show();
        break;
    case 1:
        lbParam1->setText("Uconst:");
        lbParam2->setText("Imin:");
        lbParam3->setText("Tmax:");

        lbParam1->show();
        lbParam2->show();
        lbParam3->show();

        spParam1->setSuffix(" V");
        spParam2->setSuffix(" A");
        spParam3->setSuffix(" M");

        spParam1->show();
        spParam2->show();
        spParam3->show();        
        break;
    case 2:
        lbParam1->setText("Uconst:");
        spParam1->setSuffix(" V");

        lbParam1->show();
        spParam1->show();

        lbParam2->hide();
        spParam2->hide();
        lbParam3->hide();
        spParam3->hide();

        break;

    case 3:
        setInputHidden(true);
        break;

    default:

        break;
    }
}

void LineEditor::setId(int id)
{
    id_ = id;
    rowId->setValue(id);
}

int LineEditor::getId() const
{
    return id_;
}

void LineEditor::setType(int type)
{
    comType->setCurrentIndex(type);
    updateLayout(type);
}

int LineEditor::getType() const
{
    return comType->currentIndex();
}

void LineEditor::setParam1(double value)
{
//    QPalette p;
    spParam1->setValue(value);
//    if(spParam1->value() <= 0) {
//        p.setColor(QPalette::Text, QColor("red"));
//    } else {
//        p.setColor(QPalette::Text, QColor("white"));
//    }
//    spParam1->setPalette(p);
}

double LineEditor::getParam1() const
{
    return spParam1->value();
}

void LineEditor::setParam2(double value)
{
//    QPalette p;
    spParam2->setValue(value);
//    if(spParam2->value() <= 0) {
//        p.setColor(QPalette::Text, QColor("red"));
//    } else {
//        p.setColor(QPalette::Text, QColor("white"));
//    }
//    spParam2->setPalette(p);
}

double LineEditor::getParam2() const
{
    return spParam2->value();
}

void LineEditor::setParam3(double value)
{
//    QPalette p;
    if(comType->currentIndex() == 1) {
        spParam3->setMaximum(1000);
        spParam3->setDecimals(1);
    } else {
        spParam3->setMaximum(100);
        spParam3->setDecimals(2);
    }
    spParam3->setValue(value);
//    if(spParam3->value() <= 0) {
//        p.setColor(QPalette::Text, QColor("red"));
//    } else {
//        p.setColor(QPalette::Text, QColor("white"));
//    }
//    spParam3->setPalette(p);
}

double LineEditor::getParam3() const
{
    return spParam3->value();
}

void LineEditor::onChangedValue()
{
    emit changedValue();

//    QPalette p1, p2, p3;

//    if(spParam1->value() <= 1) {
//        p1.setColor(QPalette::Text, QColor("red"));
//    } else {
//        qDebug() << "set white 1 " << spParam1->value();
//        p1.setColor(QPalette::Text, QColor("white"));
//    }
//    spParam1->setPalette(p1);
//    spParam1->update();

//    if(spParam2->value() <= 1) {
//        p2.setColor(QPalette::Text, QColor("red"));
//    } else {
//        qDebug() << "set white 2 " << spParam2->value();
//        p2.setColor(QPalette::Text, QColor("white"));
//    }
//    spParam2->setPalette(p2);
//    spParam2->update();

//    if(spParam3->value() <= 1) {
//        p3.setColor(QPalette::Text, QColor("red"));
//    } else {
//        qDebug() << "set white 3 " << spParam3->value();
//        p3.setColor(QPalette::Text, QColor("white"));
//    }

//    spParam3->setPalette(p3);
//    spParam3->update();


}
