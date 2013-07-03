#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int fileLength = 0;
    int COLUMNS = 3;

    QFile sFile("mapSolarSystems.csv");

    if(sFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&sFile);

        //Now we can just handle the rest of the file, we only need the number of rows
        while (!stream.atEnd())
        {
            stream.readLine();
            ++fileLength;
        }

        stream.reset();
        stream.seek(0);

        ui->mainTableWidget->setRowCount(fileLength);
        ui->mainTableWidget->setColumnCount(3);

        /*for(int i = 0; i < stoneFileLength ; i++)
        {
            stone[i] = stream.readLine();
        }*/

        //Start on the second row
        stream.readLine();
        int currentRow=1;

        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            QStringList strings = line.split(",");

            QLabel *Label1 = new QLabel(strings[3]);
            QLabel *Label2 = new QLabel(strings[21]);

            ui->mainTableWidget->setCellWidget(currentRow,1,Label1);
            ui->mainTableWidget->setCellWidget(currentRow,2,Label2);

            ++currentRow;
        }


    }

    sFile.close();


}

MainWindow::~MainWindow()
{
    delete ui;
}
