#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlTableModel>
#include <QList>
#include <QtXml>
#include <QDate>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Eve Online Jump Calculator");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("systemMap.db");
    //db.setConnectOptions("QSQLITE_OPEN_READONLY");

    if (!db.open())
    {
        qDebug() << "error opening DB";
    }
    else
    {
        qDebug() <<"opened db";
    }

    tableModel = new QSqlTableModel(this,db);
    tableModel->setTable("Systems");
    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    tableModel->select();
    //qDebug() << "row count" << tableModel->rowCount();

    while(tableModel->canFetchMore())
    {
        //qDebug() << "fetched more";
        tableModel->fetchMore();
    }

   // qDebug() << "row count" << tableModel->rowCount();

    // Attach it to the view
    ui->tableView->setModel(tableModel);

   // qDebug() << "row count" << tableModel->rowCount();

    ui->tableView->setSortingEnabled(true);
    tableModel->sort(2,Qt::AscendingOrder);

    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    //Move the Jumps column to the end
    ui->tableView->horizontalHeader()->moveSection(0,3);
    //Hide the SystemID column
    ui->tableView->hideColumn(1);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if(dataIsCurrent())
    {
        ui->statusBar->showMessage("cached data is current");
    }
    else
    {
        //Reset the jump column
        db.exec("UPDATE Systems SET JUMPS=0");

        //Now we grab the file from the Eve website
        networkManager = new QNetworkAccessManager(this);
        connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
        //ui->statusBar->showMessage("Downloading API data ...",3000);
        //networkManager->get(QNetworkRequest(QUrl("https://api.eveonline.com/map/Jumps.xml.aspx")));
        parseXML();
    }

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

    //Now need to read the XML file we grabbed
    parseXML();
}

void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    //tableModel->setFilter("SOLARSYSTEMNAME LIKE '%" + ui->lineEdit_2->text() + "%'");
    updateFilters();
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    //qDebug() << "SECURITY>'" + QString::number(ui->doubleSpinBox->value()) + "'";
    //tableModel->setFilter("SECURITY>='" + QString::number(ui->doubleSpinBox->value()) + "'");
    updateFilters();
}

void MainWindow::on_doubleSpinBox_2_valueChanged(double arg1)
{
    //qDebug() << tableModel->filter() + "SECURITY<='" + QString::number(ui->doubleSpinBox_2->value()) + "'";
    //tableModel->setFilter(tableModel->filter() + " SECURITY<='" + QString::number(ui->doubleSpinBox_2->value()) + "'");
    updateFilters();
}

void MainWindow::updateFilters()
{
    QString filter = "SOLARSYSTEMNAME LIKE '%" + ui->lineEdit_2->text() + "%'" +" AND SECURITY<='" + QString::number(ui->doubleSpinBox_2->value()) + "'" + "AND SECURITY>='" + QString::number(ui->doubleSpinBox->value()) + "'";
    qDebug() <<"New Filter:" << filter;
    tableModel->setFilter(filter);
    //tableModel->select();
}

void MainWindow::parseXML()
{
    //ui->statusBar->showMessage("Parsing API data ...",3000);

    QDomDocument apiData;

    //load the file
    QFile file("Jumps.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "failed to open file";
    }
    else
    {
        if(!apiData.setContent(&file))
        {
            qDebug() << "failed to load document";
        }
        file.close();
    }

    //get the root element
    QDomElement root = apiData.firstChildElement();
    qDebug() <<"API version (expecting 2) : " << root.attributeNode("version").value();

    //ui->statusBar->showMessage(root.firstChildElement("cachedUntill").attribute());

    QDomElement dataRoot = root.firstChildElement("result").firstChildElement("rowset");
    QDomNodeList rows = dataRoot.elementsByTagName("row");
    int numberOfRows = rows.count();

    db.transaction();

    for (int i = 0; i < numberOfRows ; ++i)
    {
        QDomNode currentRow = rows.at(i);

        //convert to element
        if(currentRow.isElement())
        {
            QDomElement row = currentRow.toElement();
            QString id = row.attributeNode("solarSystemID").value();
            QString numberOfJumps = row.attributeNode("shipJumps").value();

            QString query = "UPDATE Systems SET JUMPS=" + numberOfJumps + " WHERE SOLARSYSTEMID='" + id + "'";

            qDebug() << query;
            db.exec(query);
            qDebug() << (double)i/numberOfRows*100 <<  "% done ";

        }
    }

    db.commit();

    file.close();

    qDebug() << "finished with xml data";

    //update the view
    tableModel->select();

    //qDebug() << QDate::currentDate() << "     " << QTime::currentTime();
}

bool MainWindow::dataIsCurrent()
{
    QDomDocument apiData;

    //load the file if something goes wrong, we need to redownload
    QFile file("Jumps.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    else
    {
        if(!apiData.setContent(&file))
        {
            qDebug() << "failed to load document";
            return false;
        }
        file.close();
    }

    //There has to be a better way to do this
    QString cachedDate = apiData.firstChildElement().toElement().elementsByTagName("cachedUntil").at(0).toElement().text();

    qDebug() << "comparing " << cachedDate << "to" << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");
    if(cachedDate <= QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss"))
    {
        qDebug() << "current cached data is not current";
        return false;
    }
    else
    {
        qDebug() << "current cached data is current";
        return true;
    }
}
