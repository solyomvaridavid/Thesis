#include "mstf_dialog.h"
#include "ui_mstf_dialog.h"

#include <QMessageBox>
#include <QTime>

MSTF_Dialog::MSTF_Dialog(std::shared_ptr<Model> model,
                         UsersListModel* usersListModel,
                         QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MSTF_Dialog)
    , model(model)
{
    ui->setupUi(this);
    ui->listView->setModel(usersListModel);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    initConnections();
}

MSTF_Dialog::~MSTF_Dialog()
{
    delete ui;
}

void MSTF_Dialog::initConnections()
{
    connect(ui->findButton,
            &QPushButton::clicked,
            this,
            &MSTF_Dialog::onFindButtonClicked);

    connect(ui->cancelButton,
            &QPushButton::clicked,
            this,
            &MSTF_Dialog::close);
}

void MSTF_Dialog::onFindButtonClicked()
{
    QModelIndexList selectedIndexes = ui->listView->selectionModel()->selectedIndexes();

    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "Failed to find mutual spare time",
                             "Select at least one invitee!");
        return;
    }
    if (ui->durationTimeEdit->time().isNull()) {
        QMessageBox::warning(this, "Failed to find mutual spare time",
                             "Set the duration to at least 1 hour!");
        return;
    }

    if (ui->fromTimeEdit->time() >= ui->tilTimeEdit->time()) {
        QMessageBox::warning(this, "Failed to find mutual spare time",
                             "Invalid time interval!");
        return;
    }

    int hours = ui->durationTimeEdit->time().hour();
    int minutes = ui->durationTimeEdit->time().minute();
    int duration = hours * 3600 + minutes * 60;
    QString from_time = ui->fromTimeEdit->text();
    QString til_time = ui->tilTimeEdit->text();
    QString invitees = "";

    for (int i = 0; i < selectedIndexes.count(); ++i) {
        QModelIndex index = selectedIndexes.at(i);
        QVariant data = index.data(Qt::DisplayRole);
        invitees += "invitees[]=" + data.toString() + "&";
    }

    invitees = invitees.remove(invitees.size()-1, 1);

    QEventLoop spareTimeWaitLoop;

    connect(model.get(),
            &Model::findFinished,
            &spareTimeWaitLoop,
            &QEventLoop::quit);

    QString event = model->findTime(duration, from_time, til_time, invitees);

    if(model->getEvent().isEmpty()) {
        spareTimeWaitLoop.exec();
    }

    if (event == "notime") {
        QMessageBox::warning(this, "Failed to find mutual spare time",
                             "Couldn't find mutual spare time in the next 2 weeks!");
    } else {
        QMessageBox::information(this, "Mutual spare time found",
                                 event);
    }
}
