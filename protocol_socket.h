#ifndef PROTOCOL_SOCKET_H
#define PROTOCOL_SOCKET_H
#include <QObject>
#include <QtNetwork>

struct TFrame {
    QByteArray frame;
    QHostAddress address;
    quint16 port;
};

class QUdpSocket;
class QTimer;

class CProtocolSocket : public QObject{
    Q_OBJECT
public:
    explicit CProtocolSocket(QObject* parent);
    ~CProtocolSocket();

    void write(const QByteArray& data, const QHostAddress& host, quint16 port);
    QList<TFrame> frameList();
signals:
    void frameReady();
public slots:
    void onReadyRead();
    void onTimeout();
private:
    QUdpSocket* m_socket;
    QMap<QString, QByteArray> m_cacheMap;
    QMap<QString, QMap<quint16, QMap<quint16, QByteArray>>> m_frameMap;
    QList<TFrame> m_frameList;
    quint16 m_index;
    QTimer* m_timer;
};

#endif // PROTOCOL_SOCKET_H
