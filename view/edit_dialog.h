#pragma once

#include "model/userslist_model.h"
#include "model/list_model.h"
#include "model/model.h"

#include <QDialog>

namespace Ui {
class Edit_dialog;
}

class Edit_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Edit_dialog(std::shared_ptr<Model> model,
                        UsersListModel* usersListModel,
                        Event event,
                        QString inv,
                        QWidget *parent = nullptr);
    ~Edit_dialog();

private slots:
    void onSaveButtonClicked();

private:
    void initConnections();

private:
    Ui::Edit_dialog *ui;
    std::shared_ptr<Model> model;
    Event event;
    QString inv;
    QString date;
};
