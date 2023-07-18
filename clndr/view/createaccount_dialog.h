#pragma once

#include "model/model.h"

#include <QDialog>

namespace Ui {
class CreateAccount_dialog;
}

class CreateAccount_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateAccount_dialog(std::shared_ptr<Model> model,
                                  QWidget *parent = nullptr);
    ~CreateAccount_dialog();

signals:
    void loginDialogReq();

private slots:
    void onCreateAccountButtonClicked();
    void reject() override;

private:
    void initConnections();

private:
    Ui::CreateAccount_dialog *ui;
    std::shared_ptr<Model> model;
};

