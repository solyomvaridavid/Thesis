#include "edit_dialog.h"
#include "ui_edit_dialog.h"

#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QDateTime>
#include <algorithm>
#include <QTimer>

Edit_dialog::Edit_dialog(std::shared_ptr<Model> model,
                         UsersListModel* usersListModel,
                         Event event,
                         QString inv,
                         QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Edit_dialog)
    , model(model)
    , event(event)
    , inv(inv)
    , date(event.start_date.toString("yyyy-MM-dd"))
{
    ui->setupUi(this);

    usersListModel->populate();
    ui->listView->setModel(usersListModel);
    QString name = event.name.trimmed();
    ui->nameLineEdit->setText(name);
    QString timeString = event.start_time;
    QTime time = QTime::fromString(timeString,"HH:mm");
    ui->startTimeEdit->setTime(time);
    QString tString = event.end_time;
    QTime t = QTime::fromString(tString,"HH:mm");
    ui->endTimeEdit->setTime(t);
    ui->labelComboBox->setCurrentText(event.label);

    QStringList usernames = inv.split(',');

    QTimer::singleShot(100, this, [this, usernames, usersListModel] {
        QItemSelectionModel* selectionModel = ui->listView->selectionModel();

        for (int row = 0; row < usersListModel->rowCount(); ++row) {
            QModelIndex index = usersListModel->index(row, 0);
            QString item = usersListModel->data(index).toString().trimmed();
            if (usernames.contains(item)) {
                selectionModel->select(index, QItemSelectionModel::Select);
            }
        }
    });

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initConnections();
}

Edit_dialog::~Edit_dialog()
{
    delete ui;
}

void Edit_dialog::initConnections()
{
    connect(ui->saveButton,
            &QPushButton::clicked,
            this,
            &Edit_dialog::onSaveButtonClicked);

    connect(ui->cancelButton,
            &QPushButton::clicked,
            this,
            &Edit_dialog::close);

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
            &Model::eventEdited,
            this,
            [this](std::optional<int> value) {
        if (value == std::nullopt) {
            this->close();
        } else {
            switch(value.value()) {
            case 404:
                QMessageBox::warning(this, "Failed to edit event",
                                     "Event not found!");
                break;
            case 401:
                QMessageBox::warning(this, "Failed to edit event",
                                     "Unauthorized user!");
                break;
            case 403:
                QMessageBox::warning(this, "Failed to edit event",
                                     "You are not the owner of this event!");
                break;
            default:
                QMessageBox::warning(this, "Failed to edit event",
                                     "Other server error!");
            }
        }
    });
}

void Edit_dialog::onSaveButtonClicked()
{
    if(!(ui->nameLineEdit->text().isEmpty()) &&
            (ui->endTimeEdit->time() >= ui->startTimeEdit->time()))
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

        QModelIndexList selectedIndexes = ui->listView->selectionModel()->selectedIndexes();
        QJsonArray jsonArray;
        for (int i = 0; i < selectedIndexes.count(); ++i) {
            QModelIndex index = selectedIndexes.at(i);
            QVariant data = index.data(Qt::DisplayRole);
            jsonArray.append(data.toString());
        }

        Model::getInstance()->editEvent(event.id, name, start_date, start_time,
                         end_time, label, jsonArray);
    }
    else
    {
        QMessageBox::warning(this, "Failed to edit event",
                             "Please provide all the information above!");
    }
}
