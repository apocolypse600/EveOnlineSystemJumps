#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlTableModel>
#include <QList>
#include <QtXml>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Eve Online Jump Calculator");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
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
        qDebug() <<"Last error" << db.lastError().text();
        QList<QString> tables = db.tables();
        qDebug() << "number of tables: " << tables.length();
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

    //hide all of the columns, then re-show the ones we are interested in
    /*for(int i = 0; i<4;++i)
    {
        ui->tableView->hideColumn(i);
    }*/


    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    //Move the Jumps column to the end
    ui->tableView->horizontalHeader()->moveSection(0,3);
    //ui->tableView->hideColumn(3);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    /*ui->tableView->showColumn(5);
    ui->tableView->showColumn(23);
    ui->tableView->showColumn(1);*/

    //Now we grab the file from the Eve website

    networkManager = new QNetworkAccessManager(this);
    //TODO uncomment these when it caches the data properly
    //connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
    //networkManager->get(QNetworkRequest(QUrl("https://api.eveonline.com/map/Jumps.xml.aspx")));

    //Need to force a wait for the file to be downloaded

    //Now need to read the XML file we grabbed

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

    QDomElement dataRoot = root.firstChildElement("result").firstChildElement("rowset");
    QDomNodeList rows = dataRoot.elementsByTagName("row");
    int numberOfRows = rows.count();

    //Reset the jump column

    //qDebug() << "setting jump column to 0";
    //db.exec("UPDATE Systems SET JUMPS=0");
    if(tableModel->submitAll())
    {
        qDebug() << "reset and saved to db";
    }
    else
    {
        qDebug() << db.lastError().text();
    }


    for (int i = 0; i < numberOfRows ; ++i)
    {
        QDomNode currentRow = rows.at(i);

        //convert to element
        if(currentRow.isElement())
        {
            QDomElement row = currentRow.toElement();
            QString id = row.attributeNode("solarSystemID").value();
            QString numberOfJumps = row.attributeNode("shipJumps").value();

            /*QSqlQuery dbquery(db);
            dbquery.exec("SELECT NUMBER from Systems where SOLARSYSTEMID='" + id + "'");
            while(dbquery.next())
            {
                while(tableModel->canFetchMore())
                {
                    //qDebug() << "fetched more";
                    tableModel->fetchMore();
                }

                //qDebug() << dbquery.value(0).toString();
                if(tableModel->setData(tableModel->index(dbquery.value(0).toInt(),1),numberOfJumps))
                {
                    //qDebug() << "Data set at row " << dbquery.value(0).toInt();
                }
                else
                {
                   // qDebug() << "row count" << tableModel->rowCount();
                   // qDebug() << "Data failed at row " << dbquery.value(0).toInt();
                }



            }*/

            QString query = "UPDATE Systems SET JUMPS=" + numberOfJumps + " WHERE SOLARSYSTEMID='" + id + "'";

            qDebug() << query;
            db.exec(query);
            qDebug() << (double)i/numberOfRows*100 <<  "% done ";




        }


    }

    //save changes to DB
    if(tableModel->submitAll())
    {
        qDebug() << "Saved new values to DB";
    }
    else
    {
        qDebug() << db.lastError().text();
    }

    QSqlDatabase::removeDatabase("QSQLITE");

    qDebug() << "finished with xml data";
    file.close();

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
