#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gelfserver.h"
#include "gelfmessagemodel.h"
#include "qjsonmodel.h"

#include <QSortFilterProxyModel>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QDebug>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    system_tray_menu(new QMenu(this)),
    system_tray_icon(new QSystemTrayIcon(this)),
    gelf_server(new GELFServer(this)),
    gelf_message_model(new GELFMessageModel(this)),
    gelf_message_proxy_model(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);
    // tray menu
    QAction *action;
    action = system_tray_menu->addAction(tr("Quit"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(close()));
    // tray icon
    system_tray_icon->setIcon(QIcon(":/GELFServer.svg"));
    system_tray_icon->setContextMenu(system_tray_menu);
    system_tray_icon->show();

    // Earlysignals connect
    connect(gelf_server, SIGNAL(message(QJsonObject)), this, SLOT(onMessage(QJsonObject)));
    connect(ui->messagesView->horizontalHeader(), SIGNAL(sectionCountChanged(int,int)), this, SLOT(onSectionCountChanged(int,int)));
    connect(ui->columnsWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onColumnsWidgetItemChange(QListWidgetItem*)));

    // configure model
    gelf_message_proxy_model->setSourceModel(gelf_message_model);

    // configure table view
    ui->messagesView->setModel(gelf_message_proxy_model);

    // load configuration
    QSettings settings;
    restoreState(settings.value("MainWindow/state").toByteArray());
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    ui->splitter->restoreState(settings.value("MainWindow/splitter/state").toByteArray());
    ui->splitter->restoreGeometry(settings.value("MainWindow/splitter/geometry").toByteArray());
    ui->messagesView->horizontalHeader()->restoreState(settings.value("MainWindow/messagesView/state").toByteArray());
    ui->messagesView->horizontalHeader()->restoreGeometry(settings.value("MainWindow/messagesView/geometry").toByteArray());

    // finalize UI setttings
    ui->messagesView->setSortingEnabled(true);
    ui->messagesView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->messagesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->messagesView->verticalHeader()->setVisible(false);
    ui->messagesView->horizontalHeader()->setStretchLastSection(true);
    ui->messagesView->horizontalHeader()->setSectionsMovable(true);
    ui->splitter->setChildrenCollapsible(false);

    // Late signals connect
    connect(ui->messagesView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(onCurrentRowChanged(QModelIndex,QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;

    // views and widget state
    settings.setValue("MainWindow/messagesView/geometry", ui->messagesView->horizontalHeader()->saveGeometry());
    settings.setValue("MainWindow/messagesView/state", ui->messagesView->horizontalHeader()->saveState());
    settings.setValue("MainWindow/splitter/geometry", ui->splitter->saveGeometry());
    settings.setValue("MainWindow/splitter/state", ui->splitter->saveState());
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/state", saveState());

    // column state
    int columns_count = ui->columnsWidget->count();
    for (int i=0; i<columns_count; i++) {
        QListWidgetItem* item = ui->columnsWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            settings.setValue("MainWindow/columns/" + item->text(), true);
        } else {
            settings.setValue("MainWindow/columns/" + item->text(), false);
        }
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::onMessage(QJsonObject message)
{
    // push data to model
    gelf_message_model->onMessage(message);
    // show notification
    QString title = message["host"].toString() + " ";
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information;
    int level = message["level"].toInt();
    switch (level) {
    case 0: // Emergency
        icon = QSystemTrayIcon::Critical;
        title += tr("Emergency");
        break;
    case 1: // Alert
        icon = QSystemTrayIcon::Critical;
        title += tr("Alert");
        break;
    case 2: // Critical
        icon = QSystemTrayIcon::Critical;
        title += tr("Critical");
        break;
    case 3: // Error
        icon = QSystemTrayIcon::Critical;
        title += tr("Error");
        break;
    case 4: // Warning
        icon = QSystemTrayIcon::Warning;
        title += tr("Warning");
        break;
    case 5: // Notice
        icon = QSystemTrayIcon::Warning;
        title += tr("Notice");
        break;
    case 6: // Informational
        icon = QSystemTrayIcon::Information;
        title += tr("Info");
        break;
    case 7: // Debug
        icon = QSystemTrayIcon::Information;
        title += tr("Debug");
        break;
    default:
        break;
    }
    system_tray_icon->showMessage(title, message["full_message"].toString(), icon, 5000);
}

void MainWindow::onSectionCountChanged(int,int b)
{
    QSettings settings;
    ui->columnsWidget->clear();
    for (int i=0; i<b; i++) {
        QString header_name = gelf_message_proxy_model->headerData(i, Qt::Horizontal).toString();

        QListWidgetItem* item = new QListWidgetItem(header_name, ui->columnsWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setData(Qt::UserRole, i);

        bool column_enabled = settings.value("MainWindow/columns/" + header_name, true).toBool();
        if (column_enabled) {
            item->setCheckState(Qt::Checked);
            ui->messagesView->horizontalHeader()->setSectionHidden(i, false);
        } else {
            item->setCheckState(Qt::Unchecked);
            ui->messagesView->horizontalHeader()->setSectionHidden(i, true);
        }
    }
}

void MainWindow::onCurrentRowChanged(QModelIndex current, QModelIndex /*previous*/)
{
    QModelIndex source_index = gelf_message_proxy_model->mapToSource(current);
    QJsonObject data = gelf_message_model->rowData(source_index.row());
    qDebug() << data;
    QJsonModel *model = new QJsonModel;
    qDebug() << model;
    model->setJsonObject(data);
    ui->detailsView->setModel(model);
}

void MainWindow::onColumnsWidgetItemChange(QListWidgetItem*item)
{
    int column_number = item->data(Qt::UserRole).toInt();
    if (item->checkState() == Qt::Checked) {
        ui->messagesView->horizontalHeader()->setSectionHidden(column_number, false);
    } else {
        ui->messagesView->horizontalHeader()->setSectionHidden(column_number, true);
    }
}
