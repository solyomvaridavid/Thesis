#pragma once

#include "model/model.h"
#include "model/list_model.h"

#include <QDialog>

namespace Ui {
class Delete_dialog;
}

class Delete_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Delete_dialog(std::shared_ptr<Model> model,
                           QWidget *parent = nullptr);
    ~Delete_dialog();
    void setEventId(const int id){ eventId = id; }

private slots:
    void onYesButtonClicked();

private:
    void initConnections();

private:
    Ui::Delete_dialog *ui;
    ListModel* listModel;
    std::shared_ptr<Model> model;
    int eventId = 0;
};
