#pragma once

#include "model.h"
#include <QAbstractListModel>

struct User
{
    int id;
    QString username;
    QString last_login;

    bool operator==(const User& other) const
    {
        return id == other.id &&
               username == other.username &&
               last_login == other.last_login;
    }
};

class UsersListModel : public QAbstractListModel
{
    Q_OBJECT

public:
   explicit UsersListModel(std::shared_ptr<Model> model,
                           QObject *parent = nullptr);
   int rowCount(const QModelIndex &parent = QModelIndex()) const override;
   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   void setCurrentUser(QString u){currentUser= u;}
   void setState(QString s){state = s;}
   void populate();
   void refresh();
   QList<User> convertJsonArrayToList(QJsonArray jsonArray);

private:
    std::shared_ptr<Model> model;
    QList<User> users;
    QString currentUser;
    QString state;
};
