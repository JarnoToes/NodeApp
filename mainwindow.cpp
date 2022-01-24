#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include <string>
#include <QChar>
#include <QAbstractItemView>
#include "nodeData.h"

//#define IP (QString("143.176.228.86:8090"))

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mappedNodes(new nodeData)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();
    QObject::connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply)
    {
        if(reply->error())
        {
            qDebug() << reply->errorString();
            return;
        }
        QString answer = reply->readAll();

        qDebug() << answer;
        response = answer;
    }
    );
}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager;
    delete mappedNodes;
}


void MainWindow::on_but_OnOff_clicked()
{
    QString selectedNode = ui->list_Nodes->currentText();
    bool check = ui->checkBox->isChecked();

    request.setUrl(QUrl("http://"+IP+"/onoff="+selectedNode+"&"+QString::number(check)));
    manager->get(request);
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

    for(auto& t : mappedNodes->nodeMap)
    {
        QString node = t.first;
        request.setUrl(QUrl("http://"+IP+"/get/"+node));
        manager->get(request);
    }
}

void MainWindow::refreshData()
{
    request.setUrl(QUrl("http://"+IP+"/get/all"));
    manager->get(request);

    std::stringstream ss(response.toStdString());
    std::string nodeAll;

    //Go through each line, see if publish is not available, then add the node ID to the dropdown menu
    while(std::getline(ss, nodeAll, '\n')){
        if((nodeAll.find("publish",0)) == std::string::npos){
            QString nodeID;
            int t = nodeAll.find("unicastAddress",0);

            nodeID = QString::fromStdString(nodeAll).mid(t+18, 4);
            mappedNodes->nodeMap.insert(std::pair<QString, bool>(nodeID, 0));
        }
    }
}

