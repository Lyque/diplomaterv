#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QMessageBox>
#include <QLabel>
#include <QPixmap>

#include "settingsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);

    void on_led0CheckBox_toggled(bool checked);

    void on_led1CheckBox_toggled(bool checked);

private:
    void initActionsConnections();
    void showStatusMessage(const QString &message);

private:
    Ui::MainWindow *ui;
    QLabel *status;
    SettingsDialog *settings;
    QSerialPort *serial;

    QPixmap ledOn;
    QPixmap ledOff;
    QPixmap switchOn;
    QPixmap switchOff;
};

#endif // MAINWINDOW_H
