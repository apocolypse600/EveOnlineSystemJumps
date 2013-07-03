#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlTableModel>
#include <QList>
#include <QSqlError>
#include <QSqlDatabase>
#include <QtSql>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
     db.setHostName("localhost");
    db.setDatabaseName("systemMap.db");

    if (!db.open())
    {
        qDebug() << "error opening DB";
    }
    else
    {
        qDebug() <<"opened db";
        qDebug() <<"Last error" << db.lastError().text();
        QList<QString> tables = db.tables();
        qDebug() << "number of tables: " << tables.length();
    }

    QSqlTableModel *tableModel = new QSqlTableModel(this,db);
    tableModel->setTable("Systems");
    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    tableModel->select();

    // Attach it to the view, and make it read only
    ui->tableView->setModel(tableModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for(int i = 0; i<27;i++)
    {
        ui->tableView->hideColumn(i);
    }

    ui->tableView->showColumn(3);
    ui->tableView->showColumn(22);

    qDebug() << "Last Error: " << db.lastError().text();


}

MainWindow::~MainWindow()
{
    delete ui;
}
