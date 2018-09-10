#ifndef DEVELOP_H
#define DEVELOP_H

#include <QtWidgets>
#include <QTimer>
#include <QtCharts>
#include <xlsx/xlsxdocument.h>
#include <layer2.h>
#include <config/config.h>
#include <controldock.h>

class Develop : public QMainWindow
{
    Q_OBJECT
public:
    explicit Develop(QWidget *parent = 0);
    ~Develop();
    void initVariable();
    void createElement();
    void createContent();
    void createLayout();
    void createConnection();

signals:

    void signalConnectionEvent(int event);
    void signalChargerData(const QString &string);
    void signalEnablePanel(bool en);
private slots:
    void openConnection();
    void closeConnection();
    void showControlPanel();
    void recvChargerDataEvent(const QString &string);
    void recvConnectionEvent(int event);
    void onTimerClb();
    void onShowStatusMsg(const QString &status);
    void readData();

public slots:
    void onSendData(QString data);

private:

    QToolBar *toolBar;
    QVBoxLayout *mainLayout;
    QWidget *centerWidget;

    QToolButton *btnOpenConnection,
                *btnCloseConnection,
                *btnShowControlPanel;

    ControlDock *control;

    QToolButton *btnClear, *btnSave;

    QSplineSeries *series;
    QChart *ivChart;
    QValueAxis *ivXAxis;

    QSerialPort *m_serial = nullptr;

    std::shared_ptr<ibc::layer2> ibc_obj_;

    QTimer  hTimerControl;

};




#endif // DEVELOP_H
