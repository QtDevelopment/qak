#include "resource.h"

Resource::Resource(QQuickItem* parent):QQuickItem(parent)
{
    _networkManager = new QNetworkAccessManager(this);
    _dataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    _baseUrl = "http://1499.dk/bw";

    // Remember the bug where you connected multiple times in the load() function - only connect once :)
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReply(QNetworkReply*)));

    QDir dir(_dataPath);
    if (!dir.exists()) {
        if(dir.mkpath("."))
            qDebug() << "Created data directory" << _dataPath;
        else
            emit error("Failed creating data directory "+_dataPath);
    }
}

bool Resource::available(const QString &name)
{
    QFile file(resourceFile(name));
    return file.exists();
}

bool Resource::exists(const QString &path)
{
    QFile file(":"+path);
    return file.exists();
}

void Resource::load(const QString &name)
{
    QUrl url = resourceUrl(name);
    _networkManager->get(QNetworkRequest(url));
}

bool Resource::unload(const QString &name)
{
    if(available(name))
    {
        bool unloadSuccess = QResource::unregisterResource(resourceFile(name),"/"+name+"/");
        emit unloaded(name);
        return unloadSuccess;
    }
    return false;
}

void Resource::onNetworkReply(QNetworkReply* reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        int httpstatuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
        switch(httpstatuscode)
        {
            case 200:
            if (reply->isReadable())
            {
                QString name = resourceName(reply->request().url());

                //Assuming this is a human readable file replyString now contains the file
                //replyString = QString::fromUtf8(reply->readAll().data());

                QByteArray data = reply->readAll();
                qDebug() << "resource.cpp" << name;

                QString resName = resourceFile(name);
                QFile file(resName);
                file.open(QIODevice::WriteOnly);
                qint64 result = file.write(data);
                file.close();
                if(result > -1) {
                    QResource::registerResource(resName,"/"+name+"/");
                    emit loaded(name);
                } else
                    emit error(name);
            }
            break;
            case 404:
            case 333:
            default:
                break;
        }
    } else {
        emit networkError(reply->error(),reply->errorString());
    }

    reply->deleteLater();
}

QString Resource::resourceFile(const QString &name)
{
    return _dataPath+"/"+name+".rcc";
}

QUrl Resource::resourceUrl(const QString &name)
{
    return _baseUrl+"/"+name+".rcc";
}

QString Resource::resourceName(const QUrl &url)
{
    QFileInfo fi(url.fileName());
    return fi.baseName();
}

QString Resource::resourceName(const QString &str)
{
    QFileInfo fi(str);
    return fi.baseName();
}

QString Resource::appPath()
{
    return QDir::currentPath();
}
