#pragma once

#include "model/model.h"
#include "model/userslist_model.h"

#include <QDialog>

namespace Ui {
class MSTF_Dialog;
}

class MSTF_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit MSTF_Dialog(std::shared_ptr<Model> model,
                         UsersListModel* usersListModel,
                         QWidget *parent = nullptr);
    ~MSTF_Dialog();

private slots:
    void onFindButtonClicked();

private:
    void initConnections();

private:
    Ui::MSTF_Dialog *ui;
    std::shared_ptr<Model> model;
};
