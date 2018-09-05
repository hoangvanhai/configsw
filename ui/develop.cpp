#include "develop.h"
#include <QDebug>
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
    ibc_stream_ =std::make_shared<ibc::layer2>();
    ibc_debug_ = std::make_shared<ibc::layer2>();
    msgFile = new QXlsx::Document("default.xlsx", this);
    initExcelHeader();
}

void Develop::createElement()
{
    btnOpenDebug = new QToolButton(this);
    btnCloseDebug = new QToolButton(this);
    btnShowDebug = new QToolButton(this);
    btnOpenStream = new QToolButton(this);
    btnCloseStream = new QToolButton(this);
    btnShowStream = new QToolButton(this);
    btnClearStream = new QToolButton(this);
    btnSaveStream = new QToolButton(this);
    centerWidget = new QWidget(this);
    mainLayout = new QHBoxLayout;
    tableView = new QTableView(this);
    model = new QStandardItemModel(this);
    debug = new DebugDock(tr("Debug"), this);
    stream = new StreamDock(tr("Stream"), this); 
    toolBar = addToolBar(tr("Tool Bar"));
}

void Develop::createContent()
{
    statusBar()->showMessage(tr("Ready"));
    statusBar()->setStyleSheet("QStatusBar { color: green;}");
    createTableViewHeader();
    stream->set_ibc_object(ibc_stream_);
}

void Develop::createLayout()
{
    mainLayout->addWidget(tableView);
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);
    addDockWidget(Qt::RightDockWidgetArea, debug);
    addDockWidget(Qt::BottomDockWidgetArea, stream);

    {
        toolBar->addSeparator();
        QGroupBox *groupBox = new QGroupBox("stream");
        groupBox->setAccessibleName("noborder");
        QHBoxLayout *hLayout = new QHBoxLayout;
        groupBox->setLayout(hLayout);

        btnOpenStream->setFixedSize(50, 50);
        btnOpenStream->setIcon(QIcon("://icon/play-button (2).png"));
        btnOpenStream->setIconSize(QSize(35,35));
        btnOpenStream->setToolTip("open stream port");        
        hLayout->addWidget(btnOpenStream);


        btnCloseStream->setIcon(QIcon("://icon/stop.png"));
        btnCloseStream->setFixedSize(50, 50);
        btnCloseStream->setIconSize(QSize(35,35));
        btnCloseStream->setToolTip("close stream port");
        btnCloseStream->setEnabled(false);
        hLayout->addWidget(btnCloseStream);


        btnClearStream->setIcon(qApp->style()->standardIcon(QStyle::SP_TrashIcon));
        btnClearStream->setFixedSize(50, 50);
        btnClearStream->setIconSize(QSize(35,35));
        hLayout->addWidget(btnClearStream);
        btnClearStream->setToolTip("clear stream message");


        btnSaveStream->setIcon(QIcon("://icon/save (1).png"));
        btnSaveStream->setFixedSize(50, 50);
        btnSaveStream->setIconSize(QSize(35,35));
        hLayout->addWidget(btnSaveStream);
        btnSaveStream->setToolTip("save stream message to xlsx file");



        btnShowStream->setIcon(QIcon("://icon/transfer.png"));
        btnShowStream->setFixedSize(50, 50);
        btnShowStream->setIconSize(QSize(35,35));
        hLayout->addWidget(btnShowStream);
        btnShowStream->setToolTip("show stream tx");


        toolBar->addWidget(groupBox);
    }

    {
        toolBar->addSeparator();
        QGroupBox *groupBox = new QGroupBox("debug");
        groupBox->setAccessibleName("noborder");
        QHBoxLayout *hLayout = new QHBoxLayout;
        groupBox->setLayout(hLayout);
        hLayout->setAlignment(Qt::AlignTop);

        btnOpenDebug->setFixedSize(50, 50);
        btnOpenDebug->setIcon(QIcon("://icon/play-button (2).png"));
        btnOpenDebug->setIconSize(QSize(35,35));
        btnOpenDebug->setToolTip("open debug port");
        hLayout->addWidget(btnOpenDebug);


        btnCloseDebug->setIcon(QIcon("://icon/stop.png"));
        btnCloseDebug->setFixedSize(50, 50);
        btnCloseDebug->setIconSize(QSize(35,35));        
        btnCloseDebug->setToolTip("close debug port");
        btnCloseDebug->setEnabled(false);
        hLayout->addWidget(btnCloseDebug);


        btnShowDebug->setIcon(QIcon("://icon/monitor (1).png"));
        btnShowDebug->setFixedSize(50, 50);
        btnShowDebug->setIconSize(QSize(35,35));        
        btnShowDebug->setToolTip("close debug port");        
        hLayout->addWidget(btnShowDebug);


        toolBar->addWidget(groupBox);
        toolBar->addSeparator();

    }
}

