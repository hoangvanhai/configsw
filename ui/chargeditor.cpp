#include "chargeditor.h"

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
    app::appsetting setting = app::config::instance()->get_app_setting();
    numLine = 0;
    filePath = setting.filePath;
}

void ChargEditor::createElement()
{
    chart = new QChart;
    iSeries = new QLineSeries;
    vSeries = new QLineSeries;
    chartView = new QChartView(chart, this);

    chart->addSeries(iSeries);
    chart->addSeries(vSeries);
    chart->createDefaultAxes();
    chart->axisX()->setRange(0, 10);
    chart->axisY()->setRange(0, 20);
    chart->setTitle("GRAPH");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    iSeries->setName("current");
    vSeries->setName("voltage");
    layoutEditor = new QVBoxLayout;

    hSplitter = new QSplitter(Qt::Horizontal);
    vSplitter = new QSplitter(Qt::Vertical);

    groupEditor = new QGroupBox("CHARACTER EDITOR", this);
    groupControl = new QGroupBox("CONTROL", this);

    btnAddPoint = new QPushButton("ADD POINT");
    btnRemPoint = new QPushButton("DEL POINT");

    btnConnect = new QPushButton  ("   CONNECT   ");
    btnDisconnect = new QPushButton("DISCONNECT");

    btnBrowFileImport = new QPushButton("IMPORT");
    btnBrowFileExport = new QPushButton("EXPORT");
    btnWrite = new QPushButton("DOWNLOAD");
    editFileImport = new QLineEdit;
    editFileExport = new QLineEdit;
}

void ChargEditor::createLayout()
{
    QScrollArea *scroll = new QScrollArea(this);
    QVBoxLayout *vBox = new QVBoxLayout;
    QWidget *scWidget = new QWidget(this);

    vBox->addWidget(scroll);
    groupEditor->setLayout(vBox);
    scroll->setWidget(scWidget);
    scroll->setWidgetResizable(true);
    scWidget->setLayout(layoutEditor);

    hSplitter->addWidget(vSplitter);
    hSplitter->addWidget(groupEditor);
    vSplitter->addWidget(groupControl);
    vSplitter->addWidget(chartView);    

    layoutEditor->setContentsMargins(0,0,0,0);

//    QString wgstyle = QString("background-color: red;");
//    scWidget->setStyleSheet(wgstyle);
//    layoutEditor->setSizeConstraint(QLayout::SetMinimumSize);

    QList<int> heights;
    heights.push_back(200);
    heights.push_back(600);
    vSplitter->setSizes(heights);
    QList<int> widths;
    widths.push_back(600);
    widths.push_back(370);
    hSplitter->setSizes(widths);
    setCentralWidget(hSplitter);


    QGridLayout *gridLayout = new QGridLayout;
    groupControl->setLayout(gridLayout);

    gridLayout->addWidget(btnConnect, 0, 1);
    gridLayout->addWidget(btnDisconnect, 0, 2);
    gridLayout->addWidget(btnWrite, 0, 3);

    gridLayout->addWidget(editFileImport, 1, 0, 1 , 2);
    gridLayout->addWidget(btnBrowFileImport, 1, 2);

    gridLayout->addWidget(editFileExport, 2, 0, 1 , 2);
    gridLayout->addWidget(btnBrowFileExport, 2, 2);

    gridLayout->addWidget(btnRemPoint, 3, 2);
    gridLayout->addWidget(btnAddPoint, 3, 3);


    gridLayout->setColumnStretch(0, 1);

}

void ChargEditor::createContent()
{
    statusBar()->showMessage("Ready");
    editFileImport->setText(filePath);
    editFileExport->setText(filePath);
}

void ChargEditor::createConnection()
{
    connect(btnAddPoint, SIGNAL(clicked(bool)), this, SLOT(onBtnAddPoint()));
    connect(btnRemPoint, SIGNAL(clicked(bool)), this, SLOT(onBtnRemPoint()));
    connect(btnBrowFileExport, SIGNAL(clicked(bool)), this, SLOT(onBtnExport()));
    connect(btnBrowFileImport, SIGNAL(clicked(bool)), this, SLOT(onBtnImport()));
}

bool ChargEditor::exportDataToFile(const QString &file)
{

    if(listEditor.size() > 0) {
        listData.clear();
        for(auto var : listEditor) {
            NodeInfo node;
            node.id = var->getId();
            node.type = var->getType();
            node.current = var->getCurrent();
            node.voltage = var->getVoltage();
            node.time = var->getTime();
            listData.push_back(node);
            printNodeInfo(node);
        }

        (void)file;
        return true;
    }

    return false;
}

void ChargEditor::updateChart()
{
    if(listEditor.size() > 0) {
        listData.clear();
        for(auto var : listEditor) {
            NodeInfo node;
            node.id = var->getId();
            node.type = var->getType();
            node.current = var->getCurrent();
            node.voltage = var->getVoltage();
            node.time = var->getTime();
            listData.push_back(node);
        }

        drawChart();

    }
}

void ChargEditor::drawChart()
{
    static int count = 0;
    if(listData.size() > 0) {
        chart->axisX()->setRange(0, listData.size() + 1);
        iSeries->clear();
        vSeries->clear();
        count = 0;
        for(auto var : listData) {
            iSeries->append(count, var.current);
            vSeries->append(count, var.voltage);
            count++;
            //qDebug() << "#";
        }

    }
    //qDebug() << "update chart ";
}

void ChargEditor::importDataFromFile(const QString &file)
{
    (void)file;
}

