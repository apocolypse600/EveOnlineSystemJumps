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
    void on_lineEdit_2_textChanged(const QString &arg1);
    void on_doubleSpinBox_valueChanged(double arg1);
    void on_doubleSpinBox_2_valueChanged(double arg1);
    void updateFilters();

};

#endif // MAINWINDOW_H
