#include "delete_dialog.h"
#include "ui_delete_dialog.h"

#include <QMessageBox>

Delete_dialog::Delete_dialog(std::shared_ptr<Model> model,
                             QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Delete_dialog)
    , model(model)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initConnections();
}

Delete_dialog::~Delete_dialog()
{
    delete ui;
}

void Delete_dialog::initConnections()
{
    connect(ui->yesButton,
            &QPushButton::clicked,
            this,
            &Delete_dialog::onYesButtonClicked);

    connect(ui->noButton,
            &QPushButton::clicked,
            this,
            &Delete_dialog::close);

    connect(model.get(),
            &Model::eventDeleted,
            this,
            [this] (std::optional<int> value) {
        if (value == std::nullopt) {
            this->close();
        } else {
            switch(value.value()) {
            case 404:
                QMessageBox::warning(this, "Failed to delete event",
                                     "Event not found!");
                break;
            case 401:
                QMessageBox::warning(this, "Failed to delete event",
                                     "Unauthorized user!");
                break;
            case 403:
                QMessageBox::warning(this, "Failed to delete event",
                                     "You are not the owner of this event!");
                break;
            default:
                QMessageBox::warning(this, "Failed to delete event",
                                     "Other server error!");
            }
        }
    });
}

void Delete_dialog::onYesButtonClicked()
{
    Model::getInstance()->deleteEvent(eventId);
}

