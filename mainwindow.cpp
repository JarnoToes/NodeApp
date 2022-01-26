#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include <string>
#include <QChar>
#include <QAbstractItemView>
#include <QListWidget>
#include <QThread>
#include <QSignalSpy>
#include <QProgressBar>
#include <QJsonArray>
#include "nodeData.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mappedNodes(new nodeData)
{
    ui->setupUi(this);

    on_but_UpdateList_clicked();
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager;
    delete mappedNodes;
}

void MainWindow::getFinished(QNetworkReply *reply)
{
    response = reply->readAll();
}


void MainWindow::getRequest(const QUrl& url)
{
    QEventLoop loop;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply;
    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkRequest request(url);
    reply = manager->get(request);
    loop.exec();
    response = reply->readAll();
    qDebug() << response;
}

void MainWindow::on_getProvisionable_clicked()
{
    ui->list_provision->clear();
    getRequest(QUrl("http://"+IP+"/provision/scan"));
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());

    obj = doc.object();

    QJsonValue temp = obj.value("UUIDs");
    QJsonArray UUIDs = temp.toArray();

    QJsonDocument udoc;
    doc.setArray(UUIDs);
    QString data(udoc.toJson());
    ui->list_provision->addItem(data);

    {

    }
}

void MainWindow::on_butProvision_clicked()
{
    QString selectedUUID = ui->list_provision->currentText();

    getRequest(QUrl("http://"+IP+"/provision/"+selectedUUID));


}



void MainWindow::on_but_OnOff_clicked()
{
    QString selectedNode = ui->list_Nodes->currentText();
    bool check = ui->checkBox->isChecked();

    getRequest(QUrl("http://"+IP+"/onoff="+selectedNode+"&"+QString::number(check)));
}


void MainWindow::on_but_UpdateList_clicked()
{
    refreshData();

    for(auto& t : mappedNodes->nodeMap)
    {
        if((ui->list_Nodes->findText(t.first)) == -1 )
            ui->list_Nodes->addItem(t.first);
    }
}


void MainWindow::on_but_Status_clicked()
{
    refreshData();
    ui->view_nodes->clear();
    ui->progressBar->setValue(0);
    //QApplication::processEvents();
    double addVal = ((100 / mappedNodes->nodeMap.size()) / 2);


    for(auto& t : mappedNodes->nodeMap)
    {
        QString node = t.first;
        getRequest(QUrl("http://"+IP+"/get/"+node));
        pbarVal += addVal;
        ui->progressBar->setValue(pbarVal);
         //QApplication::processEvents();
        getRequest(QUrl("http://"+IP+"/get/"+node));
        pbarVal += addVal;
        ui->progressBar->setValue(pbarVal);
         //QApplication::processEvents();

        std::stringstream ss(response.toStdString());
        std::string nodeStatus;
        while(std::getline(ss, nodeStatus, '\n'))
        {
            if(nodeStatus.find("target onoff", 0) != std::string::npos)
            {
                ui->view_nodes->addItem(node + " | " + QString::fromStdString(nodeStatus));
            }
        }

    }
    ui->progressBar->setValue(100);
}

void MainWindow::refreshData()
{
    getRequest(QUrl("http://"+IP+"/get/all"));

    std::stringstream ss(response.toStdString());
    std::string nodeAll;

    //Go through each line, see if publish is not available, then add the node ID to the dropdown menu

    while(std::getline(ss, nodeAll, '\n')){
        if((nodeAll.find("elementIndex",0)) != std::string::npos){
            if((nodeAll.find("publish",0)) == std::string::npos){
                QString nodeID;
                int t = nodeAll.find("unicastAddress",0);

                nodeID = QString::fromStdString(nodeAll).mid(t+18, 4);
                mappedNodes->nodeMap.insert(std::pair<QString, bool>(nodeID, 0));
            }
        }
    }
}

