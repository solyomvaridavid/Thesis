#include "login_dialog.h"
#include "ui_login_dialog.h"
#include "clndr.h"
#include "createaccount_dialog.h"

#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

Login_dialog::Login_dialog(std::shared_ptr<Model> model,
                           QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login_dialog)
    , model(model)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initConnections();
}

Login_dialog::~Login_dialog()
{
    delete ui;
}

void Login_dialog::initConnections()
{
    connect(ui->loginButton,
            &QPushButton::clicked,
            this,
            &Login_dialog::onLoginButtonClicked);

    connect(ui->newUserButton,
            &QPushButton::clicked,
            this,
            &Login_dialog::onNewUserButtonClicked);

    connect(model.get(),
            &Model::loginAttempt,
            this,
            [this](std::optional<int> value) {
        if (value == std::nullopt) {
            this->close();
        } else {
            switch(value.value()) {
            case 400:
                QMessageBox::warning(this, "Failed to login",
                                     "Incorrect username or password!");
                break;
            case 404:
                emit calendarCancelled();
                QMessageBox::warning(this, "Failed to login",
                                     "This user does not exist!");
                break;
            default:
                QMessageBox::warning(this, "Failed to login",
                                     "The server is not running!");
            }
        }
    });
}

void Login_dialog::onLoginButtonClicked()
{
    if(!(ui->usernameLineEdit->text().isEmpty() || ui->passwordLineEdit->text().isEmpty()))
    {
        QEventLoop tokenWaitLoop;
        QString typedUsername = ui->usernameLineEdit->text();
        QString typedPassword = ui->passwordLineEdit->text();

        connect(model.get(),
                &Model::tokenSetFinished,
                &tokenWaitLoop,
                &QEventLoop::quit);

        if (typedUsername != "admin")
        {
            model->logIntoCalendar(typedUsername, typedPassword);
            QString state = "short";

            if(model->getToken().isEmpty()) {
                tokenWaitLoop.exec();
            }
            emit calendarRequested();
            emit listModelRequested();
            emit usersListModelRequested(typedUsername, state);
            emit currentUser(typedUsername);
        }
        else {
            model->logIntoCalendar(typedUsername, typedPassword);
            QString state = "full";

            if(model->getToken().isEmpty()) {
                tokenWaitLoop.exec();
            }
            emit adminDialogRequested();
            emit usersListModelRequested(typedUsername, state);
        }

        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
    }
    else
    {
        QMessageBox::warning(this, "Failed to login",
                             "Please enter your username and password!");
        return;
    }
}

void Login_dialog::onNewUserButtonClicked()
{
    emit createAccountRequested();
    ui->usernameLineEdit->clear();
    ui->passwordLineEdit->clear();
    this->close();
}
