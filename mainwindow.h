#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QtCore>
#include <QtGui>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QSqlError>
#include <QSqlDatabase>
#include <QtSql>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
     QNetworkAccessManager* networkManager;
     QSqlTableModel *tableModel;
     QSqlDatabase db;
     void parseXML();
     bool dataIsCurrent();

private slots:
    void downloadFinished(QNetworkReply *reply);
    void updateFilters();

};

#endif // MAINWINDOW_H