void Develop::createConnection()
{
    connect(btnOpenStream, SIGNAL(clicked(bool)), this, SLOT(openStream()));
    connect(btnCloseStream, SIGNAL(clicked(bool)), this, SLOT(closeStream()));
    connect(btnClearStream, SIGNAL(clicked(bool)), this, SLOT(clearMessage()));
    connect(btnSaveStream, SIGNAL(clicked(bool)), this, SLOT(saveMessage()));
    connect(btnShowStream, SIGNAL(clicked(bool)), this, SLOT(showStream()));
    connect(btnOpenDebug, SIGNAL(clicked(bool)), this, SLOT(openDebug()));
    connect(btnCloseDebug, SIGNAL(clicked(bool)), this, SLOT(closeDebug()));
    connect(btnShowDebug, SIGNAL(clicked(bool)), this, SLOT(showDebug()));

    connect(this, SIGNAL(signalStreamData(uint8_t*,int)), this, SLOT(recvStreamDataEvent(uint8_t*,int)));
    connect(this, SIGNAL(signalStreamEvent(int)), this, SLOT(recvStreamEvent(int)));
    connect(this, SIGNAL(signalDebugEvent(int)), this, SLOT(recvDebugEvent(int)));
    connect(this, SIGNAL(signalDebugData(uint8_t*,int)), debug, SLOT(recvUpdateDebug(uint8_t*,int)));
}


void Develop::createTableViewHeader()
{
    tableView->setWordWrap(true);
    QStringList header;

    header << "header" << "sequence" << "direction" << "type" << "Data" << "Infomation";
    model->setHorizontalHeaderLabels(header);

    tableView->setModel(model);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setColumnWidth(0, 170);
    tableView->setColumnWidth(1, 80);
    tableView->setColumnWidth(2, 180);
    tableView->setColumnWidth(3, 80);
    tableView->setColumnWidth(4, 600);
    tableView->setColumnWidth(5, 100);
}

void Develop::initExcelHeader()
{
    if(msgFile) {
        QXlsx::Format formatCenter;
        formatCenter.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        formatCenter.setFontSize(9);

        QXlsx::Format formatLeft;
        formatLeft.setFontSize(9);

        msgFile->setColumnFormat(1,1, formatLeft);
        msgFile->setColumnFormat(2,2, formatCenter);
        msgFile->setColumnFormat(3,3, formatCenter);
        msgFile->setColumnFormat(4,4, formatCenter);
        msgFile->setColumnFormat(5,5, formatLeft);

        msgFile->setColumnWidth(1,1,25);
        msgFile->setColumnWidth(3,3,10);
        msgFile->setColumnWidth(5,5,100);

        current_row = 1;
        writeRow(QStringList() << "HEADER" << "SEQ" << "DIRECTION" << "TYPE" << "DATA");
    }
}

