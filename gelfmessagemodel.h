#ifndef GELFMESSAGEMODEL_H
#define GELFMESSAGEMODEL_H

#include <QAbstractTableModel>
#include <QJsonObject>
#include <QStringList>
#include <QList>

class GELFMessageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    GELFMessageModel(QObject *parent = 0);

private:
    QStringList column_names;
    QList<QJsonObject> messages;

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

public slots:
    void onMessage(QJsonObject message);
};

#endif // GELFMESSAGEMODEL_H
