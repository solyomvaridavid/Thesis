#include "help_dialog.h"
#include "ui_help_dialog.h"

Help_dialog::Help_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Help_dialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QPixmap calendarHelp("C:/Szakdolgozat/clndr/resources/calendarhelp.png");
    ui->calendarLabel->setPixmap(calendarHelp);

    QPixmap addHelp("C:/Szakdolgozat/clndr/resources/addhelp.png");
    ui->addLabel->setPixmap(addHelp);

    QPixmap editHelp("C:/Szakdolgozat/clndr/resources/edithelp.png");
    ui->editLabel->setPixmap(editHelp);

    QPixmap mutualHelp("C:/Szakdolgozat/clndr/resources/mutualhelp.png");
    ui->mutualLabel->setPixmap(mutualHelp);

    QPixmap accountHelp("C:/Szakdolgozat/clndr/resources/accounthelp.png");
    ui->accountLabel->setPixmap(accountHelp);

    QPixmap settingsHelp("C:/Szakdolgozat/clndr/resources/settingshelp.png");
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