void Develop::openDebug()
{
    std::cout << "open debug \n"; fflush(stdout);
    app::appsetting setting = app::config::instance()->get_app_setting();
    if(ibc_debug_) {
        ibc_debug_->set_notify_event([&](communication::Status status) {
            emit signalDebugEvent(status);
        });

        ibc_debug_->set_ack(false);
        bool proto = setting.protocol == "isc" ? true : false;
        ibc_debug_->set_protocol(proto);


        if(proto) {
            std::cout << "set debug protocol\n";
            ibc_debug_->set_callback_frame_recv([&](uint8_t *frame_,
                                                int frame_len_) {
                (void)frame_len_;
                //std::cout << "recv debug protocol: " << frame_len_ << std::endl;
                emit signalDebugData(frame_ + IDX_FRM_DATA0, frame_[IDX_FRM_DLEN]);
            });
        } else {
            std::cout << "set debug raw\n";
            ibc_debug_->set_callback_recv_raw([&](const void *frame_,
                                              int frame_len_) {
              //std::cout << "recv debug raw: " << frame_len_ << std::endl;
              emit signalDebugData((uint8_t*)frame_, frame_len_);
            });
        }



        std::cout << " port name " << setting.debug.port_name.toStdString() << std::endl;
        fflush(stdout);
        int ret = ibc_debug_->start(setting.debug.port_name.toStdString(),
                           setting.debug.baudrate,
                           setting.debug.databits,
                           setting.debug.paritybits,
                           setting.debug.stopbits);

        std::cout << "ret " << ret << std::endl; fflush(stdout);
    }

}

void Develop::recvDebugEvent(int event)
{
    switch(event ){
    case communication::Status_Connected:
        btnOpenDebug->setEnabled(false);
        btnCloseDebug->setEnabled(true);
        statusBar()->setStyleSheet("QStatusBar { color: green;}");
        statusBar()->showMessage("debug port connected to " +
                                 app::config::instance()->get_app_setting().debug.port_name);
        break;
    case communication::Status_Disconnected:
        btnOpenDebug->setEnabled(true);
        btnCloseDebug->setEnabled(false);
        if(ibc_debug_->is_start()) {
            statusBar()->setStyleSheet("QStatusBar { color: red;}");
            statusBar()->showMessage(tr("Can not open debug on %1 check port in system ").
                                     arg(app::config::instance()->get_app_setting().debug.port_name));
        } else {
            statusBar()->showMessage(tr("debug port is closed"));
        }
        break;
    case communication::Status_Device_Disconnected:
        btnOpenDebug->setEnabled(true);
        btnCloseDebug->setEnabled(false);
        ibc_debug_->stop();
        statusBar()->setStyleSheet("QStatusBar { color: red;}");
        statusBar()->showMessage("Lost connection with " +
                                 app::config::instance()->get_app_setting().debug.port_name +
                                 " stop debug");
        break;
    default:
        std::cout << "def state\n";
        break;
    }
}

void Develop::recvDebugDataEvent(uint8_t *data_, int data_len_)
{
    (void)data_;
    (void)data_len_;
    if(data_len_ <= 0)
        return;
}


void Develop::closeDebug()
{    
    if(ibc_debug_) {
        ibc_debug_->stop();
    }
}


void Develop::showDebug()
{
    statusBar()->showMessage(tr("show debug window"));
    if(debug) {
        debug->show();
    }
}


void Develop::openStream()
{    
    app::appsetting setting = app::config::instance()->get_app_setting();
    if(ibc_stream_) {
        ibc_stream_->set_notify_event([&](communication::Status status) {
            emit signalStreamEvent(status);
        });        

        ibc_stream_->set_callback_frame_recv([&](const void *frame_,
                                            int frame_len_) {
            //std::cout << "recv data: " << frame_len_ << std::endl;
            emit signalStreamData((uint8_t*)frame_, frame_len_);
        });

        ibc_stream_->set_callback_cmd_recv([&](const void *cmd_,
                                           int cmd_len_) {
            //std::cout << "recv cmd: " << cmd_len_ << std::endl;
            emit signalStreamData((uint8_t*)cmd_, cmd_len_);
        });

        ibc_stream_->set_ack(false);

        ibc_stream_->start(setting.stream.port_name.toStdString(),
                           setting.stream.baudrate,
                           setting.stream.databits,
                           setting.stream.paritybits,
                           setting.stream.stopbits);        
    }
}

