#include "bglib.h"

#include <QDebug>

BGLib::BGLib(QObject *parent) :
    QObject(parent)
{
}

BGLib::HelloEventArgs::HelloEventArgs()
{

}

BGLib::ConnectDirectEventArgs::ConnectDirectEventArgs(quint16 result, char connection_handle)
{
    this->result = result;
    this->connection_handle = connection_handle;
}

BGLib::BootEventArgs::BootEventArgs(quint16 major, quint16 minor, quint16 patch, quint16 build, quint16 ll_version, char protocol_version, char hw)
{
    this->major = major;
    this->minor = minor;
    this->patch = patch;
    this->build = build;
    this->ll_version = ll_version;
    this->protocol_version = protocol_version;
    this->hw = hw;
}

BGLib::ConnectionStatusEventArgs::ConnectionStatusEventArgs(char connection, char flags, QByteArray address, char address_type, quint16 conn_interval, quint16 timeout, quint16 latency, char bonding)
{
    this->connection = connection;
    this->flags = flags;
    this->address = address;
    this->address_type = address_type;
    this->conn_interval = conn_interval;
    this->timeout = timeout;
    this->latency = latency;
    this->bonding = bonding;
}

BGLib::ProcedureCompletedEventArgs::ProcedureCompletedEventArgs(char connection, quint16 result, quint16 chrhandle)
{
    this->connection = connection;
    this->result = result;
    this->chrhandle = chrhandle;
}

BGLib::AttributeValueEventArgs::AttributeValueEventArgs(char connection, quint16 atthandle, char type, QByteArray value)
{
    this->connection = connection;
    this->atthandle = atthandle;
    this->type = type;
    this->value = value;
}

QByteArray BGLib::BLECommandSystemReset(char boot_in_dfu)
{
    char cmd[] = {0, 1, 0, 0, boot_in_dfu};
    return QByteArray(cmd, sizeof(cmd));
}

QByteArray BGLib::BLECommandSystemHello()
{
    char cmd[] = {0, 0, 0, 1};
    return QByteArray(cmd, sizeof(cmd));
}

QByteArray BGLib::BLECommandATTClientReadByHandle(char connection, quint16 chrhandle)
{
    char cmd[] = {0, 3, 4, 4, connection, (char)(chrhandle & 0xFF), (char)((chrhandle >> 8) & 0xFF)};
    return QByteArray(cmd, sizeof(cmd));
}

QByteArray BGLib::BLECommandATTClientAttributeWrite(char connection, quint16 atthandle, QByteArray data)
{
    char cmd[8+data.length()];
    cmd[0] = 0;
    cmd[1] = 4+data.length();
    cmd[2] = 4;
    cmd[3] = 5;
    cmd[4] = connection;
    cmd[5] = (char)(atthandle & 0xFF);
    cmd[6] = (char)((atthandle >> 8) & 0xFF);
    cmd[7] = data.length();
    for(int i=0;i<data.length();i++)
        cmd[i+8] = data.at(i);

    return QByteArray(cmd, sizeof(cmd));
}

QByteArray BGLib::BLECommandGAPConnectDirect(char address[6], char addr_type, quint16 conn_interval_min, quint16 conn_interval_max, quint16 timeout, quint16 latency)
{
    char cmd[19];
    cmd[0] = 0;
    cmd[1] = 15;
    cmd[2] = 6;
    cmd[3] = 3;
    cmd[4] = address[0];
    cmd[5] = address[1];
    cmd[6] = address[2];
    cmd[7] = address[3];
    cmd[8] = address[4];
    cmd[9] = address[5];
    cmd[10] = addr_type;
    cmd[11] = (char)(conn_interval_min & 0xFF);
    cmd[12] = (char)((conn_interval_min >> 8) & 0xFF);
    cmd[13] = (char)(conn_interval_max & 0xFF);
    cmd[14] = (char)((conn_interval_max >> 8) & 0xFF);
    cmd[15] = (char)(timeout & 0xFF);
    cmd[16] = (char)((timeout >> 8) & 0xFF);
    cmd[17] = (char)(latency & 0xFF);
    cmd[18] = (char)((latency >> 8) & 0xFF);

    return QByteArray(cmd, sizeof(cmd));
}

QByteArray BGLib::BLECommandConnectionDisconnect(char connection)
{
    char cmd[] = {0, 1, 3 ,0, connection};

    return QByteArray(cmd, sizeof(cmd));
}

void BGLib::SetBusy(bool isBusy)
{
    this->parserBusy = isBusy;
}

