#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H
#include <QDialog>

class QLineEdit;
class QPushButton;

class CLoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit CLoginDialog(QWidget* parent);
    ~CLoginDialog();

    QString username() const;
    QString password() const;
    void setUsername(const QString& username);
    void setPassword(const QString& password);
private:
    QLineEdit* m_username;
    QLineEdit* m_password;
    QPushButton* m_confirm;
    QPushButton* m_cancel;
public slots:
    void onConfirm();
};

#endif // LOGIN_DIALOG_H
