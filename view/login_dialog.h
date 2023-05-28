#pragma once

#include "model/model.h"

#include <QDialog>

namespace Ui {
class Login_dialog;
}

class Login_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Login_dialog(std::shared_ptr<Model> model,
                          QWidget* parent = nullptr);
    ~Login_dialog();

signals:
    void login();

    void calendarRequested();
    void createAccountRequested();

    void listModelRequested();
    void usersListModelRequested(const QString& username,
                                 const QString& state);
    void adminDialogRequested();
    void currentUser(const QString& username);

private slots:
    void onLoginButtonClicked();
    void onNewUserButtonClicked();

private:
    void initConnections();

private:
    Ui::Login_dialog* ui;
    std::shared_ptr<Model> model;
};
