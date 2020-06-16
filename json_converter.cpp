#include "json_converter.h"
#ifndef JC_USING_JSONCPP
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>

namespace jc {

CJsonDataBase::CJsonDataBase()
    : m_object() {

}

CJsonDataBase::CJsonDataBase(const QJsonObject &object)
    : m_object() {

    fromObject(object);
}

CJsonDataBase::CJsonDataBase(const CJsonDataBase &other)
    : m_object() {

    fromObject(other.toObject());
}

CJsonDataBase::~CJsonDataBase() {
}

bool CJsonDataBase::isNull() const {
    return m_object.isEmpty();
}

std::string CJsonDataBase::toJson() const {
    update();
    QJsonDocument document;
    document.setObject(m_object);
    return document.toJson(QJsonDocument::Compact).toStdString();
}

int32_t CJsonDataBase::fromJson(const std::string& json) {
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &error);
    if (!document.isObject() || QJsonParseError::NoError != error.error) {
        return JSON_ERROR_PARSE;
    }
    m_object = document.object();
    parse();
    return JSON_ERROR_OK;
}

const QJsonObject& CJsonDataBase::toObject() const {
    update();
    return m_object;
}

void CJsonDataBase::fromObject(const QJsonObject &object) {
    m_object = object;
    parse();
}

void CJsonDataBase::clear() {
    QStringList keyList = m_object.keys();
    for (QString key : keyList) {
        m_object.remove(key);
    }
}

bool CJsonDataBase::isExists(const char* name) const {
    return m_object.contains(name) && !m_object.value(name).isNull();
}

void CJsonDataBase::set(const char *name, int32_t value) {
    m_object.insert(name, value);
}

void CJsonDataBase::get(const char *name, int32_t &value) const {
    value = 0;
    if (m_object.contains(name) && !m_object.value(name).isNull()) {
        value = m_object.value(name).toInt();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<int32_t>& values) {
    QJsonArray array;
    for (auto it = values.begin(); it != values.end(); ++it) {
        array.append(*it);
    }
    m_object.insert(name, array);
}

void CJsonDataBase::get(const char* name, std::vector<int32_t>& values) {
    values.clear();
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isArray()) {
            const QJsonArray& array = jsonValue.toArray();
            if (!array.empty()) {
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    values[i] = array.at(i).toInt();
                }
            }
        }
    }
}

void CJsonDataBase::set(const char* name, uint32_t value) {
    m_object.insert(name, (int32_t)value);
}

void CJsonDataBase::get(const char* name, uint32_t& value) const {
    value = 0;
    if (m_object.contains(name) && !m_object.value(name).isNull()) {
        value = m_object.value(name).toInt();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<uint32_t>& values) {
    QJsonArray array;
    for (auto it = values.begin(); it != values.end(); ++it) {
        array.append((int32_t)*it);
    }
    m_object.insert(name, array);
}

void CJsonDataBase::get(const char* name, std::vector<uint32_t>& values) const {
    values.clear();
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isArray()) {
            const QJsonArray& array = jsonValue.toArray();
            if (!array.empty()) {
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    values[i] = array.at(i).toInt();
                }
            }
        }
    }
}

void CJsonDataBase::set(const char* name, int64_t value) {
    m_object.insert(name, QJsonValue((qint64)value));
}

void CJsonDataBase::get(const char* name, int64_t& value) const {
    value = 0;
    if (m_object.contains(name) && !m_object.value(name).isNull()) {
        value = m_object.value(name).toVariant().toLongLong();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<int64_t>& values) {
    QJsonArray array;
    for (auto it = values.begin(); it != values.end(); ++it) {
        array.append(QJsonValue((qint64)*it));
    }
    m_object.insert(name, array);
}

void CJsonDataBase::get(const char* name, std::vector<int64_t>& values) {
    values.clear();
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isArray()) {
            const QJsonArray& array = jsonValue.toArray();
            if (!array.empty()) {
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    values[i] = array.at(i).toVariant().toLongLong();
                }
            }
        }
    }
}

void CJsonDataBase::set(const char *name, uint64_t value) {
    m_object.insert(name, QJsonValue((qint64)value));
}

void CJsonDataBase::get(const char *name, uint64_t &value) const {
    value = 0;
    if (m_object.contains(name) && !m_object.value(name).isNull()) {
        value = m_object.value(name).toVariant().toLongLong();
    }
}

void CJsonDataBase::set(const char *name, const std::vector<uint64_t> &values) {
    QJsonArray array;
    for (auto it = values.begin(); it != values.end(); ++it) {
        array.append(QJsonValue((qint64)(*it)));
    }
    m_object.insert(name, array);
}

void CJsonDataBase::get(const char *name, std::vector<uint64_t> &values) {
    values.clear();
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isArray()) {
            const QJsonArray& array = jsonValue.toArray();
            if (!array.empty()) {
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    values[i] = array.at(i).toVariant().toLongLong();
                }
            }
        }
    }
}

void CJsonDataBase::set(const char* name, bool value) {
    m_object.insert(name, value);
}

