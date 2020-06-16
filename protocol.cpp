#include "protocol.h"
#include <QDebug>
#include <QMap>

QByteArray uint16ToByteArray(quint16 i) {
    QByteArray array;
    array.append(char(i));
    array.append(char(i >> 8));
    return array;
}

quint16 byteArrayToUint16(const QByteArray& array) {
    if (array.size() != 2) {
        return 0;
    }
    quint16 i = 0;
    i = array[0];
    i = i | (quint16(array[1]) << 8);
    return i;
}

QList<QByteArray> CProtocolHelper::pack(const QByteArray& buffer, quint16 index) {
    QList<QByteArray> frameList;
    int32_t size = buffer.size();
    if (size <= 0xFF) {
        QByteArray frame;
        frame.append(char(0xAA));
        frame.append(char(0xAA));
        frame.append(char(0x01));
        frame.append(char(0x00));
        frame.append(char(size));
        frame.append(buffer);
        frameList.append(frame);
    } else {
        int32_t times = size / 0xFF;
        if (size % 0xFF > 0) {
            ++times;
        }
        for (int32_t i = 0; i < times; ++i) {
            QByteArray temp;
            if (i == times - 1) {
                temp = buffer.mid(i * 0xFF);
            } else {
                temp = buffer.mid(i * 0xFF, 0xFF);
            }
            QByteArray frame;
            frame.append(char(0xAA));
            frame.append(char(0xAA));
            frame.append(uint16ToByteArray(quint16(times)));
            frame.append(uint16ToByteArray(quint16(i)));
            frame.append(uint16ToByteArray(index));
            frame.append(char(temp.size()));
            frame.append(temp);
            frameList.append(frame);
        }
    }
    return frameList;
}

bool findHeader(QByteArray& cache) {
    QByteArray header;
    header.append(char(0xAA));
    header.append(char(0xAA));
    int i = cache.indexOf(header);
    if (i == -1) {
        cache.clear();
        return false;
    } else if (i != 0) {
        cache = cache.mid(i);
        return true;
    }
    return true;
}



QList<QByteArray> CProtocolHelper::parse(QByteArray& cache, QMap<quint16, QMap<quint16, QByteArray>>& frameMap) {
    QList<QByteArray> frameList;
    while (findHeader(cache)) {
        if (cache.size() < ESingleDataIndex) {
            return frameList;
        }
        quint16 count = byteArrayToUint16(cache.mid(ECountIndex, 2));
        if (count == 0) {
            cache = cache.mid(ECountIndex);
            continue;
        } else if (count == 1) {
            quint8 dataLength = cache[ESingleDataLengthIndex];
            const QByteArray& data = cache.mid(ESingleDataIndex);
            if (data.size() < dataLength) {
                return frameList;
            }
            const QByteArray& frame = data.mid(0, dataLength);
            frameList.push_back(frame);
            cache = cache.mid(dataLength + ESingleDataIndex);
            continue;
        } else {
            if (cache.size() < EDataIndex) {
                return frameList;
            }
            quint8 dataLength = cache[EDataLengthIndex];
            quint16 packageIndex = byteArrayToUint16(cache.mid(EPackageIndex, 2));
            quint16 frameIndex = byteArrayToUint16(cache.mid(EFrameIndex, 2));
            const QByteArray& data = cache.mid(EDataIndex);
            if (data.size() < dataLength) {
                return frameList;
            }
            const QByteArray& frame = data.mid(0, dataLength);
            auto it = frameMap.find(frameIndex);
            if (it == frameMap.end()) {
                frameMap.insert(frameIndex, QMap<quint16, QByteArray>());
                it = frameMap.find(frameIndex);
            }
            QMap<quint16, QByteArray>& packageMap = it.value();
            packageMap.insert(packageIndex, frame);
            QByteArray temp;
            if (packageMap.size() == count) {
                for (int i = 0; i < count; ++i) {
                    auto it = packageMap.find(i);
                    if (it == packageMap.end()) {
                        break;
                    }
                    temp.append(it.value());
                }
                frameList.append(temp);
                frameMap.remove(frameIndex);
            }
            cache = cache.mid(dataLength + EDataIndex);
            continue;
        }
    }
    return frameList;
}