void Develop::recvStreamEvent(int event)
{
    switch(event ){
    case communication::Status_Connected:
        btnOpenStream->setEnabled(false);
        btnCloseStream->setEnabled(true);
        statusBar()->setStyleSheet("QStatusBar { color: green;}");
        statusBar()->showMessage("stream port connected to " +
                                 app::config::instance()->get_app_setting().stream.port_name);
        break;
    case communication::Status_Disconnected:
        btnOpenStream->setEnabled(true);
        btnCloseStream->setEnabled(false);
        if(ibc_stream_->is_start()) {
            statusBar()->setStyleSheet("QStatusBar { color: red;}");
            statusBar()->showMessage(tr("Can not open port %1 check port in system ").
                                     arg(app::config::instance()->get_app_setting().stream.port_name));
        } else {
            statusBar()->showMessage(tr("stream port is closed"));
        }
        break;
    case communication::Status_Device_Disconnected:
        btnOpenStream->setEnabled(true);
        btnCloseStream->setEnabled(false);
        ibc_stream_->stop();
        statusBar()->setStyleSheet("QStatusBar { color: red;}");
        statusBar()->showMessage("Lost connection with " +
                                 app::config::instance()->get_app_setting().stream.port_name +
                                 " stop streaming thread");
        break;
    default:
        std::cout << "def state\n";
        break;
    }
}


void Develop::closeStream()
{


    if(ibc_stream_) {
        ibc_stream_->stop();
    }

}

void Develop::showStream()
{
    statusBar()->showMessage(tr("show stream transmission window"));
    if(stream) {
        stream->show();
    }
}




