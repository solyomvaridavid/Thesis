#include "account_dialog.h"
#include "ui_account_dialog.h"

Account_dialog::Account_dialog(const QString& username,
                               QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Account_dialog)
    , username(username)
{
    ui->setupUi(this);
    ui->label->setText("Logged in as: " + username);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initConnections();
}

Account_dialog::~Account_dialog()
{
    delete ui;
}

void Account_dialog::initConnections()
{
    connect(ui->logoutButton,
            &QPushButton::clicked,
            this,
            &Account_dialog::onLogoutButtonClicked);
}

void Account_dialog::onLogoutButtonClicked()
{
    this->close();
    emit clicked();
}


