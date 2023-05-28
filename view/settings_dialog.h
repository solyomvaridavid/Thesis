#pragma once

#include "model/model.h"

#include <QDialog>

namespace Ui {
class Settings_dialog;
}

class Settings_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Settings_dialog(std::shared_ptr<Model> model,
                             QWidget *parent = nullptr);
    ~Settings_dialog();
    void setComboBoxText(QString text);

signals:
    void changed(int);

private slots:
    void comboBoxCurrentIndexCustomSettings();
    void onSaveButtonClicked();
    void onCancelButtonClicked();

private:
    void initConnections();
    void initUi();

private:
    Ui::Settings_dialog *ui;
    int current_index{0};
    QString current_text;
    static constexpr const char* light_str = "Light";
    static constexpr const char* dark_str = "Dark";
    static constexpr const char* custom_str = "Custom";
    std::shared_ptr<Model> model;

};
