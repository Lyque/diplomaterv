#ifndef BGLIB_H
#define BGLIB_H

#include <QObject>
#include <QSerialPort>

class BGLib : public QObject
{
    Q_OBJECT
public:
    explicit BGLib(QObject *parent = 0);

// Response és Event paraméterváltozók
    // HelloEventArgs
    class HelloEventArgs;

    class HelloEventArgs
    {
    public:
        HelloEventArgs();
    };
    // ConnectDirectEventArgs
    class ConnectDirectEventArgs;

    class ConnectDirectEventArgs
    {
    public:
        quint16 result;
        char connection_handle;

        ConnectDirectEventArgs(quint16 result,char connection_handle);
    };
    // BootEventArgs
    class BootEventArgs;

    class BootEventArgs
    {
    public:
        quint16 major;
        quint16 minor;
        quint16 patch;
        quint16 build;
        quint16 ll_version;
        char    protocol_version;
        char    hw;

        BootEventArgs(quint16 major, quint16 minor, quint16 patch, quint16 build, quint16 ll_version, char protocol_version, char hw);
    };
    // ConnectionStatusEventArgs
    class ConnectionStatusEventArgs;

    class ConnectionStatusEventArgs
    {
    public:
        char connection;
        char flags;
        QByteArray address;
        char address_type;
        quint16 conn_interval;
        quint16 timeout;
        quint16 latency;
        char bonding;

        ConnectionStatusEventArgs(char connection, char flags, QByteArray address, char address_type, quint16 conn_interval, quint16 timeout, quint16 latency, char bonding);
    };
    // ProcedureCompletedEventArgs
    class ProcedureCompletedEventArgs;

    class ProcedureCompletedEventArgs
    {
    public:
        char connection;
        quint16 result;
        quint16 chrhandle;

        ProcedureCompletedEventArgs(char connection, quint16 result, quint16 chrhandle);
    };
    // AttributeValueEventArgs
    class AttributeValueEventArgs;

    class AttributeValueEventArgs
    {
    public:
        char connection;
        quint16 atthandle;
        char type;
        QByteArray value;

        AttributeValueEventArgs(char connection, quint16 atthandle, char type, QByteArray value);
    };

private:
    char bgapiBuffer[65];
    int bgapiRXBufferPos = 0;
    int bgapiRXDataLen = 0;
    bool bgapiPacketMode = false;
    bool parserBusy = false;

public:
    void SetBusy(bool isBusy);
    bool IsBusy();
    void SetPacketMode(bool packetMode);
    quint16 Parse(char ch);

    // Commands
    QByteArray BLECommandSystemReset(char boot_in_dfu);
    QByteArray BLECommandSystemHello();
    QByteArray BLECommandATTClientReadByHandle(char connection, quint16 chrhandle);
    QByteArray BLECommandATTClientAttributeWrite(char connection, quint16 atthandle, QByteArray data);
    QByteArray BLECommandGAPConnectDirect(char address[6], char addr_type, quint16 conn_interval_min, quint16 conn_interval_max, quint16 timeout, quint16 latency);
    QByteArray BLECommandConnectionDisconnect(char connection);

    int SendCommand(QSerialPort *port, QByteArray cmd);
signals:
    void SystemHelloResponse(QObject *sender, BGLib::HelloEventArgs e);
    void GAPConnectDirectResponse(QObject *sender, BGLib::ConnectDirectEventArgs e);

    void SystemBootEvent(QObject *sender, BGLib::BootEventArgs e);
    void ConnectionStatusEvent(QObject *sender, BGLib::ConnectionStatusEventArgs e);
    void ATTClientProcedureCompletedEvent(QObject *sender, BGLib::ProcedureCompletedEventArgs e);
    void ATTClientAttributeValueEvent(QObject *sender, BGLib::AttributeValueEventArgs e);
/** Szükséges signalok:
 * Response:
 *      SystemHelloResponse
 *      GAPConnectDirectResponse
 *
 * Event:
 *      SystemBootEvent
 *      ConnectionStatusEvent
 *      ATTClientProcedureCompletedEvent
 *      ATTClientAttributeValueEvent
 **/

 /** A hozzájuk tartozó struktúrák:
  * Response:
  *     HelloEventArgs:
  *         -
  *
  *     ConnectDirectEventArgs:
  *         - uint16 result;
  *         - byte connection_handle;
  *
  * Event:
  *     BootEventArgs:
  *         - uint16 major;
  *         - uint16 minor;
  *         - uint16 patch;
  *         - uint16 build;
  *         - uint16 ll_version;
  *         - byte protocol_version;
  *         - byte hw;
  *
  *     StatusEventArgs:
  *         - byte connection;
  *         - byte flags;
  *         - byte[] address;
  *         - byte address_type;
  *         - uint16 conn_interval;
  *         - uint16 timeout;
  *         - uint16 latency;
  *         - byte bonding;
  *
  *     ProcedureCompletedEventArgs:
  *         - byte connection;
  *         - uint16 result;
  *         - uint16 chrhandle;
  *
  *     AttributeValueEventArgs:
  *         - byte connection;
  *         - uint16 atthandle;
  *         - type;
  *         - byte[] value;
  **/

  /** Egyéb megvalósitandó függvények:
   * Parse:
   *    Egyesével kapja a karaktereket. Az első karaktert megnézi, hogy a protokollnak megfelelő értékű-e (0x00 vagy 0x80).
   *    Ezután a következő karakterekből meghatározza a beérkező üzenet hosszát. Miután megérkezett a várt számú karakter,
   *    azután megkezdődik az üzenet feldolgozása. Ez egy igencsak nagy if fában történik. Amint megtalálta az üzenethez tartozó
   *    feladatot, meghivja a kezelő-függvényt (jelen esetben jelez a signallal). A signalnak két paramétere van: a küldő objektum,
   *    illetve a függvényhez tartozó strukturált üzenet (amit célszerű osztályként definiálni).
   *
   * Command üzenetek:
   *    Ezek pár soros függvények, amik hardkódolva tartalmazzák a parancshoz tartozó tényleges üzenetet. Pár függvény paramétert
   *    is vesz át, amit beilleszt a byte-sorozatba.
   *
   * Busy:
   *    Tagváltozóban tárolni kell a parser aktuális állapotát (foglalt vagy sem).
   *
   * Packet mode:
   *    A kommunikáció történhez packet mode-ban, illetve flow-control mode-ban. Ezt szintén tagváltozó jelzi.
   *
   * SendCommand:
   *    Ez a függvény végzi a karakterek küldését. Paraméterként a küldő interface-t és a kiküldendő parancsot kapja.
   *    Packet mode-ban ez a függvény teszi hozzá a parancshoz a hosszát jelző byte-ot.
   **/
public slots:

};

#endif // BGLIB_H
