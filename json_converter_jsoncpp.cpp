#include "json_converter.h"
#ifdef JC_USING_JSONCPP
#include <sstream>
#include <QDebug>

namespace jc {

Json::CharReaderBuilder CJsonDataBase::m_readerBuilder;
Json::StreamWriterBuilder CJsonDataBase::m_writerBuilder;

CJsonDataBase::CJsonDataBase()
    : m_object()
    , m_mutex()
    , m_objectMap() {

    if (m_writerBuilder.settings_["indentation"] != "") {
        m_writerBuilder.settings_["indentation"] = "";
    }
}

CJsonDataBase::CJsonDataBase(const Json::Value &object)
    : m_object()
    , m_mutex()
    , m_objectMap() {

    fromObject(object);
    if (m_writerBuilder.settings_["indentation"] != "") {
        m_writerBuilder.settings_["indentation"] = "";
    }
}

CJsonDataBase::CJsonDataBase(const CJsonDataBase &other)
    : m_object()
    , m_mutex()
    , m_objectMap() {

    fromObject(other.toObject());
    if (m_writerBuilder.settings_["indentation"] != "") {
        m_writerBuilder.settings_["indentation"] = "";
    }
}

CJsonDataBase::~CJsonDataBase() {
}

bool CJsonDataBase::isNull() const {
    return m_object.isNull();
}

std::string CJsonDataBase::toJson() const {
    Json::StreamWriter* writer = m_writerBuilder.newStreamWriter();
    std::ostringstream stream;
    update();
    writer->write(m_object, &stream);
    delete writer;
    writer = nullptr;
    return stream.str();
}

void CJsonDataBase::update() const {
    CJsonDataBase* tempThis = const_cast<CJsonDataBase*>(this);
    tempThis->m_mutex.lock();
    for (auto it = m_objectMap.begin(); it != m_objectMap.end(); ++it) {
        tempThis->m_object[it->first] = it->second.toObject();
    }
    tempThis->m_mutex.unlock();
}

int32_t CJsonDataBase::fromJson(const std::string& json) {
    JSONCPP_STRING errs;
    int32_t error = JSON_ERROR_PARSE;
    Json::CharReader* reader = m_readerBuilder.newCharReader();
    if (reader->parse(json.data(), json.data() + json.size(), &m_object, &errs)) {
        error = JSON_ERROR_OK;
    }
    delete reader;
    reader = nullptr;

    if (!m_object.isObject()) {
        m_object = Json::Value();
        return JSON_ERROR_PARSE;
    }
    parse();
    return error;
}

const Json::Value& CJsonDataBase::toObject() const {
    update();
    return m_object;
}

void CJsonDataBase::fromObject(const Json::Value &object) {
    m_object = object;
    parse();
}

void CJsonDataBase::parse() {
    Json::Value::const_iterator it = m_object.begin();
    for (; it != m_object.end(); ++it) {
        if (it->isObject()) {
            CJsonDataBase child(*it);
            m_mutex.lock();
            auto objectIt = m_objectMap.find(it.name());
            if (objectIt != m_objectMap.end()) {
                objectIt->second = child;
            } else {
                m_objectMap.insert(std::make_pair(it.name(), child));
            }
            m_mutex.unlock();
        }
    }
}

void CJsonDataBase::clear() {
    m_object.clear();
}

bool CJsonDataBase::isExists(const char *name) const {
    return m_object.isMember(name) && !m_object[name].isNull();
}

void CJsonDataBase::set(const char *name, int32_t value) {
    m_object[name] = value;
}

void CJsonDataBase::get(const char *name, int32_t &value) const {
    value = 0;
    const Json::Value& temp = m_object.get(name, value);
    if (temp.isNumeric()) {
        value = temp.asInt();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<int32_t>& values) {
    Json::Value array;
    for (int32_t value : values) {
        array.append(value);
    }
    m_object[name] = array;
}

void CJsonDataBase::get(const char* name, std::vector<int32_t>& values) {
    values.clear();
    const Json::Value& array = m_object.get(name, Json::Value());
    if (array.isArray()) {
        size_t count = array.size();
        values.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const Json::Value& temp = array.get((Json::Value::ArrayIndex)i, 0);
            if (temp.isNumeric()) {
                values[i] = temp.asInt();
            }
        }
    }
}

void CJsonDataBase::set(const char* name, uint32_t value) {
    m_object[name] = value;
}

void CJsonDataBase::get(const char* name, uint32_t& value) const {
    value = 0;
    const Json::Value& temp = m_object.get(name, value);
    if (temp.isNumeric()) {
        value = temp.asUInt();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<uint32_t>& values) {
    Json::Value array;
    for (uint32_t value : values) {
        array.append(value);
    }
    m_object[name] = array;
}

void CJsonDataBase::get(const char* name, std::vector<uint32_t>& values) const {
    values.clear();
    const Json::Value& array = m_object.get(name, Json::Value());
    if (array.isArray()) {
        size_t count = array.size();
        values.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const Json::Value& temp = array.get((Json::Value::ArrayIndex)i, 0);
            if (temp.isNumeric()) {
                values[i] = temp.asUInt();
            }
        }
    }
}

void CJsonDataBase::set(const char* name, int64_t value) {
    m_object[name] = value;
}

void CJsonDataBase::get(const char* name, int64_t& value) const {
    value = 0;
    const Json::Value& temp = m_object.get(name, value);
    if (temp.isNumeric()) {
        value = temp.asInt64();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<int64_t>& values) {
    Json::Value array;
    for (int64_t value : values) {
        array.append(value);
    }
    m_object[name] = array;
}

void CJsonDataBase::get(const char* name, std::vector<int64_t>& values) {
    values.clear();
    const Json::Value& array = m_object.get(name, Json::Value());
    if (array.isArray()) {
        size_t count = array.size();
        values.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const Json::Value& temp = m_object.get((Json::Value::ArrayIndex)i, 0);
            if (temp.isNumeric()) {
                values[i] = temp.asInt64();
            }
        }
    }
}

void CJsonDataBase::set(const char *name, uint64_t value) {
    m_object[name] = value;
}

void CJsonDataBase::get(const char *name, uint64_t &value) const {
    value = 0;
    const Json::Value& temp = m_object.get(name, value);
    if (temp.isNumeric()) {
        value = temp.asUInt64();
    }
}

void CJsonDataBase::set(const char *name, const std::vector<uint64_t> &values) {
    Json::Value array;
    for (uint64_t value : values) {
        array.append(value);
    }
    m_object[name] = array;
}

void CJsonDataBase::get(const char *name, std::vector<uint64_t> &values) {
    values.clear();
    const Json::Value& array = m_object.get(name, Json::Value());
    if (array.isArray()) {
        size_t count = array.size();
        values.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const Json::Value& temp = m_object.get((Json::Value::ArrayIndex)i, 0);
            if (temp.isNumeric()) {
                values[i] = temp.asUInt64();
            }
        }
    }
}

