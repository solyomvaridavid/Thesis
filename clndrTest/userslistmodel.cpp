#include "userslistmodel.h"

UsersListModel::UsersListModel(std::shared_ptr<Model> model,
                               QObject *parent)
    : QAbstractListModel(parent)
    , model(model)
{}

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
    Q_UNUSED(&parent);
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
