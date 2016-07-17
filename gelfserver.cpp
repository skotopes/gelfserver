#include "gelfserver.h"
#include <QJsonDocument>
#include <QUdpSocket>
#include <QDebug>

#include <zlib.h>

#define GZIP_CHUNK_SIZE 32 * 1024

GELFServer::GELFServer(QObject *parent) : QObject(parent), udp_socket(new QUdpSocket(this))
{
    udp_socket->bind(QHostAddress::LocalHost, 12201);
    connect(udp_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

void GELFServer::readPendingDatagrams()
{
    while (udp_socket->hasPendingDatagrams()) {
        QByteArray datagram, data;
        QHostAddress sender;
        quint16 senderPort;
        // Recieve datagram
        datagram.resize(udp_socket->pendingDatagramSize());
        udp_socket->readDatagram(
                    datagram.data(),
                    datagram.size(),
                    &sender,
                    &senderPort
                    );
        // check type
        if (datagram[0] == (char)0x1e && datagram[1] == (char)0x0f) {
            qWarning() << this
                     << "Datagram from" << sender
                     << "port" << senderPort
                     << "Chunked messages is not yet supported.";
            continue;
        }
        // Decompress payload
        if (!decompress(datagram, data)) {
            qWarning() << this
                     << "Datagram from" << sender
                     << "port" << senderPort
                     << "Failed to decompress data" << data;
            continue;
        }
        // Parse message json
        QJsonDocument message_document = QJsonDocument::fromJson(data);
        if (!message_document.isObject()) {
            qWarning() << this << "Inavlid GELF message: root is not object";
            return;
        }
        // Validate GELF structure
        QJsonObject message_object = message_document.object();
        QStringList keys = message_object.keys();
        if (
                !keys.contains("version")
                || !keys.contains("host")
                || !keys.contains("short_message")
                || !keys.contains("full_message")
                || !keys.contains("timestamp")
                || !keys.contains("level")
                ) {
            qWarning() << this
                     << "Ivalid GELF"
                     << "from" << sender
                     << "port" << senderPort
                     << "keys" << keys;
        }
        // emit message
        emit message(message_object);
    }
}

bool GELFServer::decompress(QByteArray &input, QByteArray &output)
{
    if (input.length() > 0) {
        // Prepare inflater status
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;
        // Initialize inflater
        int ret = inflateInit2(&strm, 32 + MAX_WBITS);
        if (ret != Z_OK)
            return false;
        // Extract pointer to input data
        char *input_data = input.data();
        int input_data_left = input.length();
        // Decompress data until available
        do {
            // Determine current chunk size
            int chunk_size = qMin(GZIP_CHUNK_SIZE, input_data_left);
            // Check for termination
            if (chunk_size <= 0)
                break;
            // Set inflater references
            strm.next_in = (unsigned char *)input_data;
            strm.avail_in = chunk_size;
            // Update interval variables
            input_data += chunk_size;
            input_data_left -= chunk_size;

            // Inflate chunk and cumulate output
            do {
                // Declare vars
                char out[GZIP_CHUNK_SIZE];

                // Set inflater references
                strm.next_out = (unsigned char *)out;
                strm.avail_out = GZIP_CHUNK_SIZE;

                // Try to inflate chunk
                ret = inflate(&strm, Z_NO_FLUSH);

                switch (ret) {
                case Z_NEED_DICT:
                    qWarning() << this << "Z_NEED_DICT";
                    ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                    qWarning() << this << "Z_DATA_ERROR";
                case Z_MEM_ERROR:
                    qWarning() << this << "Z_MEM_ERROR";
                case Z_STREAM_ERROR:
                    qWarning() << this << "Z_STREAM_ERROR";
                    // Clean-up
                    inflateEnd(&strm);
                    // Return
                    return false;
                }

                // Determine decompressed size
                int have = (GZIP_CHUNK_SIZE - strm.avail_out);

                // Cumulate result
                if (have > 0) output.append((char *)out, have);
            } while (strm.avail_out == 0);
        } while (ret != Z_STREAM_END);

        // Clean-up
        inflateEnd(&strm);

        // Return
        return (ret == Z_STREAM_END);
    } else {
        return true;
    }
}