void ChargEditor::printNodeInfo(const NodeInfo &node)
{
    qDebug() << "id " << node.id <<
                " type " << node.type <<
                " current " << node.current <<
                " voltage " << node.voltage <<
                " time " << node.time;
}

bool ChargEditor::loadImportFile(const QString &file)
{
    loader = std::make_shared<CSVFile>(file.toStdString());
    int err = loader->open();
    if(err == 0) {
        if(loader->get_count()) {
            loader->display_all();
            //CSVFile::CsvVector data = loader->rows();
        }
        return true;
    } else {
        return false;
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
    } else if (numLine < 20) {
        LineEditor *line = new LineEditor(numLine - 1, this);
        qDebug() << "insert count = " << numLine;
        layoutEditor->insertWidget(numLine - 1, line);
        connect(line, SIGNAL(changedValue()), this, SLOT(updateChart()));
        listEditor.insert(listEditor.size(), line);
        numLine++;
    } else {
        qDebug() << "max line added";
    }


    updateChart();

//    for(auto var : listEditor) {
//        qDebug() << "id: " << var->getId();
//    }
}

void ChargEditor::onBtnRemPoint()
{
    if(layoutEditor->count() >= 2) {
        qDebug() << "delete at " << layoutEditor->count() - 2;
        QWidget *widget = layoutEditor->takeAt(layoutEditor->count() - 2)->widget();        
        delete widget;
        qDebug() << "removed count = " << layoutEditor->count();
        numLine--;
        listEditor.removeLast();
    } else {
        qDebug() << "not rem count = " << layoutEditor->count() << " line " << numLine;
    }
}

void ChargEditor::onBtnExport()
{
    //if(listEditor.size() > 0)
    {
        QString fileName = QFileDialog::getSaveFileName(this,
             tr("Select file to export"), filePath == "" ? QDir::homePath() : filePath, tr("CSV file (*.csv)"));

        if(fileName != "") {
            filePath = QDir::cleanPath(fileName);
            editFileExport->setText(fileName);

            if(exportDataToFile(fileName)) {
                QMessageBox::information(this, "Information",
                                     "export successful !");
            } else {
                QMessageBox::warning(this, "Warning",
                                     "export failed, no data !");
            }
        }
    }
//    else {
//        QMessageBox::warning(this, "Warning",
//                             "no data, make data and try again !");
//    }
}

void ChargEditor::onBtnImport()
{

    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Select file to export"), filePath == "" ? QDir::homePath() : filePath, tr("CSV file (*.csv)"));

    if(fileName != "") {
        filePath = QDir::cleanPath(fileName);
        app::appsetting setting = app::config::instance()->get_app_setting();
        setting.filePath = filePath;
        app::config::instance()->save_config_all(setting);
        editFileImport->setText(fileName);

        if(loadImportFile(fileName)) {
            QMessageBox::information(this, "Information",
                                 "export successful !");
        } else {
            QMessageBox::warning(this, "Warning",
                                 "export failed, no data !");
        }
    }

}



LineEditor::LineEditor(int id, QWidget *parent) :
    QWidget(parent), id_(id)
{    
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    {
        spCurrent = new QDoubleSpinBox(this);
        spVoltage = new QDoubleSpinBox(this);
        spTimeMax = new QDoubleSpinBox(this);
        comType = new QComboBox(this);
        comType->addItem("Fi(u)");
        comType->addItem("Fu(t)");
        comType->addItem("Fi(t)");
        comType->addItem("idle");
        QSpinBox *rowId = new QSpinBox;
        rowId->setValue(id);
        hLayout->addWidget(rowId);
        hLayout->addWidget(new QLabel(tr("Type:")));
        hLayout->addWidget(comType);
        hLayout->addWidget(new QLabel(tr("I:")));
        hLayout->addWidget(spCurrent);
        hLayout->addWidget(new QLabel(tr("U:")));
        hLayout->addWidget(spVoltage);
        hLayout->addWidget(new QLabel(tr("Time:")));
        hLayout->addWidget(spTimeMax);

        connect(spCurrent, SIGNAL(valueChanged(double)), this, SLOT(onChangedValue()));
        connect(spVoltage, SIGNAL(valueChanged(double)), this, SLOT(onChangedValue()));
        connect(spTimeMax, SIGNAL(valueChanged(double)), this, SLOT(onChangedValue()));
    }
    hLayout->addStretch(1);
//    QString wgstyle = QString("background-color: blue;");
    //QString wgstyle = QString("#myWidget {background-color:red;}");
    //setObjectName("myWidget");
//    this->setStyleSheet(wgstyle);

    setLayout(hLayout);
}

void LineEditor::setId(int id)
{
    id_ = id;
}

int LineEditor::getId() const
{
    return id_;
}

void LineEditor::setType(int idx)
{
    comType->setCurrentIndex(idx);
}

int LineEditor::getType() const
{
    return comType->currentIndex();
}

void LineEditor::setCurrent(double value)
{
    spCurrent->setValue(value);
}

double LineEditor::getCurrent() const
{
    return spCurrent->value();
}

void LineEditor::setVoltage(double value)
{
    spVoltage->setValue(value);
}

double LineEditor::getVoltage() const
{
    return spVoltage->value();
}

void LineEditor::setTime(double time)
{
    spTimeMax->setValue(time);
}

double LineEditor::getTime() const
{
    return spTimeMax->value();
}

void LineEditor::onChangedValue()
{
    emit changedValue();
    qDebug() << "value changed ";
}
