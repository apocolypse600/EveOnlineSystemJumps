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

    qDebug() << "Last Error: " << db.lastError().text();

    tableModel->removeColumns(0,27);
    tableModel->insertColumn(1);
    tableModel->insertColumn(22);
}

MainWindow::~MainWindow()
{
    delete ui;
}
