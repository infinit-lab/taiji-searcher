#include "mainwindow.h"
#include "protocol.h"
#include "protocol_socket.h"
#include "login_dialog.h"
#include "net_dialog.h"
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QTimer>

const int ipColumn = 0;
const int fingerprintColumn = 1;
const int versionColumn = 2;
const int buildTimeColumn = 3;
const int authorizationColumn = 4;
const int durationColumn = 5;

const int ipRole = Qt::UserRole + 1;
const int portRole = Qt::UserRole + 2;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_search(new QPushButton(this))
    , m_searchResult(new QTableWidget(this))
    , m_broadcast(nullptr)
    , m_loginDialog(new CLoginDialog(this))
    , m_netDialog(new CNetDialog(this))
    , m_timer(new QTimer(this)) {
    QLabel* label = new QLabel(this);
    label->setText(tr("Please double click to config"));

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_search);
    buttonLayout->addWidget(label);
    buttonLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(buttonLayout);
    layout->addWidget(m_searchResult);

    m_search->setText(tr("Search"));

    QStringList headers;
    headers << tr("IP") << tr("Fingerprint") << tr("Version") << tr("Build Time") << tr("Authorization") << tr("Duration");
    m_searchResult->setColumnCount(headers.size());
    m_searchResult->setHorizontalHeaderLabels(headers);
    m_searchResult->setMinimumWidth(1400);
    m_searchResult->setMinimumHeight(500);
    m_searchResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // m_searchResult->setSelectionMode(QAbstractItemView::NoSelection);
    // m_searchResult->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_searchResult->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_search, SIGNAL(released()), this, SLOT(onSearch()));
    connect(m_searchResult, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onCellDoubleClicked(int, int)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    onSearch();
}

MainWindow::~MainWindow() {
}

void MainWindow::onSearch() {
    m_timer->stop();
    m_timer->start(30 * 1000);
    m_search->setEnabled(false);
    m_searchResult->clearContents();
    m_searchResult->setRowCount(0);
    CRequest request;
    request.command(commandSearch);
    request.session(0);

    if (m_broadcast) {
        disconnect(m_broadcast, SIGNAL(frameReady()), this, SLOT(onFrameReady()));
        m_broadcast->deleteLater();
        m_broadcast = nullptr;
    }
    m_broadcast = new CProtocolSocket(this);
    connect(m_broadcast, SIGNAL(frameReady()), this, SLOT(onFrameReady()));

    const QByteArray& data = QByteArray::fromStdString(request.toJson());

    QList<QNetworkInterface> networkInterface = QNetworkInterface::allInterfaces();
    auto it = networkInterface.begin();
    for (; it != networkInterface.end(); ++it) {
        QList<QNetworkAddressEntry> addressList = it->addressEntries();
        auto aIt = addressList.begin();
        for (; aIt != addressList.end(); ++aIt) {
            m_broadcast->write(data, aIt->broadcast(), 5254);
        }
    }
    m_broadcast->write(data, QHostAddress::Broadcast, 5254);
}

int MainWindow::findRow(QString ip) {
    const QList<QTableWidgetItem*>& itemList = m_searchResult->findItems(ip, Qt::MatchExactly);
    if (itemList.empty()) {
        return -1;
    }
    return itemList.front()->row();
}

void MainWindow::onFrameReady() {
    const QList<TFrame>& frameList = m_broadcast->frameList();
    auto it = frameList.begin();
    for (; it != frameList.end(); ++it) {
        CSearchResponse response;
        response.fromJson(it->frame.toStdString());
        if (!response.result()) {
            continue;
        }
        quint32 address = it->address.toIPv4Address();
        quint8* pAddress = (quint8*)&address;
        QString ipString = QString("%0.%1.%2.%3").arg(pAddress[3]).arg(pAddress[2]).arg(pAddress[1]).arg(pAddress[0]);
        int row = findRow(ipString);
        if (row == -1) {
            row = m_searchResult->rowCount();
            m_searchResult->insertRow(row);
            int columnCount = m_searchResult->columnCount();
            for (int i = 0; i < columnCount; ++i) {
                QTableWidgetItem* item = new QTableWidgetItem;
                item->setTextAlignment(Qt::AlignCenter);
                m_searchResult->setItem(row, i, item);
            }
        }


        QTableWidgetItem* ip = m_searchResult->item(row, ipColumn);
        ip->setText(ipString);
        ip->setToolTip(ip->text());
        ip->setData(portRole, it->port);
        ip->setData(ipRole, address);

        QTableWidgetItem* fingerprint = m_searchResult->item(row, fingerprintColumn);
        fingerprint->setText(QString::fromStdString(response.data().license().fingerprint()));
        fingerprint->setToolTip(fingerprint->text());

        QTableWidgetItem* version = m_searchResult->item(row, versionColumn);
        version->setText(QString::fromStdString(response.data().version().version()) + \
            " Build(" + QString::fromStdString(response.data().version().commitId()) + ")");
        version->setToolTip(version->text());

        QTableWidgetItem* buildTime = m_searchResult->item(row, buildTimeColumn);
        buildTime->setText(QString::fromStdString(response.data().version().buildTime()));
        buildTime->setToolTip(buildTime->text());

        QTableWidgetItem* authorization = m_searchResult->item(row, authorizationColumn);
        QString status;
        switch (response.data().license().status()) {
        case Unauthorized:
            status = tr("Unauthorized");
            break;
        case Authorized:
            status = tr("Authorized");
            break;
        case Importing:
            status = tr("Importing");
            break;
        }
        authorization->setText(status);
        authorization->setToolTip(authorization->text());

        QTableWidgetItem* duration = m_searchResult->item(row, durationColumn);
        if (response.data().license().isForever()) {
            duration->setText(tr("Forever"));
        } else {
            duration->setText(QString::fromStdString(response.data().license().validDatetime()));
        }
        duration->setToolTip(duration->text());
    }
}

void MainWindow::onCellDoubleClicked(int row, int /*column*/) {
    m_loginDialog->setUsername("");
    m_loginDialog->setPassword("");
    int result = m_loginDialog->exec();
    if (result != QDialog::Accepted) {
        return;
    }
    QTableWidgetItem* ip = m_searchResult->item(row, ipColumn);
    if (!ip) {
        return;
    }
    QStringList sections = ip->text().split(".");
    if (sections.size() != 4) {
        return;
    }
    quint32 host = ip->data(ipRole).toUInt();
    quint16 port = ip->data(portRole).toUInt();
    m_netDialog->setHost(host, port, m_loginDialog->username(), m_loginDialog->password());
    if (m_netDialog->exec() == QDialog::Accepted) {
        onSearch();
    }
}

void MainWindow::onTimeout() {
    m_search->setEnabled(true);
}
