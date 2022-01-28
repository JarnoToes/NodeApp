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
#include <QTextBrowser>
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

/**
 * Gets url to sent GET request to API.
 * @brief MainWindow::getRequest
 * @param url
 */
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

/**
 * When button getProvisionable is clicked. The downdown box is filled with UUIDs that can be provisioned
 * @brief MainWindow::on_getProvisionable_clicked
 */
void MainWindow::on_getProvisionable_clicked()
{
    ui->list_provision->clear();
    getRequest(QUrl("http://"+IP+"/provision/scan"));
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());

    obj = doc.object();

    QJsonValue temp = obj.value("UUIDs");
    QJsonArray UUIDs = temp.toArray();

    for(auto t : UUIDs)
    {
        QString chopchop = t.toString();
        chopchop.chop(1);
        ui->list_provision->addItem(chopchop);
    }
}

/**
 * Provision selected UUID from dropdown menu
 * @brief MainWindow::on_butProvision_clicked
 */
void MainWindow::on_butProvision_clicked()
{
    QString selectedUUID = ui->list_provision->currentText();
    QString url = QString("http://"+IP+"/provision/%1").arg(selectedUUID);


    getRequest(QUrl(url));
    ui->provUserAction->setText("Press the button on the node this many times: \n");

    ui->provUserAction->setText(response);
}

void MainWindow::on_but_getResult_clicked()
{
    getRequest(QUrl("http://"+IP+"/provision/getresult"));
    ui->provUserAction->setText("Provisioning succesful: "+response);
}

/**
 * Turn on or off slected node from dropdown menu, with checkbox
 * @brief MainWindow::on_but_OnOff_clicked
 */
void MainWindow::on_but_OnOff_clicked()
{
    QString selectedNode = ui->list_Nodes->currentText();
    bool check = ui->checkBox->isChecked();

    getRequest(QUrl("http://"+IP+"/onoff="+selectedNode+"&"+QString::number(check)));
}

/**
 * Update dropdown menu with possible nodes to address
 * @brief MainWindow::on_but_UpdateList_clicked
 */
void MainWindow::on_but_UpdateList_clicked()
{
    refreshData();

    for(auto& t : mappedNodes->nodeMap)
    {
        if((ui->list_Nodes->findText(t.first)) == -1 )
            ui->list_Nodes->addItem(t.first);
    }
}

/**
 * Get the status of all possible nodes in the network
 * @brief MainWindow::on_but_Status_clicked
 */
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

/**
 * Refresh the map that contains all the nodes in the network
 * @brief MainWindow::refreshData
 */
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

