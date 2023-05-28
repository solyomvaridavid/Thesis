#include "model/model.h"
#include "model/list_model.h"
#include "model/userslist_model.h"
#include "view/clndr.h"
#include "view/add_dialog.h"
#include "view/edit_dialog.h"
#include "view/admin_dialog.h"
#include "view/login_dialog.h"
#include "view/createaccount_dialog.h"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Login_dialog l(Model::getInstance());

    ListModel listModel(Model::getInstance());
    UsersListModel usersListModel(Model::getInstance());
    UsersListModel a_usersListModel(Model::getInstance());

    Admin_dialog admin(Model::getInstance(), &a_usersListModel);
    Clndr c(Model::getInstance(), &listModel, &usersListModel);
    CreateAccount_dialog ca(Model::getInstance());

    QObject::connect(&l,
                     &Login_dialog::calendarRequested,
                     &c,
                    [&c, &listModel] {
        c.show();
        QTimer::singleShot(100, &c, [&listModel]{listModel.populate(QDate::currentDate());});
        Model::getInstance()->applySettings();
    });

    QObject::connect(&l,
                     &Login_dialog::currentUser,
                     &c,
                    [&c](const QString& username) {
        c.setUsername(username);
    });

    QObject::connect(&l,
                     &Login_dialog::listModelRequested,
                     &listModel,
                    [&listModel] {
        listModel.populate(QDate::currentDate());
    });

    QObject::connect(&l,
                     &Login_dialog::createAccountRequested,
                     &ca,
                     [&ca] {
        ca.show();
    });

    QObject::connect(&l,
                     &Login_dialog::adminDialogRequested,
                     &admin,
                     [&admin, &a_usersListModel] {
        admin.show();
        QTimer::singleShot(100, &admin, [&a_usersListModel]{a_usersListModel.populate();});

    });

    QObject::connect(&admin,
                     &Admin_dialog::usersListModelChanged,
                     &a_usersListModel,
                     [&a_usersListModel] {
        a_usersListModel.populate();
    });

    QObject::connect(&l,
                     &Login_dialog::usersListModelRequested,
                     [&usersListModel, &a_usersListModel]
                     (const QString& username,
                                        const QString& state) {
        if (state == "short"){
            usersListModel.setState(state);
            usersListModel.setCurrentUser(username);
            usersListModel.populate();
        } else {
            a_usersListModel.setState(state);
            a_usersListModel.setCurrentUser(username);
            a_usersListModel.populate();
        }
    });


    QObject::connect(&c,
                     &Clndr::loginDialogRequested,
                     &l,
                     &Login_dialog::show);

    QObject::connect(&admin,
                     &Admin_dialog::logout,
                     &l,
                     &Login_dialog::show);

    QObject::connect(&ca,
                     &CreateAccount_dialog::loginDialogReq,
                     &l,
                     &Login_dialog::show);

    l.show();
    return a.exec();
}
