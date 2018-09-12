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
    numLine = 0;
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
    chart->setTitle("GRAPH");
    iSeries->setName("current");
    vSeries->setName("voltage");
    layoutEditor = new QVBoxLayout;

    hSplitter = new QSplitter(Qt::Horizontal);
    vSplitter = new QSplitter(Qt::Vertical);

    groupEditor = new QGroupBox("CHARACTER EDITOR", this);
    groupControl = new QGroupBox("CONTROL", this);

//    btnAddPoint = new QPushButton("add point", groupControl);
//    btnRemPoint = new QPushButton("del point", groupControl);
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

//    for(int i = 0; i < 1; i++) {
//        LineEditor *row = new LineEditor;
//        layoutEditor->addWidget(row);
//    }

    //layoutEditor->addStretch(1);
    layoutEditor->setContentsMargins(0,0,0,0);

    QList<int> heights;
    heights.push_back(200);
    heights.push_back(600);
    vSplitter->setSizes(heights);
    QList<int> widths;
    widths.push_back(600);
    widths.push_back(350);
    hSplitter->setSizes(widths);
    setCentralWidget(hSplitter);


    QGridLayout *gridLayout = new QGridLayout;
    groupControl->setLayout(gridLayout);
    QPushButton *btnAdd = new QPushButton("ADD");
    QPushButton *btnDel = new QPushButton("DEL");
    gridLayout->addWidget(btnAdd, 0, 1);
    gridLayout->addWidget(btnDel, 1, 1);

    gridLayout->setColumnStretch(0, 1);

    connect(btnAdd, SIGNAL(clicked(bool)), this, SLOT(onBtnAddPoint()));
    connect(btnDel, SIGNAL(clicked(bool)), this, SLOT(onBtnRemPoint()));
}

void ChargEditor::createContent()
{

}

void ChargEditor::createConnection()
{
//    connect(btnAddPoint, SIGNAL(clicked(bool)), this, SLOT(onBtnAddPoint()));
//    connect(btnRemPoint, SIGNAL(clicked(bool)), this, SLOT(onBtnRemPoint()));
}

void ChargEditor::onBtnAddPoint()
{
    if(numLine == 0) {
        layoutEditor->addWidget(new LineEditor(numLine, this));
        layoutEditor->addStretch(1);
    } else {
        layoutEditor->insertWidget(numLine, new LineEditor(numLine, this));
    }
    numLine++;
}

void ChargEditor::onBtnRemPoint()
{
    qDebug() << "count = " << layoutEditor->count();

}



LineEditor::LineEditor(int id, QWidget *parent) :
    QWidget(parent)
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
    }
    hLayout->addStretch(1);
    //QString wgstyle = QString("background-color: yellow;");
    //setStyleSheet(wgstyle);

    setLayout(hLayout);
}
