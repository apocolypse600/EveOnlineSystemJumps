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

    setWindowTitle("Eve Online Jump Calculator");

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

    //hide all of the columns, then re-show the ones we are interested in
    for(int i = 0; i<27;i++)
    {
        ui->tableView->hideColumn(i);
    }
    ui->tableView->showColumn(3);
    ui->tableView->showColumn(21);

    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    qDebug() << "Last Error: " << db.lastError().text();

    //Now we grab the file from the Eve website


    networkManager = new QNetworkAccessManager(this);
    //TODO uncomment this when it caches the data properly connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
    networkManager->get(QNetworkRequest(QUrl("https://api.eveonline.com/map/Jumps.xml.aspx")));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::downloadFinished(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "Download of API data failed";
    }
    else
    {
        QFile file("Jumps.xml");
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(reply->readAll());
            file.close();
        }

        qDebug() << "Download of API data succeeded";
    }
}
