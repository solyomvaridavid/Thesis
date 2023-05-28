#pragma once

#include <QDialog>

namespace Ui {
class Help_dialog;
}

class Help_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Help_dialog(QWidget *parent = nullptr);
    ~Help_dialog();

private:
    void initConnections();
    Ui::Help_dialog *ui;
};
