#include "userslist_model.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

UsersListModel::UsersListModel(std::shared_ptr<Model> model,
                               QObject *parent)
    : QAbstractListModel(parent)
    , model(model)
{
    initConnections();
}

void UsersListModel::initConnections()
{
    connect(model.get(),
            &Model::populatedUsersListModel,
            this,
            [this](std::optional<QJsonArray> value) {
        if (value == std::nullopt) {
            return;
        } else {
            beginResetModel();
            users.clear();
            users = convertJsonArrayToList(value.value());
            std::sort(users.begin(), users.end(), [](const User &u1, const User &u2){
                return u1.username < u2.username;
            });
            endResetModel();
        }
    });
}

QList<User> UsersListModel::convertJsonArrayToList(QJsonArray jsonArray)
{
    QList<User> ul;

    for (const auto& value : jsonArray) {
        if (value.isObject()) {
            const auto& obj = value.toObject();
            User user;
            user.id = obj["id"].toInt();
            user.username = obj["username"].toString();
            user.last_login = obj["last_login"].toString();

            if(user.username.length() < 15)
                for (int i = user.username.length(); i < 15; ++i) {
                    user.username = user.username + " ";
                }

            ul.append(user);
        }
    }
    return ul;
}

int UsersListModel::rowCount(const QModelIndex &parent) const
{
    return users.size();
}

QVariant UsersListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= users.size())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const auto& user = users.at(index.row());
        if (state == "full")
            return tr("%1 ---------- %2").arg(user.username,
                                     user.last_login);

        return tr("%1").arg(user.username);
    }
    return QVariant();
}

void UsersListModel::populate()
{
    model->usersListModelPopulate();
}

void UsersListModel::refresh()
{
    beginResetModel();
    populate();
    endResetModel();
}
