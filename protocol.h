#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <QList>
#include <QByteArray>
#include <QMap>
#include "json_converter.h"

class CRequest : public jc::CJsonDataBase {
    JC_CLASS(CRequest)
    JC_DECLARE_MEMBER(std::string, command)
    JC_DECLARE_MEMBER(int32_t, session)
};

class CResponse : public CRequest {
    JC_CLASS(CResponse)
    JC_DECLARE_MEMBER(bool, result)
    JC_DECLARE_MEMBER(std::string, error)
};

class CVersion : public jc::CJsonDataBase {
    JC_CLASS(CVersion)
    JC_DECLARE_MEMBER(std::string, version)
    JC_DECLARE_MEMBER(std::string, commitId)
    JC_DECLARE_MEMBER(std::string, buildTime)
};

class CLicense : public jc::CJsonDataBase {
    JC_CLASS(CLicense)
    JC_DECLARE_MEMBER(std::string, fingerprint)
    JC_DECLARE_MEMBER(int32_t, status)
    JC_DECLARE_MEMBER(bool, isForever)
    JC_DECLARE_MEMBER(std::string, validDatetime)
    JC_DECLARE_MEMBER(int32_t, validDuration)
};

class CSearchData : public jc::CJsonDataBase {
    JC_CLASS(CSearchData)
    JC_DECLARE_CLASS(CVersion, version)
    JC_DECLARE_CLASS(CLicense, license)
};

class CAuthRequest : public CRequest {
    JC_CLASS(CAuthRequest)
    JC_DECLARE_MEMBER(std::string, username)
    JC_DECLARE_MEMBER(std::string, password)
};

class CNetInterface : public jc::CJsonDataBase {
    JC_CLASS(CNetInterface)
    JC_DECLARE_MEMBER(std::string, name)
    JC_DECLARE_MEMBER(std::string, ip)
    JC_DECLARE_MEMBER(std::string, mask)
    JC_DECLARE_MEMBER(std::string, gateway)
    JC_DECLARE_MEMBER(std::string, mac)
};

class CNetInterfaceResponse : public CResponse {
    JC_CLASS(CNetInterfaceResponse)
    JC_DECLARE_VECTOR(CNetInterface, data)
};

class CSetNetRequest : public CAuthRequest {
    JC_CLASS(CSetNetRequest)
    JC_DECLARE_CLASS(CNetInterface, data)
};

enum ELicenseStatus {
    Unauthorized = 0,
    Authorized = 1,
    Importing = 2
};

class CSearchResponse : public CResponse {
    JC_CLASS(CSearchResponse)
    JC_DECLARE_CLASS(CSearchData, data)
};

const std::string commandSearch = "search";
const std::string commandNetList = "net_list";
const std::string commandSetNet = "set_net";

class CProtocolHelper {
    enum {
        ECountIndex = 0x02,
        EPackageIndex = 0x04,
        EFrameIndex = 0x06,
        EDataLengthIndex = 0x08,
        EDataIndex = 0x09,

        ESingleDataLengthIndex = 0x04,
        ESingleDataIndex = 0x05,
    };
private:
    CProtocolHelper() {}
    ~CProtocolHelper() {}
public:
    static QList<QByteArray> pack(const QByteArray& buffer, quint16 index);
    static QList<QByteArray> parse(QByteArray& buffer, QMap<quint16, QMap<quint16, QByteArray>>& frameMap);
};

#endif // PROTOCOL_H
