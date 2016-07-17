#include "gelfmessagemodel.h"

#include <QDateTime>
#include <QDebug>

GELFMessageModel::GELFMessageModel(QObject *parent):
    QAbstractTableModel(parent),
    column_names(),
    messages()
{
    // Set common column names
    column_names << "version" << "timestamp" << "host" << "level" << "short_message" << "full_message";
}

int GELFMessageModel::rowCount(const QModelIndex &/*parent*/) const
{
    return messages.count();
}

int GELFMessageModel::columnCount(const QModelIndex &/*parent*/) const
{
    return column_names.count();
}

QVariant GELFMessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal ) {
            return column_names[section];
        } else {
            return QString("%1").arg(section);
        }
    }
    return QVariant();
}

QVariant GELFMessageModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        QString column_name = column_names[index.column()];
        QJsonObject message = messages[index.row()];
        if (column_name == "timestamp") {
            QDateTime timestamp;
            timestamp.setTime_t(message[column_name].toDouble());
            return timestamp;
        } else {
            return message[column_name].toVariant();
        }
    }
    return QVariant();
}

void GELFMessageModel::onMessage(QJsonObject message)
{

    QStringList keys = message.keys();
    QStringListIterator i_keys(keys);
    while (i_keys.hasNext()) {
        QString key = i_keys.next();
        if (!column_names.contains(key)) {
            auto column_index = column_names.count();
            beginInsertColumns(QModelIndex(), column_index, column_index);
            column_names << key;
            endInsertColumns();
        }
    }

    // Insert row
    auto row_index = messages.size();
    beginInsertRows(QModelIndex(), row_index, row_index);
    messages << message;
    endInsertRows();
}
