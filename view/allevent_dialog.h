#pragma once

#include "model/model.h"
#include "model/list_model.h"

#include <QDialog>

namespace Ui {
class AllEvent_dialog;
}

class AllEvent_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllEvent_dialog(std::shared_ptr<Model> model,
                             QWidget* parent = nullptr);
    ~AllEvent_dialog();

private:
    void initConnections();

private:
    Ui::AllEvent_dialog* ui;
    std::shared_ptr<Model> model;
};
