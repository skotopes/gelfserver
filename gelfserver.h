#ifndef GELFSERVER_H
#define GELFSERVER_H

#include <QObject>
#include <QJsonObject>

class QUdpSocket;

class GELFServer : public QObject
{
    Q_OBJECT
public:
    explicit GELFServer(QObject *parent = 0);

private:
    QUdpSocket *udp_socket;
    bool decompress(QByteArray &input, QByteArray &output);

signals:
    void message(QJsonObject);

public slots:

private slots:
    void readPendingDatagrams();
};

#endif // GELFSERVER_H
