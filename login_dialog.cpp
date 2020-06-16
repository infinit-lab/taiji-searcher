#include "login_dialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDebug>

CLoginDialog::CLoginDialog(QWidget* parent)
    : QDialog(parent)
    , m_username(new QLineEdit(this))
    , m_password(new QLineEdit(this))
    , m_confirm(new QPushButton(this))
    , m_cancel(new QPushButton(this)) {

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Username:"), this), m_username);
    formLayout->addRow(new QLabel(tr("Password:"), this), m_password);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_confirm);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancel);
    buttonLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addSpacing(10);
    layout->addLayout(buttonLayout);

    m_confirm->setText(tr("Confirm"));
    m_cancel->setText(tr("Cancel"));

    connect(m_confirm, SIGNAL(released()), this, SLOT(onConfirm()));
    connect(m_cancel, SIGNAL(released()), this, SLOT(reject()));
}

CLoginDialog::~CLoginDialog() {

}

void CLoginDialog::onConfirm() {
    if (m_username->text() == "" || m_password->text() == "") {
        reject();
    }
    accept();
}

QString CLoginDialog::username() const {
    return m_username->text();
}

QString CLoginDialog::password() const {
    return m_password->text();
}

void CLoginDialog::setUsername(const QString& username) {
    m_username->setText(username);
}

void CLoginDialog::setPassword(const QString& password) {
    m_password->setText(password);
}
