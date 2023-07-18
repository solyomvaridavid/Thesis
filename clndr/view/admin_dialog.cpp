#include "admin_dialog.h"
#include "ui_admin_dialog.h"

#include <QMessageBox>

Admin_dialog::Admin_dialog(std::shared_ptr<Model> model,
                           UsersListModel* usersListModel,
                           QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Admin_dialog)
    , model(model)
    , usersListModel(model, usersListModel)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    usersListModel->setState("full");
    usersListModel->setCurrentUser("admin");
    ui->listView->setModel(usersListModel);
    initConnections();
}

Admin_dialog::~Admin_dialog()
{
    delete ui;
}

void Admin_dialog::initConnections()
{
    connect(ui->deleteButton,
            &QPushButton::clicked,
            this,
            &Admin_dialog::onDeleteButtonClicked);

    connect(ui->logoutButton,
            &QPushButton::clicked,
            this,
            &Admin_dialog::onLogoutButtonClicked);

    connect(model.get(),
            &Model::userDeleted,
            this,
            [this](std::optional<int> value) {
        if (value != std::nullopt) {
            switch(value.value()) {
            case 404:
                QMessageBox::warning(this, "Failed to delete user",
                                     "User not found!");
                break;
            case 401:
                QMessageBox::warning(this, "Failed to delete user",
                                     "Unauthorized user!");
                break;
            default:
                QMessageBox::warning(this, "Failed to delete user",
                                     "Other server error!");
            }
        }
    });
}

void Admin_dialog::onDeleteButtonClicked()
{

    QModelIndex index = ui->listView->currentIndex();
    current_item = index.data(Qt::DisplayRole).toString();

    QRegularExpression regex("[^a-zA-Z]");
    current_item.remove(regex);

    Model::getInstance()->deleteUser(current_item);
    emit usersListModelChanged();
}

void Admin_dialog::onLogoutButtonClicked()
{
    this->close();
    emit logout();
}
