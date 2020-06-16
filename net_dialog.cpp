#include "net_dialog.h"
#include "protocol_socket.h"
#include "protocol.h"
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QtNetwork>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QTimer>
#include <QMessageBox>

CNetDialog::CNetDialog(QWidget* parent)
    : QDialog(parent)
    , m_netInterfaces(new QComboBox(this))
    , m_ip(new QLineEdit(this))
    , m_mask(new QLineEdit(this))
    , m_gateway(new QLineEdit(this))
    , m_mac(new QLineEdit(this))
    , m_confirm(new QPushButton(this))
    , m_cancel(new QPushButton(this))
    , m_socket(new CProtocolSocket(this))
    , m_host(new QHostAddress)
    , m_port(0)
    , m_username()
    , m_password()
    , m_session(0)
    , m_loop(new QEventLoop(this))
    , m_timer(new QTimer(this))
    , m_receive()
    , m_interfaceList() {

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Interface"), m_netInterfaces);
    formLayout->addRow(tr("IP"), m_ip);
    formLayout->addRow(tr("Mask"), m_mask);
    formLayout->addRow(tr("Gateway"), m_gateway);
    formLayout->addRow(tr("MAC"), m_mac);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_confirm);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancel);
    buttonLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);

    m_confirm->setText(tr("Confirm"));
    m_cancel->setText(tr("Cancel"));

    m_mac->setReadOnly(true);

    connect(m_cancel, SIGNAL(released()), this, SLOT(reject()));
    connect(m_confirm, SIGNAL(released()), this, SLOT(onConfirm()));
    connect(m_timer, SIGNAL(timeout()), m_loop, SLOT(quit()));
    connect(m_socket, SIGNAL(frameReady()), this, SLOT(onFrameReady()));
    connect(m_netInterfaces, SIGNAL(currentTextChanged(QString)), this, SLOT(onCurrentTextChanged(QString)));
}

CNetDialog::~CNetDialog() {

}

int CNetDialog::exec() {
    CAuthRequest request;
    request.command(commandNetList);
    request.session(++m_session);
    request.username(m_username.toStdString());
    request.password(m_password.toStdString());
    m_socket->write(QByteArray::fromStdString(request.toJson()), *m_host, m_port);

    m_timer->start(3 * 1000);
    m_loop->exec();
    m_timer->stop();

    CNetInterfaceResponse response;
    response.fromJson(m_receive.toStdString());
    if (response.command() != commandNetList || response.session() != m_session || !response.result()) {
        QMessageBox::warning(this, tr("Warning"), tr("Failed to get net list: ") + QString::fromStdString(response.error()));
        return QDialog::Rejected;
    }

    m_interfaceList = response.data();
    auto it = m_interfaceList.begin();
    for (; it != m_interfaceList.end(); ++it) {
        m_netInterfaces->addItem(QString::fromStdString(it->name()));
    }
    return QDialog::exec();
}

void CNetDialog::setHost(quint32 ipv4, quint16 port, const QString &username, const QString &password) {
    m_host->setAddress(ipv4);
    m_port = port;
    m_username = username;
    m_password = QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Md5).toHex();
    m_netInterfaces->clear();
    m_ip->setText("");
    m_mask->setText("");
    m_gateway->setText("");
    m_mac->setText("");
    m_interfaceList.clear();
}

void CNetDialog::onFrameReady() {
    m_receive.clear();
    const QList<TFrame>& frameList = m_socket->frameList();
    auto it = frameList.begin();
    for (; it != frameList.end(); ++it) {
        CResponse response;
        response.fromJson(it->frame.toStdString());
        if (response.session() == m_session) {
            m_receive = it->frame;
            m_loop->quit();
        }
    }
}

void CNetDialog::onCurrentTextChanged(const QString& text) {
    auto it = m_interfaceList.begin();
    for (; it != m_interfaceList.end(); ++it) {
        if (QString::fromStdString(it->name()) == text) {
            m_ip->setText(QString::fromStdString(it->ip()));
            m_mask->setText(QString::fromStdString(it->mask()));
            m_gateway->setText(QString::fromStdString(it->gateway()));
            m_mac->setText(QString::fromStdString(it->mac()));
            break;
        }
    }
}

void CNetDialog::onConfirm() {
    const QString& currentText = m_netInterfaces->currentText();
    auto it = m_interfaceList.begin();
    for (; it != m_interfaceList.end(); ++it) {
        if (QString::fromStdString(it->name()) == currentText) {
            if (m_ip->text() == QString::fromStdString(it->ip())
                    && m_mask->text() == QString::fromStdString(it->mask())
                    && m_gateway->text() == QString::fromStdString(it->gateway())) {
                reject();
                return;
            } else {
                CSetNetRequest request;
                request.command(commandSetNet);
                request.session(++m_session);
                request.username(m_username.toStdString());
                request.password(m_password.toStdString());
                request.data().name(currentText.toStdString());
                request.data().ip(m_ip->text().toStdString());
                request.data().mask(m_mask->text().toStdString());
                request.data().gateway(m_gateway->text().toStdString());
                qDebug() << __FUNCTION__ << __LINE__ << QString::fromStdString(request.toJson());
                m_socket->write(QByteArray::fromStdString(request.toJson()), *m_host, m_port);

                m_receive.clear();
                m_timer->start(3 * 1000);
                m_loop->exec();
                m_timer->stop();

                CResponse response;
                qDebug() << __FUNCTION__ << __LINE__ << m_receive;
                response.fromJson(m_receive.toStdString());
                if (response.command() != commandNetList) {
                    accept();
                    return;
                } else if (response.session() != m_session || !response.result()) {
                    QMessageBox::warning(this, tr("Warning"), tr("Failed to set net: ") + QString::fromStdString(response.error()));
                    return;
                }
                accept();
                return;
            }
        }
    }
    QMessageBox::warning(this, tr("Warning"), tr("Invalid net interface"));
}
