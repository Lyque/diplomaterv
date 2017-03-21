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

    // ToDo: Signalok és slotok megírása a GUI elemek aktualizálására.
    void led0ChangedSlot(bool isOn);
    void led1ChangedSlot(bool isOn);
    void switch0ChangedSlot(bool isOn);
    void switch1ChangedSlot(bool isOn);
    void localTempChangedSlot(int value);

signals:
    void led0ChangedSignal(bool isOn);
    void led1ChangedSignal(bool isOn);
    void switch0ChangedSignal(bool isOn);
    void switch1ChangedSignal(bool isOn);
    void localTempChangedSignal(int value);

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