void Develop::clearMessage()
{
    statusBar()->showMessage(tr("clear stream message"));
    model->clear();
    createTableViewHeader();
    current_row = 1;
    writeRow(QStringList() << "HEADER" << "SEQ" << "DIRECTION" << "TYPE" << "DATA");
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

void Develop::recvStreamDataEvent(uint8_t *data_, int data_len_)
{
    (void)data_len_;
    std::cout << "recv slot data: " << data_len_ << std::endl;

    QString header;
    QString data;
    QString direct = " ";

    for(int i = 0; i < 8; i++) {
        header += QString::number((int)data_[i], 16).
                rightJustified(2, '0').toUpper();
        header += " ";
    }

    for(int i = 0; i < data_[IDX_FRM_DLEN]; i++) {
        data += QString::number((int)data_[IDX_FRM_DATA0 + i], 16).
                rightJustified(2, '0').toUpper();
        data += " ";
    }

    QStringList row;
    row << header << QString::number(int(data_[IDX_FRM_SEQNUM]), 16).
           rightJustified(2, '0').toUpper() <<
           direct << QString::number(int(data_[IDX_FRM_CTRL]), 16).
           rightJustified(2, '0').toUpper() << data;

    appendRow(row);
    writeRow(row);
}






void Develop::appendRow(QStringList &row)
{
    int index = 0;
    QList<QStandardItem *> items;
    QStringList::iterator iter = row.begin();
    while(iter != row.end()) {
        QString text = *iter;
        QStandardItem *item = new QStandardItem(text);
        if(index == 1 || index == 3) {
            item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        } else {
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }

        items.append(item);
        index++;
        iter++;
    }

    model->appendRow(items);
    tableView->scrollToBottom();
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

void Develop::sendDebugData(QString)
{

}




void StreamDock::updateEditData()
{
    QStringList list;
    QString text =  editData->text().remove(" ");
    QString setText;
    for(auto i = 0; i < text.size(); i+=2) {
        list << text.mid(i, 2) + " ";
    }

    for(auto str : list)
        setText += str;

    editData->setText(setText);

}

void StreamDock::onSendRepeat()
{
    if(ibc_stream_) {
        if(checkRepeat->isChecked()) {
            if(ibc_stream_->get_status() !=
                    communication::Status_Connected) {
                QMessageBox::warning(this, "stream comport is not opened!",
                                     "check stream comport and open");
                checkRepeat->setChecked(false);
            } else {
                setPanelEnable(false);
                timer->start(spinDelay->value());
            }
        } else {
            setPanelEnable(true);
            timer->stop();
        }
    }
}

void StreamDock::onSendOneTime()
{
    if(ibc_stream_) {
        if(ibc_stream_->get_status() !=
                communication::Status_Connected) {
            QMessageBox::warning(this, "stream comport is not opened!",
                                 "check stream comport and open");
        } else {
            sendData();
        }
    }

}


StreamDock::StreamDock(const QString &name, QWidget *parent) :
    QDockWidget(name, parent) {
    ibc_stream_ = nullptr;
    createElement();
    createConnection();
}

void StreamDock::createElement() {
    QGridLayout *layout = new QGridLayout;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QGroupBox *group = new QGroupBox("send frame ibc");
    group->setLayout(layout);
    group->setAlignment(Qt::AlignTop);
    comSrc = new QComboBox;
    comDst = new QComboBox;
    editCtrl = new QLineEdit;
    editData = new QLineEdit;
    checkRepeat = new QCheckBox;
    spinDelay = new QSpinBox;
    btnSend = new QPushButton("SEND");

    layout->addWidget(new QLabel("source"), 0, 0);
    layout->addWidget(comSrc, 1, 0);
    comSrc->setMaximumWidth(90);
    comSrc->addItems(QStringList() << "PC" << "Master" << "UI" << "Controller");
    layout->addWidget(new QLabel("dest"), 0, 1);
    layout->addWidget(comDst, 1, 1);
    comDst->setMaximumWidth(90);
    comDst->addItems(QStringList() << "PC" << "Master" << "UI" << "Controller");
    comDst->setCurrentIndex(3);
    layout->addWidget(new QLabel("control"), 0, 2);
    layout->addWidget(editCtrl, 1, 2);
    editCtrl->setMaximumWidth(50);
    editCtrl->setText("20");
    layout->addWidget(new QLabel("data"), 0, 3);
    layout->addWidget(editData, 1, 3);
    layout->addWidget(new QLabel("repeatly"), 0, 4);
    layout->addWidget(checkRepeat, 1, 4);
    layout->addWidget(new QLabel("delay"), 0, 5);
    layout->addWidget(spinDelay, 1, 5);
    spinDelay->setMinimumWidth(100);
    spinDelay->setSuffix(" ms");
    spinDelay->setMinimum(10);
    spinDelay->setMaximum(1000000);
    spinDelay->setValue(100);

    layout->addWidget(btnSend, 1, 6);
    btnSend->setMinimumWidth(100);

    mainLayout->addWidget(group, 0, Qt::AlignTop);
    QWidget *centerWidget = new QWidget;
    centerWidget->setLayout(mainLayout);
    setWidget(centerWidget);
    resize(width(), 100);

    QRegExp regex("(([a-fA-F]|[0-9]){2}){1}");
    QRegExpValidator *validCtrl = new QRegExpValidator(regex);

    editCtrl->setValidator(validCtrl);
    editCtrl->setMaxLength(2);
    editCtrl->setAlignment(Qt::AlignCenter);

    QRegExp regex1("(([a-fA-F]|[0-9]| ){3}){360}");
    QRegExpValidator *validData = new QRegExpValidator(regex1);
    editData->setValidator(validData);
    editData->setMaxLength(720);

    timer = new QTimer(this);
    timer->setInterval(spinDelay->value());
}

void StreamDock::setPanelEnable(bool en)
{
    comSrc->setEnabled(en);
    comDst->setEnabled(en);
    editCtrl->setEnabled(en);
    editData->setEnabled(en);
    btnSend->setEnabled(en);
}

void StreamDock::createConnection() {
    connect(editData, SIGNAL(textChanged(QString)), this, SLOT(updateEditData()));
    connect(checkRepeat, SIGNAL(clicked(bool)), this, SLOT(onSendRepeat()));
    connect(btnSend, SIGNAL(clicked(bool)), this, SLOT(onSendOneTime()));
    connect(timer, SIGNAL(timeout()), this, SLOT(sendData()));
    connect(spinDelay, SIGNAL(editingFinished()), this, SLOT(updateDelay()));
}

void StreamDock::sendData()
{
    if(ibc_stream_) {
        bool ok;
        QString text = editData->text().remove(" ");        
        auto text_size = text.size();
        uint8_t size;

        if(text_size % 2 == 0) {
            size = text_size / 2;
        } else {
            size = text_size / 2 + 1;
        }

        if(size > FRM_MAX_DLEN) {
            std::cout << "invalid dlen\n";
            return;
        }

        uint8_t len;
        uint8_t *data;

        uint8_t src = (uint8_t)comSrc->currentIndex();
        uint8_t dst = (uint8_t)comDst->currentIndex();
        uint8_t ctrl = (uint8_t)editCtrl->text().toUInt(&ok, 16);

        if(size > 0) {
            data = new uint8_t[size];
            len = size;
        } else {            
            len = 0;
            data = NULL;
        }

        for(auto i = 0, j = 0; i < text_size; i+=2, j+=1) {
            data[j] = text.mid(i, 2).toUInt(&ok, 16);
        }

        ibc_stream_->send(src, dst, ctrl, data, len);

        if(data != NULL)
            delete[] data;
    }
}

void StreamDock::updateDelay()
{
    timer->setInterval(spinDelay->value());
}

void DebugDock::clearDebugScreen()
{
    editScreen->clear();
}

void DebugDock::saveDebugScreen()
{
    QString filter="Text files (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to text file"),
                                    QDir::homePath(),
                                    "Text files (*.txt)"
                                    , &filter, QFileDialog::DontUseNativeDialog);

    if(!fileName.isEmpty()) {
        if(!fileName.endsWith(".txt")) fileName += ".txt";
        QFile file( fileName );
        if ( file.open(QIODevice::ReadWrite) ) {
            QTextStream stream( &file );
            stream << editScreen->toPlainText();
            stream.flush();
            file.close();
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
        }
    }    
}

