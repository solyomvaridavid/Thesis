#pragma once

#include <QDialog>

namespace Ui {
class Account_dialog;
}

class Account_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Account_dialog(const QString& username,
                            QWidget *parent = nullptr);
    ~Account_dialog();

signals:
    void clicked();

private slots:
    void onLogoutButtonClicked();

private:
    void initConnections();

private:
    Ui::Account_dialog *ui;
    QString username;
};
