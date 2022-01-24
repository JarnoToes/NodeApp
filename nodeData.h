#ifndef NODEDATA_H
#define NODEDATA_H

#include <QString>
#include <map>
#include "ui_mainwindow.h"

#define IP (QString("143.176.228.86:8090"))

class nodeData
{
public:
    std::map<QString, bool> nodeMap;

public:
    nodeData();
    ~nodeData();

    //void refreshData(QNetworkAccessManager&, QNetworkRequest, MainWindow&, nodeData&);

};

#endif // NODEDATA_H