void DebugDock::recvUpdateDebug(uint8_t *data, int len)
{
    if(len <= 0) return;
    data[len] = '\0';
    QString str((const char*)data);
    //std::cout << str.toStdString(); fflush(stdout);
    editScreen->insertPlainText(str);

    editScreen->verticalScrollBar()->setValue(editScreen->verticalScrollBar()->maximum());


}

DebugDock::DebugDock(const QString &name, QWidget *parent) :
    QDockWidget(name, parent) {

    createElement();
    createConnection();
}

void DebugDock::createElement() {
    resize(200, height());
    btnClear = new QToolButton;
    btnSave = new QToolButton;
    checkProDebug = new QCheckBox;
    editScreen = new QTextEdit;
    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;
    btnClear->setIcon(QIcon("://icon/clear.png"));
    btnClear->setToolTip("clear debug screen");
    btnSave->setIcon(QIcon("://icon/save (1).png"));
    btnSave->setToolTip("save debug screen to text file");
    hLayout->addWidget(btnClear, 0, Qt::AlignLeft);
    hLayout->addSpacing(10);
    hLayout->addWidget(btnSave, 1, Qt::AlignLeft);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(editScreen);
    QWidget *centerWidget = new QWidget;
    centerWidget->setLayout(vLayout);
    setWidget(centerWidget);
    setFeatures(AllDockWidgetFeatures);
}

void DebugDock::createConnection()
{
    connect(btnSave, SIGNAL(clicked(bool)), this, SLOT(saveDebugScreen()));
    connect(btnClear, SIGNAL(clicked(bool)), this, SLOT(clearDebugScreen()));
}
