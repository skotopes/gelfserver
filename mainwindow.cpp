#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gelfserver.h"
#include "gelfmessagemodel.h"

#include <QSortFilterProxyModel>
#include <QSettings>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gelf_server(new GELFServer(this)),
    gelf_message_model(new GELFMessageModel(this)),
    gelf_message_proxy_model(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);

    // Earlysignals connect
    connect(gelf_server, SIGNAL(message(QJsonObject)), gelf_message_model, SLOT(onMessage(QJsonObject)));
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
    ui->detailsWidget->horizontalHeader()->setStretchLastSection(true);
    ui->detailsWidget->horizontalHeader()->setVisible(false);
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
    int keys_count = gelf_message_proxy_model->columnCount();
    ui->detailsWidget->clearContents();
    ui->detailsWidget->setColumnCount(1);
    ui->detailsWidget->setRowCount(keys_count);
    for (int i=0; i < keys_count; i++) {
        QVariant header = gelf_message_proxy_model->headerData(i, Qt::Horizontal);
        QVariant data = gelf_message_proxy_model->data(gelf_message_proxy_model->index(current.row(), i));

        qDebug() << this << i << header << data;

        QTableWidgetItem *header_item = new QTableWidgetItem(header.toString());
        QTableWidgetItem *data_item = new QTableWidgetItem(data.toString());

        ui->detailsWidget->setVerticalHeaderItem(i, header_item);
        ui->detailsWidget->setItem(i, 0, data_item);
    }
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