void CJsonDataBase::set(const char* name, bool value) {
    m_object[name] = value;
}

void CJsonDataBase::get(const char* name, bool& value) const {
    value = false;
    const Json::Value& temp = m_object.get(name, value);
    if (temp.isBool()) {
        value = temp.asBool();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<bool>& values) {
    Json::Value array;
    for (bool value : values) {
        array.append(value);
    }
    m_object[name] = array;
}

void CJsonDataBase::get(const char* name, std::vector<bool>& values) const {
    values.clear();
    const Json::Value& array = m_object.get(name, Json::Value());
    if (array.isArray()) {
        size_t count = array.size();
        values.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const Json::Value& temp = array.get((Json::Value::ArrayIndex)i, false);
            if (temp.isBool()) {
                values[i] = temp.asBool();
            }
        }
    }
}

void CJsonDataBase::set(const char* name, float value) {
    m_object[name] = value;
}

void CJsonDataBase::get(const char* name, float& value) const {
    value = 0.0f;
    const Json::Value& temp = m_object.get(name, value);
    if (temp.isDouble()) {
        value = (float)temp.asDouble();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<float>& values) {
    Json::Value array;
    for (float value : values) {
        array.append(value);
    }
    m_object[name] = array;
}

void CJsonDataBase::get(const char* name, std::vector<float>& values) const {
    values.clear();
    const Json::Value& array = m_object.get(name, Json::Value());
    if (array.isArray()) {
        size_t count = array.size();
        values.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const Json::Value& temp = array.get((Json::Value::ArrayIndex)i, 0.0f);
            if (temp.isDouble()) {
                values[i] = (float)temp.asDouble();
            }
        }
    }
}

void CJsonDataBase::set(const char* name, double value) {
    m_object[name] = value;
}

void CJsonDataBase::get(const char* name, double& value) const {
    value = 0.0f;
    const Json::Value& temp = m_object.get(name, value);
    if (temp.isDouble()) {
        value = temp.asDouble();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<double>& values) {
    Json::Value array;
    for (double value : values) {
        array.append(value);
    }
    m_object[name] = array;
}

void CJsonDataBase::get(const char* name, std::vector<double>& values) const {
    values.clear();
    const Json::Value& array = m_object.get(name, Json::Value());
    if (array.isArray()) {
        size_t count = array.size();
        values.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const Json::Value& temp = array.get((Json::Value::ArrayIndex)i, 0.0f);
            if (temp.isDouble()) {
                values[i] = temp.asDouble();
            }
        }
    }
}

void CJsonDataBase::set(const char* name, const std::string& value) {
    m_object[name] = value;
}

void CJsonDataBase::get(const char* name, std::string& value) const {
    value = "";
    const Json::Value& temp = m_object.get(name, value);
    if (temp.isString()) {
        value = temp.asString();
    }
}

void CJsonDataBase::set(const char* name, const std::vector<std::string>& values) {
    Json::Value array;
    for (const std::string& value : values) {
        array.append(value);
    }
    m_object[name] = array;
}

void CJsonDataBase::get(const char* name, std::vector<std::string>& values) const {
    values.clear();
    const Json::Value& array = m_object.get(name, Json::Value());
    if (array.isArray()) {
        size_t count = array.size();
        values.resize(count);
        for (size_t i = 0; i < count; ++i) {
            const Json::Value& temp = array.get((Json::Value::ArrayIndex)i, std::string());
            if (temp.isString()) {
                values[i] = temp.asString();
            }
        }
    }
}

void CJsonDataBase::set(const char* name, const jc::CJsonDataBase& base) {
    m_mutex.lock();
    std::string strName(name);
    auto it = m_objectMap.find(strName);
    if (it != m_objectMap.end()) {
        it->second = base;
    } else {
        m_objectMap.insert(std::make_pair(strName, base));
    }
    m_mutex.unlock();
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
