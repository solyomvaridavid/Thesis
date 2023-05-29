#include "clndr.h"
#include "ui_clndr.h"
#include "add_dialog.h"
#include "account_dialog.h"
#include "allevent_dialog.h"
#include "delete_dialog.h"
#include "edit_dialog.h"
#include "help_dialog.h"
#include "mstf_dialog.h"
#include "settings_dialog.h"
#include "stylehelper.h"
#include "model/list_model.h"
#include "model/model.h"

#include <QTextCharFormat>
#include <QMessageBox>
#include <QFile>
#include <QDate>
#include <QTimer>

Clndr::Clndr(std::shared_ptr<Model> model,
             ListModel* listModel,
             UsersListModel* usersListModel,
             QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Clndr)
    , model(model)
    , listModel(listModel)
    , usersListModel(usersListModel)
{
    ui->setupUi(this);
    ui->listView->setModel(listModel);

    connect(ui->calendarWidget,
            &QCalendarWidget::selectionChanged,
            listModel,
            [listModel,this]() {
        QDate date = ui->calendarWidget->selectedDate();
        listModel->setCurrentDate(date);
    });
    initConnections();

    connect(model.get(),
            &Model::settingsApplied,
            this,
            [this](QString theme) {
        if (theme == "Dark") {
            this->setStyleSheet("");
            StyleHelper::text= "Dark";
            QFile styleSheetFile("C:/Szakdolgozat/clndr/Diffnes.qss");
            if (styleSheetFile.open(QFile::ReadOnly)) {
                StyleHelper::globalStyle = QLatin1String(styleSheetFile.readAll());
                styleSheetFile.close();
            }
        }
        else if (theme == "Light") {
            this->setStyleSheet("");
            StyleHelper::globalStyle = "";
            StyleHelper::text= "Light";
            ui->listView->setStyleSheet("QListView::item { "
                                                "background-color: #F1F1F1; "
                                                "padding: 1px; "
                                                "border: 0.5px solid black; "
                                            "} "
                                            "QListView::item:selected { "
                                                "background-color: #3164a5; "
                                                "border: 0.5px solid black; "
                                                "color: #fff;"
                                            "} "
                                            "QListView::item:selected:!active { "
                                                "background-color: #CCCCCC; "
                                                "border: 0.5px solid black; "
                                                "color: #000000; "
                                            "} "
                                            "QListView::item:selected:active { "
                                                "background-color: #3164a5; "
                                                "border: 0.5px solid black; "
                                                "color: #fff; "
                                            "} "
                                            " QListView::item:hover { "
                                                "background-color: #7DB2F5; "
                                                "border: 0.5px solid black; "
                                                "color: #000; "
                                            "} ");
        }
        else {
            this->setStyleSheet("");
            QString styleSheet = theme;
            StyleHelper::text= "Custom";
            StyleHelper::globalStyle = styleSheet;
        }
        this->setStyleSheet(StyleHelper::globalStyle);
    });
}

Clndr::~Clndr()
{
    delete ui;
    StyleHelper::globalStyle = "Light";
}

void Clndr::initConnections() const
{
    connect(ui->settingsButton,
            &QPushButton::clicked,
            this,
            &Clndr::onSettingsButtonClicked);

    connect(ui->addButton,
            &QPushButton::clicked,
            this,
            &Clndr::onAddButtonClicked);

    connect(ui->editButton,
            &QPushButton::clicked,
            this,
            &Clndr::onEditButtonClicked);

    connect(ui->deleteButton,
            &QPushButton::clicked,
            this,
            &Clndr::onDeleteButtonClicked);

    connect(ui->helpButton,
            &QPushButton::clicked,
            this,
            &Clndr::onHelpButtonClicked);

    connect(ui->accountButton,
            &QPushButton::clicked,
            this,
            &Clndr::onAccountButtonClicked);

    connect(ui->mstfButton,
            &QPushButton::clicked,
            this,
            &Clndr::onMstfButtonClicked);

    connect(ui->allEventButton,
            &QPushButton::clicked,
            this,
            &Clndr::onAllEventButtonClicked);

    connect(listModel,
            &ListModel::queryFinished,
            this,
            [this](Event e, QString inv) {
        Edit_dialog e_dialog(Model::getInstance(), usersListModel, e, inv);
        e_dialog.setStyleSheet(StyleHelper::globalStyle);
        e_dialog.exec();
        listModel->onRefreshEvent();
    });
}

int Clndr::eventToBe(int row)
{
    listModel->setEventToBe(listModel->getEventId(row));
    return listModel->getEventToBe();
}

void Clndr::deleteReady(Delete_dialog* d_dialog, int id)
{
    d_dialog->setEventId(id);
}

void Clndr::onSettingsButtonClicked()
{
    Settings_dialog s_dialog(Model::getInstance());
    s_dialog.setComboBoxText(StyleHelper::text);
    s_dialog.setStyleSheet(StyleHelper::globalStyle);
    s_dialog.exec();
}

void Clndr::onAddButtonClicked()
{
    QString date = ui->calendarWidget->selectedDate().toString("yyyy-MM-dd");
    Add_dialog a_dialog(Model::getInstance(), usersListModel, date);
    a_dialog.setStyleSheet(StyleHelper::globalStyle);
    a_dialog.exec();
    listModel->onRefreshEvent();
}


void Clndr::onEditButtonClicked()
{
    QModelIndex index = ui->listView->currentIndex();
    current_item = index.data(Qt::DisplayRole).toString();
    int row = index.row();

    if(!(current_item.isEmpty()))
    {
        int id = eventToBe(row);
        listModel->fetchEditableEvent(id);
    }
    else
    {
        QMessageBox::warning(this, "No event was chosen", "Please choose an event you want to edit!");
    }
}

void Clndr::onDeleteButtonClicked()
{
    QModelIndex index = ui->listView->currentIndex();
    current_item = index.data(Qt::DisplayRole).toString();
    int row = index.row();

    if(!(current_item.isEmpty()))
    {
        Delete_dialog d_dialog(Model::getInstance());
        int id = eventToBe(row);
        deleteReady(&d_dialog, id);
        d_dialog.setStyleSheet(StyleHelper::globalStyle);
        d_dialog.exec();
        listModel->onRefreshEvent();
    }
    else
    {
        QMessageBox::warning(this, "No event was chosen", "Please choose an event you want to delete!");
    }
}

void Clndr::onHelpButtonClicked()
{
    Help_dialog h_dialog;
    h_dialog.setStyleSheet(StyleHelper::globalStyle);
    h_dialog.exec();
}

void Clndr::onAccountButtonClicked()
{
    Account_dialog acc_dialog(username);
    acc_dialog.setStyleSheet(StyleHelper::globalStyle);

    connect(&acc_dialog,
            &Account_dialog::clicked,
            this,
            [this] {
        this->close();
        QDate today = QDate::currentDate();
        ui->calendarWidget->setSelectedDate(today);
        Model::getInstance()->setToken("");
        emit loginDialogRequested();
    });

    acc_dialog.exec();
}

void Clndr::onMstfButtonClicked()
{
    MSTF_Dialog mstf_dialog(Model::getInstance(), usersListModel);
    mstf_dialog.setStyleSheet(StyleHelper::globalStyle);
    mstf_dialog.exec();
}

void Clndr::onAllEventButtonClicked()
{
    AllEvent_dialog aed(Model::getInstance());
    aed.setStyleSheet(StyleHelper::globalStyle);
    aed.exec();
}
