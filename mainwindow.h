#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QPushButton;
class QTableWidget;
class CProtocolSocket;
class CLoginDialog;
class CNetDialog;
class QTimer;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onSearch();
    void onFrameReady();
    void onCellDoubleClicked(int row, int column);
    void onTimeout();
private:
    int findRow(QString ip);
private:
    QPushButton* m_search;
    QTableWidget* m_searchResult;
    CProtocolSocket* m_broadcast;
    CLoginDialog* m_loginDialog;
    CNetDialog* m_netDialog;
    QTimer* m_timer;
};
#endif // MAINWINDOW_H
