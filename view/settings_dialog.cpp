#include "settings_dialog.h"
#include "ui_settings_dialog.h"
#include "stylehelper.h"

#include <QMessageBox>

Settings_dialog::Settings_dialog(std::shared_ptr<Model> model,
                                 QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings_dialog)
    , model(model)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initConnections();
    initUi();
}

Settings_dialog::~Settings_dialog()
{
    delete ui;
}

void Settings_dialog::initConnections()
{
    connect(ui->saveButton,
            &QPushButton::clicked,
            this,
            &Settings_dialog::onSaveButtonClicked);

    connect(ui->cancelButton,
            &QPushButton::clicked,
            this,
            &Settings_dialog::onCancelButtonClicked);

    connect(ui->comboBox,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            &Settings_dialog::comboBoxCurrentIndexCustomSettings);

    connect(ui->bgLineEdit,
            &QLineEdit::textChanged,
            this,
            [this](const QString& newText){
        QColor color(newText);
        if (color.isValid()) {
            ui->bgLabelBox->setStyleSheet(QString("background-color: %1").arg(newText));
        }
    });

    connect(ui->tLineEdit,
            &QLineEdit::textChanged,
            this,
            [this](const QString& newText){
        QColor color(newText);
        if (color.isValid()) {
            ui->tLabelBox->setStyleSheet(QString("background-color: %1").arg(newText));
        }
    });

    connect(ui->cLineEdit,
            &QLineEdit::textChanged,
            this,
            [this](const QString& newText){
        QColor color(newText);
        if (color.isValid()) {
            ui->cLabelBox->setStyleSheet(QString("background-color: %1").arg(newText));
        }
    });

    connect(ui->tbLineEdit,
            &QLineEdit::textChanged,
            this,
            [this](const QString& newText){
        QColor color(newText);
        if (color.isValid()) {
            ui->tbLabelBox->setStyleSheet(QString("background-color: %1").arg(newText));
        }
    });

    connect(ui->buttonLineEdit,
            &QLineEdit::textChanged,
            this,
            [this](const QString& newText){
        QColor color(newText);
        if (color.isValid()) {
            ui->buttonLabelBox->setStyleSheet(QString("background-color: %1").arg(newText));
        }
    });

    connect(model.get(),
            &Model::settingsPut,
            this,
            [this](std::optional<int> value) {
        if (value != std::nullopt) {
            QMessageBox::warning(this, "Error",
                                 "Failed to save theme to database!");
        }
    });

}

void Settings_dialog::initUi()
{
    ui->comboBox->setCurrentIndex(current_index);

    ui->bgLabel->setEnabled(false);
    ui->bgLineEdit->setEnabled(false);
    ui->tLabel->setEnabled(false);
    ui->tLineEdit->setEnabled(false);
    ui->cLabel->setEnabled(false);
    ui->cLineEdit->setEnabled(false);
    ui->tbLabel->setEnabled(false);
    ui->tbLineEdit->setEnabled(false);
    ui->buttonLabel->setEnabled(false);
    ui->buttonLineEdit->setEnabled(false);
}
void Settings_dialog::comboBoxCurrentIndexCustomSettings()
{
    if(ui->comboBox->currentText() == light_str)
    {
        ui->bgLabel->setEnabled(false);
        ui->bgLineEdit->setEnabled(false);
        ui->tLabel->setEnabled(false);
        ui->tLineEdit->setEnabled(false);
        ui->cLabel->setEnabled(false);
        ui->cLineEdit->setEnabled(false);
        ui->tbLabel->setEnabled(false);
        ui->tbLineEdit->setEnabled(false);
        ui->buttonLabel->setEnabled(false);
        ui->buttonLineEdit->setEnabled(false);
    }
    else if (ui->comboBox->currentText() == dark_str)
    {
        ui->bgLabel->setEnabled(false);
        ui->bgLineEdit->setEnabled(false);
        ui->tLabel->setEnabled(false);
        ui->tLineEdit->setEnabled(false);
        ui->cLabel->setEnabled(false);
        ui->cLineEdit->setEnabled(false);
        ui->tbLabel->setEnabled(false);
        ui->tbLineEdit->setEnabled(false);
        ui->buttonLabel->setEnabled(false);
        ui->buttonLineEdit->setEnabled(false);
    }
    else if (ui->comboBox->currentText() == custom_str)
    {
        ui->bgLabel->setEnabled(true);
        ui->bgLineEdit->setEnabled(true);
        ui->tLabel->setEnabled(true);
        ui->tLineEdit->setEnabled(true);
        ui->cLabel->setEnabled(true);
        ui->cLineEdit->setEnabled(true);
        ui->tbLabel->setEnabled(true);
        ui->tbLineEdit->setEnabled(true);
        ui->buttonLabel->setEnabled(true);
        ui->buttonLineEdit->setEnabled(true);
    }

    current_index = ui->comboBox->currentIndex();
    current_text = ui->comboBox->currentText();
    emit changed(current_index);
}

void Settings_dialog::setComboBoxText(QString text)
{
    ui->comboBox->setCurrentText(text);
}

void Settings_dialog::onSaveButtonClicked()
{
    if (ui->comboBox->currentText() != "Custom")
    {
        model->putSettings(ui->comboBox->currentText());
        QMessageBox::information(this, "Application's theme changed",
                             "The changes will take effect after you quit the application and log back in.");
    }
    else {
        if(QColor(ui->bgLineEdit->text()).isValid() && QColor(ui->tLineEdit->text()).isValid() &&
           QColor(ui->cLineEdit->text()).isValid() && QColor(ui->tbLineEdit->text()).isValid() &&
           QColor(ui->buttonLineEdit->text()).isValid())
        {
            QString q_widget = ui->bgLineEdit->text();
            QString calendar_widget = ui->cLineEdit->text();
            QString list_view = ui->tLineEdit->text();
            QString push_button = ui->buttonLineEdit->text();
            QString line_edit = ui->tbLineEdit->text();
            QString styleSheet = "QCalendarWidget QWidget { background-color: " + calendar_widget + "; }"
                                 " QToolButton { background-color: " + calendar_widget + "; }"
                                 " QWidget { background-color: " + q_widget + "; }"
                                 " QListView { background-color: " + list_view + "; }"
                                 " QPushButton { background-color: " + push_button + "; }"
                                 " QComboBox { background-color: " + line_edit + "; }"
                                 " QLineEdit { background-color: " + line_edit + "; }"
                                 " QCalendarWidget QAbstractItemView { font-size: 12px; font: Bahnschrift SemiBold; }";

            model->putSettings(styleSheet);
            QMessageBox::information(this, "Application's theme changed",
                                     "The changes will take effect after you quit the application and log back in.");
        } else {
            QMessageBox::warning(this, "Invalid color(s)",
                                 "One or more color codes that you provided are invalid!");
        }
    }
    this->close();
}

void Settings_dialog::onCancelButtonClicked()
{
    this->close();
}
