#include "hardware.h"

Hardware::Hardware(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    {
    QGroupBox *groupFactory = new QGroupBox("Select Hardware version");
    QHBoxLayout *hLayout = new QHBoxLayout;

    QRadioButton *rbtnHwVer1 = new QRadioButton("Hardware verion 1");
    QRadioButton *rbtnHwVer2 = new QRadioButton("Hardware version 2");

    hLayout->addWidget(rbtnHwVer1, 0, Qt::AlignLeft);
    hLayout->addWidget(rbtnHwVer2, 1, Qt::AlignLeft);
    groupFactory->setLayout(hLayout);
    mainLayout->addWidget(groupFactory, 1, Qt::AlignTop | Qt::AlignLeft);
    }
}
