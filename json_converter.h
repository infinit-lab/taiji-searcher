#ifndef __JSON_CONVERTER_H__
#define __JSON_CONVERTER_H__
#include <string>
#include <vector>
#include <mutex>
#include <map>
#ifdef JC_USING_JSONCPP
#include "json/json.h"
#else
#include <QJsonArray>
#include <QJsonObject>
#endif


#define JSON_ERROR_OK (0)
#define JSON_ERROR_PARSE (1)

#define JC_CLASS(CLASS) \
public: \
    CLASS() {} \
    CLASS(const jc::CJsonDataBase& base) { fromObject(base.toObject()); } \
    CLASS(const CLASS& base) { fromObject(base.toObject()); } \
    ~CLASS() {}

#define JC_DECLARE_MEMBER(TYPE, NAME) \
public: \
    void NAME(const TYPE& NAME) {\
        CJsonDataBase::set(#NAME, NAME); \
    } \
    TYPE NAME() const { \
        TYPE temp; \
        CJsonDataBase::get(#NAME, temp); \
        return temp; \
    } \
    bool NAME##Exists() const { \
        return isExists(#NAME); \
    } \

#define JC_DECLARE_CLASS(TYPE, NAME) \
public: \
    void NAME(const TYPE& NAME) { \
        CJsonDataBase::set(#NAME, NAME); \
    } \
    TYPE& NAME() const { \
        return CJsonDataBase::get<TYPE>(#NAME); \
    } \
    bool NAME##Exists() const { \
        return isExists(#NAME); \
    } \


#define JC_DECLARE_VECTOR(TYPE, NAME) \
public: \
    void NAME(const std::vector<TYPE>& NAME) { \
        CJsonDataBase::set(#NAME, NAME); \
    } \
    const std::vector<TYPE> NAME() const { \
        std::vector<TYPE> temp; \
        CJsonDataBase::get(#NAME, temp); \
        return temp; \
    } \
    bool NAME##Exists() const { \
        return isExists(#NAME); \
    } \


namespace jc {

class  CJsonDataBase{
public:
    CJsonDataBase();
    CJsonDataBase(const CJsonDataBase& other);
#ifdef JC_USING_JSONCPP
    CJsonDataBase(const Json::Value& object);
#else
    CJsonDataBase(const QJsonObject& object);
#endif
    virtual ~CJsonDataBase();

public:
    std::string toJson() const;
    int32_t fromJson(const std::string& json);
    bool isNull() const;
    bool isExists(const char* name) const;

#ifdef JC_USING_JSONCPP
    const Json::Value& toObject() const;
    void fromObject(const Json::Value& object);
#else
    const QJsonObject& toObject() const;
    void fromObject(const QJsonObject& object);
#endif

    void clear();
    const CJsonDataBase& operator=(const CJsonDataBase& other);
protected:
    void set(const char* name, int32_t value);
    void get(const char* name, int32_t& value) const;

    void set(const char* name, const std::vector<int32_t>& values);
    void get(const char* name, std::vector<int32_t>& values);

    void set(const char* name, uint32_t value);
    void get(const char* name, uint32_t& value) const;

    void set(const char* name, const std::vector<uint32_t>& values);
    void get(const char* name, std::vector<uint32_t>& values) const;

    void set(const char* name, int64_t value);
    void get(const char* name, int64_t& value) const;

    void set(const char* name, const std::vector<int64_t>& values);
    void get(const char* name, std::vector<int64_t>& values);

    void set(const char* name, uint64_t value);
    void get(const char* name, uint64_t& value) const;

    void set(const char* name, const std::vector<uint64_t>& values);
    void get(const char* name, std::vector<uint64_t>& values);

    void set(const char* name, bool value);
    void get(const char* name, bool& value) const;

    void set(const char* name, const std::vector<bool>& values);
    void get(const char* name, std::vector<bool>& values) const;

    void set(const char* name, float value);
    void get(const char* name, float& value) const;

    void set(const char* name, const std::vector<float>& values);
    void get(const char* name, std::vector<float>& values) const;

    void set(const char* name, double value);
    void get(const char* name, double& value) const;

    void set(const char* name, const std::vector<double>& values);
    void get(const char* name, std::vector<double>& values) const;

    void set(const char* name, const std::string& value);
    void get(const char* name, std::string& value) const;

    void set(const char* name, const std::vector<std::string>& values);
    void get(const char* name, std::vector<std::string>& values) const;

    void set(const char* name, const jc::CJsonDataBase& value);

    template<typename T>
    void set(const char* name, const std::vector<T>& values) {
#ifdef JC_USING_JSONCPP
        Json::Value array;
        for (auto it = values.begin(); it != values.end(); ++it) {
            array.append(it->toObject());
        }
        m_object[name] = array;
#else
        QJsonArray array;
        for (auto it = values.begin(); it != values.end(); ++it) {
            array.append(it->toObject());
        }
        m_object.insert(name, array);
#endif
    }

    template<typename T>
    void get(const char* name, std::vector<T>& values) const {
        values.clear();
#ifdef JC_USING_JSONCPP
        const Json::Value& array = m_object.get(name, Json::Value());
        if (array.isArray()) {
            size_t count = array.size();
            values.resize(count);
            for (int32_t i = 0; i < count; ++i) {
                const Json::Value& temp = array.get(i, Json::Value());
                if (temp.isObject()) {
                    jc::CJsonDataBase base;
                    base.fromObject(temp);
                    values[i] = base;
                }
            }
        }
#else
        if (m_object.contains(name)) {
            const QJsonValue& jsonValue = m_object.value(name);
            if (jsonValue.isArray()) {
                const QJsonArray& array = jsonValue.toArray();
                int32_t size = array.size();
                values.resize(size);
                for (int32_t i = 0; i < size; ++i) {
                    jc::CJsonDataBase base;
                    base.fromObject(array.at(i).toObject());
                    values[i] = base;
                }
            }
        }
#endif
    }

    template <typename T>
    T& get(const char* name) const {
        CJsonDataBase* tempThis = const_cast<CJsonDataBase*>(this);
        tempThis->m_mutex.lock();
        auto it = m_objectMap.find(std::string(name));
        if (it == m_objectMap.end()) {
            CJsonDataBase child;
            tempThis->m_objectMap.insert(std::make_pair(std::string(name), child));
            it = m_objectMap.find(std::string(name));
        }
        tempThis->m_mutex.unlock();
        CJsonDataBase* database = const_cast<CJsonDataBase*>(&it->second);
        return *(reinterpret_cast<T*>(database));
    }

    void parse();
    void update() const;

private:
#ifdef JC_USING_JSONCPP
    Json::Value m_object;
    static Json::CharReaderBuilder m_readerBuilder;
    static Json::StreamWriterBuilder m_writerBuilder;
#else
    QJsonObject m_object;
#endif
    std::mutex m_mutex;
    std::map<std::string, CJsonDataBase> m_objectMap;
};

}

#endif //__JSON_CONVERTER_H__
