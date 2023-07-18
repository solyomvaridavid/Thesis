#pragma once

#include <QDialog>

#include "model/model.h"
#include "model/userslist_model.h"

namespace Ui {
class Admin_dialog;
}

class Admin_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Admin_dialog(std::shared_ptr<Model> model,
                          UsersListModel* usersListModel,
                          QWidget *parent = nullptr);
    ~Admin_dialog();

signals:
    void logout();
    void usersListModelChanged();

private slots:
    void onDeleteButtonClicked();
    void onLogoutButtonClicked();

private:
    void initConnections();

private:
    Ui::Admin_dialog *ui;
    std::shared_ptr<Model> model;
    UsersListModel usersListModel;
    QString current_item;
};