bool BGLib::IsBusy()
{
    return this->parserBusy;
}

void BGLib::SetPacketMode(bool packetMode)
{
    this->bgapiPacketMode = packetMode;
}

quint16 BGLib::Parse(char ch)
{
    // Check packet position
    if(bgapiRXBufferPos == 0){
        // Beginning of packet, check for correct framing/expected byte(s)
        if((ch & 0x78) == 0x00) {
            bgapiBuffer[bgapiRXBufferPos++] = ch;
        } else {
            return 1;
        }
    } else {
        bgapiBuffer[bgapiRXBufferPos++] = ch;
        if(bgapiRXBufferPos == 2) {
            bgapiRXDataLen = ch + ((bgapiBuffer[0] & 0x03) << 8);
        } else if(bgapiRXBufferPos == bgapiRXDataLen + 4) {
            // Check packet type
            if((bgapiBuffer[0] & 0x80) == 0){
                // 0x00 = Response packet
                if(bgapiBuffer[2] == 0) {
                    if(bgapiBuffer[3] == 1){
                        emit SystemHelloResponse(this, HelloEventArgs());
                    }
                }
                else if(bgapiBuffer[2] == 6){
                    if(bgapiBuffer[3] == 3){
                        emit GAPConnectDirectResponse(this, ConnectDirectEventArgs((quint16)(bgapiBuffer[4] | (bgapiBuffer[5] << 8)), bgapiBuffer[6]));
                    }
                }
            }
            else{
                // 0x80 = Event packet
                if(bgapiBuffer[2] == 0){
                    if(bgapiBuffer[3] == 0){
                        emit SystemBootEvent(this, BootEventArgs(
                                                (quint16)(bgapiBuffer[4] | (bgapiBuffer[5] << 8)),
                                                (quint16)(bgapiBuffer[6] | (bgapiBuffer[7] << 8)),
                                                (quint16)(bgapiBuffer[8] | (bgapiBuffer[9] << 8)),
                                                (quint16)(bgapiBuffer[10] | (bgapiBuffer[11] << 8)),
                                                (quint16)(bgapiBuffer[12] | (bgapiBuffer[13] << 8)),
                                                bgapiBuffer[14],
                                                bgapiBuffer[15]
                                            ));
                    }
                }
                else if(bgapiBuffer[2] == 3){
                    if(bgapiBuffer[3] == 0){
                        char address[] = {bgapiBuffer[6], bgapiBuffer[7], bgapiBuffer[8], bgapiBuffer[9], bgapiBuffer[10], bgapiBuffer[11]};
                        emit ConnectionStatusEvent(this, ConnectionStatusEventArgs(
                                                bgapiBuffer[4],
                                                bgapiBuffer[5],
                                                QByteArray(address, sizeof(address)),
                                                bgapiBuffer[12],
                                                (quint16)(bgapiBuffer[13] | (bgapiBuffer[14] << 8)),
                                                (quint16)(bgapiBuffer[15] | (bgapiBuffer[16] << 8)),
                                                (quint16)(bgapiBuffer[17] | (bgapiBuffer[18] << 8)),
                                                bgapiBuffer[19]
                                            ));
                    }
                }
                else if(bgapiBuffer[2] == 4){
                    if(bgapiBuffer[3] == 1){
                        emit ATTClientProcedureCompletedEvent(this, ProcedureCompletedEventArgs(
                                                bgapiBuffer[4],
                                                (quint16)(bgapiBuffer[5] | (bgapiBuffer[6] << 8)),
                                                (quint16)(bgapiBuffer[7] | (bgapiBuffer[8] << 8))
                                            ));
                    }
                    else if(bgapiBuffer[3] == 5){
                        char value[bgapiRXBufferPos-9];
                        for(int i=0;i<sizeof(value);i++)
                            value[i] = bgapiBuffer[i+9];
                        emit ATTClientAttributeValueEvent(this, AttributeValueEventArgs(
                                                bgapiBuffer[4],
                                                (quint16)(bgapiBuffer[5] | (bgapiBuffer[6] << 8)),
                                                bgapiBuffer[7],
                                                QByteArray(value, sizeof(value))
                                            ));
                    }
                }
            }

            bgapiRXBufferPos = 0;
        }
    }
    return 0;
}

int BGLib::SendCommand(QSerialPort *port, QByteArray cmd)
{
    char lengthChar[] = {(char)(cmd.length() & 0xFF)};
    QByteArray length(lengthChar, sizeof(lengthChar));

    if(bgapiPacketMode)
        port->write(length);

    port->write(cmd);

    return 0;
}
