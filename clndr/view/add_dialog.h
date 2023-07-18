#pragma once

#include "model/model.h"
#include "model/userslist_model.h"

#include <QDialog>

namespace Ui {
class Add_dialog;
}

class Add_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Add_dialog(std::shared_ptr<Model> model,
                        UsersListModel* usersListModel,
                        const QString& date,
                        QWidget *parent = nullptr);
    ~Add_dialog();

private slots:
    void onAddButtonClicked();

private:
    void initConnections();

private:
    Ui::Add_dialog *ui;
    std::shared_ptr<Model> model;
    QString date;
};
