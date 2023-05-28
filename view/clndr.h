#pragma once

#include "model/model.h"
#include "model/list_model.h"
#include "model/userslist_model.h"

#include <QMainWindow>

class Delete_dialog;

QT_BEGIN_NAMESPACE
namespace Ui { class Clndr; }
QT_END_NAMESPACE

class Clndr : public QMainWindow
{
    Q_OBJECT

public:
    explicit Clndr(std::shared_ptr<Model> model,
                    ListModel* listModel,
                   UsersListModel* usersListModel,
                   QWidget *parent = nullptr);
    ~Clndr();
    void setUsername(QString u){username = u;}

signals:
    void loginDialogRequested();

private slots:
    void onSettingsButtonClicked();
    void onAddButtonClicked();
    void onEditButtonClicked();
    void onDeleteButtonClicked();
    void onHelpButtonClicked();
    void onAccountButtonClicked();
    void onMstfButtonClicked();
    void onAllEventButtonClicked();

private:
    int eventToBe(int row);
    void deleteReady(Delete_dialog* d_dialog, int id);
    void initConnections() const;

private:
    Ui::Clndr* ui;
    std::shared_ptr<Model> model;
    ListModel* listModel;
    UsersListModel* usersListModel;
    QString current_item;
    QString username;
};
