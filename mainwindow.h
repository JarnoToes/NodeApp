#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "nodeData.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void refreshData();
    void getRequest(const QUrl&);

    QString response;

private slots:
    void on_but_OnOff_clicked();
    void on_but_UpdateList_clicked();
    void on_but_Status_clicked();
    void on_getProvisionable_clicked();
    void on_butProvision_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    nodeData *mappedNodes;

    friend class nodeData;

    double pbarVal = 0;
};
#endif // MAINWINDOW_H
