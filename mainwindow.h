#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QJsonObject>

namespace Ui {
class MainWindow;
}

class GELFServer;
class GELFMessageModel;
class GELFMessageProxyModel;
class QJsonModel;

class QSortFilterProxyModel;
class QListWidgetItem;
class QSystemTrayIcon;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QMenu *system_tray_menu;
    QSystemTrayIcon *system_tray_icon;

    GELFServer *gelf_server;
    GELFMessageModel *gelf_message_model;
    QSortFilterProxyModel *gelf_message_proxy_model;
    QJsonModel *gelf_details_model;

private slots:
    void onMessage(QJsonObject);
    void onSectionCountChanged(int, int);
    void onCurrentRowChanged(QModelIndex, QModelIndex);
    void onColumnsWidgetItemChange(QListWidgetItem*);

};

#endif // MAINWINDOW_H
