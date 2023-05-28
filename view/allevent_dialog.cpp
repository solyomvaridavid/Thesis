#include "allevent_dialog.h"
#include "ui_allevent_dialog.h"

AllEvent_dialog::AllEvent_dialog(std::shared_ptr<Model> model,
                                 QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::AllEvent_dialog)
    , model(model)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    auto listModel = new ListModel(model);
    listModel->setListenAllEvents(true);
    ui->listView->setModel(listModel);
    initConnections();
    listModel->populate();
}

AllEvent_dialog::~AllEvent_dialog()
{
    delete ui;
}

void AllEvent_dialog::initConnections()
{
    connect(ui->closeButton,
            &QPushButton::clicked,
            this,
            &AllEvent_dialog::close);
}


