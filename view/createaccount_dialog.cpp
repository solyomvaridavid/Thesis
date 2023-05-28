#include "createaccount_dialog.h"
#include "ui_createaccount_dialog.h"

#include <QMessageBox>

CreateAccount_dialog::CreateAccount_dialog(std::shared_ptr<Model> model,
                                           QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateAccount_dialog)
    , model(model)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initConnections();
}

CreateAccount_dialog::~CreateAccount_dialog()
{
    delete ui;
}

void CreateAccount_dialog::initConnections()
{
    connect(ui->newUserButton,
            &QPushButton::clicked,
            this,
            &CreateAccount_dialog::onCreateAccountButtonClicked);

    connect(model.get(),
            &Model::userCreated,
            this,
            [this](std::optional<int> value) {
        if (value == std::nullopt) {
            QMessageBox::information(this, "Account created",
                                             "You can now login to the application.");
            emit loginDialogReq();
            ui->usernameLineEdit->clear();
            ui->passwordLineEdit->clear();
            ui->passwordAgainLineEdit->clear();
            this->hide();
        } else {
            switch(value.value()) {
            case 400:
                QMessageBox::warning(this, "Failed to create user",
                        "There is an already existing account under this username!");
                break;
            default:
                QMessageBox::warning(this, "Failed to create event",
                                     "Other server error!");
            }
        }
    });
}

void CreateAccount_dialog::onCreateAccountButtonClicked()
{
    if(ui->usernameLineEdit->text().isEmpty() || ui->passwordLineEdit->text().isEmpty()
         || ui->passwordAgainLineEdit->text().isEmpty()) {

        QMessageBox::warning(this, "Failed to create account",
                             "Please provide the necessary information!");
        return;
    }
    else if(ui->passwordLineEdit->text() != ui->passwordAgainLineEdit->text())
    {
        QMessageBox::warning(this, "Failed to create account",
                             "The two passwords don't match!");
        return;
    }
    else
    {
        QString typedUsername = ui->usernameLineEdit->text();
        QString typedPassword = ui->passwordLineEdit->text();

        Model::getInstance()->createUser(typedUsername, typedPassword);
    }
}

void CreateAccount_dialog::reject()
{
    this->close();
    emit loginDialogReq();
}
