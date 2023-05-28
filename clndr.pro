QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    view/account_dialog.cpp \
    view/add_dialog.cpp \
    view/admin_dialog.cpp \
    view/allevent_dialog.cpp \
    view/createaccount_dialog.cpp \
    view/delete_dialog.cpp \
    view/edit_dialog.cpp \
    view/help_dialog.cpp \
    model/list_model.cpp \
    view/login_dialog.cpp \
    main.cpp \
    view/clndr.cpp \
    model/model.cpp \
    view/mstf_dialog.cpp \
    view/settings_dialog.cpp \
    stylehelper.cpp \
    model/userslist_model.cpp

HEADERS += \
    view/account_dialog.h \
    view/add_dialog.h \
    view/admin_dialog.h \
    view/allevent_dialog.h \
    view/clndr.h \
    view/createaccount_dialog.h \
    view/delete_dialog.h \
    view/edit_dialog.h \
    view/help_dialog.h \
    model/list_model.h \
    view/login_dialog.h \
    model/model.h \
    view/mstf_dialog.h \
    view/settings_dialog.h \
    stylehelper.h \
    model/userslist_model.h

FORMS += \
    view/account_dialog.ui \
    view/add_dialog.ui \
    view/admin_dialog.ui \
    view/allevent_dialog.ui \
    view/clndr.ui \
    view/createaccount_dialog.ui \
    view/delete_dialog.ui \
    view/edit_dialog.ui \
    view/help_dialog.ui \
    view/login_dialog.ui \
    view/mstf_dialog.ui \
    view/settings_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/clndr_res.qrc \
    resources/help_res.qrc \
    resources/login_dialog_res.qrc \
    resources/settings_res.qrc

DISTFILES +=