void CJsonDataBase::get(const char* name, bool& value) const {
    value = false;
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isBool()) {
            value = jsonValue.toBool();
        }
    }
}

void CJsonDataBase::set(const char* name, const std::vector<bool>& values) {
    QJsonArray array;
    for (auto it = values.begin(); it != values.end(); ++it) {
        array.append(*it);
    }
    m_object.insert(name, array);
}

void CJsonDataBase::get(const char* name, std::vector<bool>& values) const {
    values.clear();
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isArray()) {
            const QJsonArray& array = jsonValue.toArray();
            if (!array.empty()) {
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    values[i] = array.at(i).toBool();
                }
            }
        }
    }
}

void CJsonDataBase::set(const char* name, float value) {
    m_object.insert(name, value);
}

void CJsonDataBase::get(const char* name, float& value) const {
    value = 0.0f;
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isDouble()) {
            value = jsonValue.toDouble();
        }
    }
}

void CJsonDataBase::set(const char* name, const std::vector<float>& values) {
    QJsonArray array;
    for (auto it = values.begin(); it != values.end(); ++it) {
        array.append(*it);
    }
    m_object.insert(name, array);
}

void CJsonDataBase::get(const char* name, std::vector<float>& values) const {
    values.clear();
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isArray()) {
            const QJsonArray& array = jsonValue.toArray();
            if (!array.empty()) {
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    values[i] = array.at(i).toDouble();
                }
            }
        }
    }
}

void CJsonDataBase::set(const char* name, double value) {
    m_object.insert(name, value);
}

void CJsonDataBase::get(const char* name, double& value) const {
    value = 0.0f;
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isDouble()) {
            value = jsonValue.toDouble();
        }
    }
}

void CJsonDataBase::set(const char* name, const std::vector<double>& values) {
    QJsonArray array;
    for (auto it = values.begin(); it != values.end(); ++it) {
        array.append(*it);
    }
    m_object.insert(name, array);
}

void CJsonDataBase::get(const char* name, std::vector<double>& values) const {
    values.clear();
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isArray()) {
            const QJsonArray& array = jsonValue.toArray();
            if (!array.empty()) {
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    values[i] = array.at(i).toDouble();
                }
            }
        }
    }
}

void CJsonDataBase::set(const char* name, const std::string& value) {
    m_object.insert(name, QString::fromStdString(value));
}

void CJsonDataBase::get(const char* name, std::string& value) const {
    value = "";
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isString()) {
            value = jsonValue.toString().toStdString();
        }
    }
}

void CJsonDataBase::set(const char* name, const std::vector<std::string>& values) {
    QJsonArray array;
    for (auto it = values.begin(); it != values.end(); ++it) {
        array.append(QString::fromStdString(*it));
    }
    m_object.insert(name, array);
}

void CJsonDataBase::get(const char* name, std::vector<std::string>& values) const {
    values.clear();
    if (m_object.contains(name)) {
        const QJsonValue& jsonValue = m_object.value(name);
        if (jsonValue.isArray()) {
            const QJsonArray& array = jsonValue.toArray();
            if (!array.empty()) {
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    values[i] = array.at(i).toString().toStdString();
                }
            }
        }
    }
}

void CJsonDataBase::set(const char* name, const jc::CJsonDataBase& base) {
    //m_object.insert(name, base.toObject());
    m_mutex.lock();
    auto it = m_objectMap.find(name);
    if (it != m_objectMap.end()) {
        it->second = base;
    } else {
        m_objectMap.insert(std::make_pair(name, base));
    }
    m_mutex.unlock();
}

//void CJsonDataBase::get(const char* name, jc::CJsonDataBase& base) const {
//    if (m_object.contains(name)) {
//        const QJsonValue& jsonValue = m_object.value(name);
//        if (jsonValue.isObject()) {
//            base.fromObject(jsonValue.toObject());
//        }
//    }
//}

void CJsonDataBase::parse() {
    QJsonObject::iterator it = m_object.begin();
    for (; it != m_object.end(); ++it) {
        if (it.value().isObject()) {
            CJsonDataBase child(it.value().toObject());
            m_mutex.lock();
            auto objectIt = m_objectMap.find(it.key().toStdString());
            if (objectIt != m_objectMap.end()) {
                objectIt->second = child;
            }
            else {
                m_objectMap.insert(std::make_pair(it.key().toStdString(), child));
            }
            m_mutex.unlock();
        }
    }
}

void CJsonDataBase::update() const {
    CJsonDataBase* tempThis = const_cast<CJsonDataBase*>(this);
    tempThis->m_mutex.lock();
    auto it = m_objectMap.begin();
    for (; it != m_objectMap.end(); ++it) {
        tempThis->m_object.insert(QString::fromStdString(it->first), it->second.toObject());
    }
    tempThis->m_mutex.unlock();
}

const CJsonDataBase& CJsonDataBase::operator =(const CJsonDataBase& other) {
    if (this == &other) {
        return other;
    }
    fromObject(other.toObject());
    return other;
}

}

#endif
