#include "help_dialog.h"
#include "ui_help_dialog.h"

Help_dialog::Help_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Help_dialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QPixmap calendarHelp(":/helpres/calendarhelp");
    ui->calendarLabel->setPixmap(calendarHelp);

    QPixmap addHelp(":/helpres/addhelp");
    ui->addLabel->setPixmap(addHelp);

    QPixmap editHelp(":/helpres/edithelp");
    ui->editLabel->setPixmap(editHelp);

    QPixmap mutualHelp(":/helpres/mutualhelp");
    ui->mutualLabel->setPixmap(mutualHelp);

    QPixmap accountHelp(":/helpres/accounthelp");
    ui->accountLabel->setPixmap(accountHelp);

    QPixmap settingsHelp(":/helpres/settingshelp");
    ui->settingsLabel->setPixmap(settingsHelp);

    initConnections();
}

void Help_dialog::initConnections()
{
    connect(ui->closeButton,
            &QPushButton::clicked,
            this,
            &Help_dialog::close);
}

Help_dialog::~Help_dialog()
{
    delete ui;
}
