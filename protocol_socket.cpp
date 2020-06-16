#include "protocol_socket.h"
#include "protocol.h"
#include <QTimer>

CProtocolSocket::CProtocolSocket(QObject* parent)
    : QObject(parent)
    , m_socket(new QUdpSocket(this))
    , m_cacheMap()
    , m_frameMap()
    , m_frameList()
    , m_index(0)
    , m_timer(new QTimer(this)){

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_timer->start(5 * 60 * 1000);
}

CProtocolSocket::~CProtocolSocket() {

}

void CProtocolSocket::write(const QByteArray &data, const QHostAddress &host, quint16 port) {
    const QList<QByteArray>& frameList = CProtocolHelper::pack(data, ++m_index);
    auto it = frameList.begin();
    for (; it != frameList.end(); ++it) {
        m_socket->writeDatagram(*it, host, port);
    }
}

void CProtocolSocket::onReadyRead() {
    while (m_socket->hasPendingDatagrams()) {
        QByteArray buffer;
        buffer.resize(m_socket->pendingDatagramSize());
        QHostAddress address;
        quint16 port;
        m_socket->readDatagram(buffer.data(), buffer.size(), &address, &port);
        QString key;
        key += address.toString() + ":" + QString::number(port);

        auto cacheIt = m_cacheMap.find(key);
        if (cacheIt == m_cacheMap.end()) {
            m_cacheMap.insert(key, QByteArray());
            cacheIt = m_cacheMap.find(key);
        }
        cacheIt->append(buffer);

        auto frameIt = m_frameMap.find(key);
        if (frameIt == m_frameMap.end()) {
            m_frameMap.insert(key, QMap<quint16, QMap<quint16, QByteArray>>());
            frameIt = m_frameMap.find(key);
        }

        const QList<QByteArray>& frameList = CProtocolHelper::parse(cacheIt.value(), frameIt.value());
        if (!frameList.empty()) {
            auto it = frameList.begin();
            for (; it != frameList.end(); ++it) {
                TFrame frame;
                frame.address = address;
                frame.port = port;
                frame.frame = *it;
                m_frameList.push_back(frame);
                emit frameReady();
            }
        }
    }
}

QList<TFrame> CProtocolSocket::frameList() {
    QList<TFrame> frameList(m_frameList);
    m_frameList.clear();
    return frameList;
}

void CProtocolSocket::onTimeout() {
    m_cacheMap.clear();
    m_frameMap.clear();
    m_timer->start(5 * 60 * 1000);
}
