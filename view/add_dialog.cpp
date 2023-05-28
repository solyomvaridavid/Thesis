#include "add_dialog.h"
#include "ui_add_dialog.h"

#include <QMessageBox>

Add_dialog::Add_dialog(std::shared_ptr<Model> model,
                       UsersListModel* usersListModel,
                       const QString& date,
                       QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Add_dialog)
    , model(model)
    , date(date)
{
    ui->setupUi(this);
    ui->listView->setModel(usersListModel);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initConnections();
    usersListModel->populate();
}

Add_dialog::~Add_dialog()
{
    delete ui;
}

void Add_dialog::initConnections()
{
    connect(ui->addButton,
            &QPushButton::clicked,
            this,
            &Add_dialog::onAddButtonClicked);

    connect(ui->cancelButton,
            &QPushButton::clicked,
            this,
            &Add_dialog::close);

    connect(ui->allDayCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        if (checked) {
            ui->startTimeEdit->setDisabled(true);
            ui->endTimeEdit->setDisabled(true);
        } else {
            ui->startTimeEdit->setEnabled(true);
            ui->endTimeEdit->setEnabled(true);
        }
    });

    connect(model.get(),
            &Model::eventAdded,
            this,
            [this](std::optional<int> value) {
        if (value == std::nullopt) {
            this->close();
        } else {
            switch(value.value()) {
            case 400:
                QMessageBox::warning(this, "Failed to create event",
                                     "Not all required information arrived!");
                break;
            case 401:
                QMessageBox::warning(this, "Failed to create event",
                                     "Unauthorized user!");
                break;
            default:
                QMessageBox::warning(this, "Failed to create event",
                                     "Other server error!");
            }
        }
    });
}

void Add_dialog::onAddButtonClicked()
{
    if(!(ui->nameLineEdit->text().isEmpty()) && (ui->endTimeEdit->time() >= ui->startTimeEdit->time()))
    {
        QString name = ui->nameLineEdit->text();
        QString start_date = date;
        QString start_time;
        QString end_time;
        if (ui->allDayCheckBox->isChecked()) {
            start_time = "00:00";
            end_time = "23:59";
        }
        else {
            start_time = ui->startTimeEdit->text();
            end_time = ui->endTimeEdit ->text();
        }

        QString label = ui->labelComboBox->currentText();
        QString occurrence = ui->occComboBox->currentText();

        QModelIndexList selectedIndexes = ui->listView->selectionModel()->selectedIndexes();
        QJsonArray jsonArray;
        for (int i = 0; i < selectedIndexes.count(); ++i) {
            QModelIndex index = selectedIndexes.at(i);
            QVariant data = index.data(Qt::DisplayRole);
            QString s = data.toString().trimmed();
            jsonArray.append(s);
        }
        Model::getInstance()->addEvent(name, start_date, start_time, end_time, label, occurrence, jsonArray);
    }
    else
    {
        QMessageBox::warning(this, "Failed to create event",
                             "Please provide all the information above!");
    }
}
