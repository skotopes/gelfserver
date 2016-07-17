#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
class MainWindow;
}

class GELFServer;
class GELFMessageModel;
class GELFMessageProxyModel;
class QSortFilterProxyModel;
class QListWidgetItem;

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
    GELFServer *gelf_server;
    GELFMessageModel *gelf_message_model;
    QSortFilterProxyModel *gelf_message_proxy_model;

private slots:
    void onSectionCountChanged(int, int);
    void onCurrentRowChanged(QModelIndex, QModelIndex);
    void onColumnsWidgetItemChange(QListWidgetItem*);
};

#endif // MAINWINDOW_H
