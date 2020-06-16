#ifndef NET_DIALOG_H
#define NET_DIALOG_H
#include <QDialog>
#include "protocol.h"
#include <vector>

class QComboBox;
class QLineEdit;
class CProtocolSocket;
class QHostAddress;
class QEventLoop;
class QTimer;

class CNetDialog : public QDialog {
    Q_OBJECT
public:
    explicit CNetDialog(QWidget* parent);
    ~CNetDialog();

    void setHost(quint32 ipv4, quint16 port, const QString& username, const QString& password);
    int exec() override;
public slots:
    void onFrameReady();
    void onCurrentTextChanged(const QString& text);
    void onConfirm();
private:
    QComboBox* m_netInterfaces;
    QLineEdit* m_ip;
    QLineEdit* m_mask;
    QLineEdit* m_gateway;
    QLineEdit* m_mac;
    QPushButton* m_confirm;
    QPushButton* m_cancel;
    CProtocolSocket* m_socket;
    QHostAddress* m_host;
    quint16 m_port;
    QString m_username;
    QString m_password;
    int32_t m_session;
    QEventLoop* m_loop;
    QTimer* m_timer;
    QByteArray m_receive;
    std::vector<CNetInterface> m_interfaceList;
};

#endif // NET_DIALOG_H
