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
    app::appsetting setting = app::config::instance()->get_app_setting();
    numLine = 0;
    filePath = setting.filePathImport;
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
    iSeries->setColor(QColor("red"));
    layoutEditor = new QVBoxLayout;

    hSplitter = new QSplitter(Qt::Horizontal);
    vSplitter = new QSplitter(Qt::Vertical);

    groupEditor = new QGroupBox("CHARACTER EDITOR", this);
    groupControl = new QGroupBox("CONTROL", this);

    btnAddPoint = new QPushButton("ADD POINT");
    btnRemPoint = new QPushButton("DEL POINT");

    btnConnect = new QPushButton  ("   CONNECT   ");
    btnDisconnect = new QPushButton("DISCONNECT");

    btnBrowFileImport = new QPushButton;
    btnBrowFileImport->setIcon(QIcon(":/icon/open-folder.png"));
    btnBrowFileExport = new QPushButton;
    btnBrowFileExport->setIcon(QIcon(":/icon/open-folder.png"));

    btnWrite = new QToolButton;
    btnWrite->setIcon(QIcon(":/icon/download.png"));
    btnWrite->setText("DOWNLOAD");

    editFileImport = new QLineEdit;
    editFileExport = new QLineEdit;

    btnImport = new QPushButton("IMPORT");
    btnExport = new QPushButton("EXPORT");
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

    gridLayout->addWidget(btnConnect, 0, 2);
    gridLayout->addWidget(btnDisconnect, 0, 3);

    gridLayout->addWidget(editFileImport, 1, 0);
    gridLayout->addWidget(btnBrowFileImport, 1, 1);
    gridLayout->addWidget(btnImport, 1, 2);
    gridLayout->addWidget(btnWrite, 1, 3, 2, 1);

    gridLayout->addWidget(editFileExport, 2, 0);
    gridLayout->addWidget(btnBrowFileExport, 2, 1);
    gridLayout->addWidget(btnExport, 2, 2);

    gridLayout->addWidget(btnRemPoint, 3, 2);
    gridLayout->addWidget(btnAddPoint, 3, 3);


    btnWrite->setMinimumHeight(50);
    btnWrite->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnWrite->setIconSize(QSize(30, 30));
    gridLayout->setColumnStretch(0, 1);

    btnConnect->setToolTip(tr("connect serial port to charger"));
    btnDisconnect->setToolTip(tr("disconnect serial port to charger"));
    btnRemPoint->setToolTip(tr("remove the last point on editor screen"));
    btnAddPoint->setToolTip(tr("add to the end of editor screen"));
    btnWrite->setToolTip(tr("download config to the board"));

}

void ChargEditor::createContent()
{
    statusBar()->showMessage("Ready");
    app::appsetting setting = app::config::instance()->get_app_setting();
    editFileImport->setText(setting.filePathImport);
    editFileExport->setText(setting.filePathExport);
}

void ChargEditor::createConnection()
{
    connect(btnAddPoint, SIGNAL(clicked(bool)), this, SLOT(onBtnAddPoint()));
    connect(btnRemPoint, SIGNAL(clicked(bool)), this, SLOT(onBtnRemPoint()));
    connect(btnBrowFileExport, SIGNAL(clicked(bool)), this, SLOT(onBtnSelectExport()));
    connect(btnBrowFileImport, SIGNAL(clicked(bool)), this, SLOT(onBtnSelectImport()));
    connect(btnImport, SIGNAL(clicked(bool)), this, SLOT(onBtnImport()));
    connect(btnExport, SIGNAL(clicked(bool)), this, SLOT(onBtnExport()));
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

        loader = std::make_shared<CSVFile>(editFileExport->text().toStdString());
        int err = loader->open(true);
        if(err == 0) {

            std::vector<std::string> row;
            row.push_back("No");
            row.push_back("Type");
            row.push_back("Current");
            row.push_back("Voltage");
            row.push_back("Time");
            writeRowToFile(row);

            for(auto var : listData) {
                row.clear();
                row.push_back(QString::number(var.id).toStdString());
                row.push_back(QString::number(var.type).toStdString());
                row.push_back(QString::number(var.current).toStdString());
                row.push_back(QString::number(var.voltage).toStdString());
                row.push_back(QString::number(var.time).toStdString());
                writeRowToFile(row);
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
        }

    }
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
                        line->setCurrent(str::Util::convert<double>(row.at(2), 0));
                        line->setVoltage(str::Util::convert<double>(row.at(3), 0));
                        line->setTime(str::Util::convert<double>(row.at(4), 0));
                        layoutEditor->addWidget(line);
                        layoutEditor->addStretch(1);
                        //connect(line, SIGNAL(changedValue()), this, SLOT(updateChart()));
                        listEditor.push_back(line);
                        numLine+=2;
                    } else if (numLine < 22) {
                        LineEditor *line = new LineEditor(numLine - 1, this);
                        line->setType(str::Util::convert<int>(row.at(1), 0));
                        line->setCurrent(str::Util::convert<double>(row.at(2), 0));
                        line->setVoltage(str::Util::convert<double>(row.at(3), 0));
                        line->setTime(str::Util::convert<double>(row.at(4), 0));
                        qDebug() << "insert count = " << numLine;
                        layoutEditor->insertWidget(numLine - 1, line);
                        //connect(line, SIGNAL(changedValue()), this, SLOT(updateChart()));
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
            return true;
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
             //qDebug() << "write row";
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
    } else if (numLine < 20) {
        LineEditor *line = new LineEditor(numLine - 1, this);
        qDebug() << "insert count = " << numLine;
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
        qDebug() << "delete at " << layoutEditor->count() - 2;
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

void ChargEditor::onBtnSelectExport()
{
    {
        QString fileName = QFileDialog::getSaveFileName(this,
             tr("Select file to export"), filePath == "" ? QDir::homePath() : filePath, tr("CSV file (*.csv)"));

        if(fileName != "") {
            filePath = QDir::cleanPath(fileName);
            app::appsetting setting = app::config::instance()->get_app_setting();
            setting.filePathExport = filePath;
            app::config::instance()->save_config_all(setting);
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
}

void ChargEditor::onBtnSelectImport()
{

    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Select file to export"), filePath == "" ? QDir::homePath() : filePath, tr("CSV file (*.csv)"));

    if(fileName != "") {
        filePath = QDir::cleanPath(fileName);
        app::appsetting setting = app::config::instance()->get_app_setting();
        setting.filePathImport = filePath;
        app::config::instance()->save_config_all(setting);
        editFileImport->setText(fileName);

        if(loadImportFile(fileName)) {
            QMessageBox::information(this, "Information",
                                     "import successful !");
        } else {
            QMessageBox::warning(this, "Warning",
                                 "import failed, no data !");
        }
    }

}

void ChargEditor::onBtnImport()
{
    if(editFileImport->text() != "") {
        if(loadImportFile(editFileImport->text())) {
            QMessageBox::information(this, "Information",
                                     "import successful !");
        }
    } else {
        QMessageBox::warning(this, "Warning",
                             "Please select file before import !");
    }
}

void ChargEditor::onBtnExport()
{
    if(editFileExport->text() != "") {
        if(exportDataToFile(editFileExport->text())) {
            QMessageBox::information(this, "Information",
                                     "export successful !");
        }
    } else {
        QMessageBox::warning(this, "Warning",
                             "Please select file before export !");
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
        rowId->setReadOnly(true);
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
}
