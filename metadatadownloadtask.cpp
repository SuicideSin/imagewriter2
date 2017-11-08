#include "metadatadownloadtask.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QNetworkReply>

MetadataDownloadTask::MetadataDownloadTask(QString serviceName, QUrl metadataUrl)
    : Task(tr("Getting information about %1 images").arg(serviceName)),
      serviceName(serviceName),
      metadataUrl(metadataUrl)
{
    auto cacheLocation = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    destinationFilePath = QStringLiteral("%1/org.opensuse.imgwriter/%2.xml").arg(cacheLocation).arg(serviceName);

    connect(&nam, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
}

MetadataDownloadTask::~MetadataDownloadTask()
{

}

void MetadataDownloadTask::start()
{
    setState(Task::Running);
    setMessage(tr("Downloading metadata"));

    // Only try to fetch metadata if connection available
    if(nam.networkAccessible() == QNetworkAccessManager::Accessible)
        nam.get(QNetworkRequest{metadataUrl});
    else
        replyFinished(nullptr);
}

void MetadataDownloadTask::stop()
{
    setState(Task::Failed);
}

void MetadataDownloadTask::replyFinished(QNetworkReply *reply)
{
    // Might be a redirection
    QUrl redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if(redirectionTarget.isValid())
    {
        nam.get(QNetworkRequest{redirectionTarget});
        reply->deleteLater();
        return;
    }

    setProgress(100);

    QFileInfo fileInfo(destinationFilePath);
    if(!reply || reply->error() != QNetworkReply::NoError)
    {
        // Failed, try the cache
        if(fileInfo.exists())
        {
            setState(Task::Done);
            setMessage(tr("Metadata download failed, using cache"));
            emit finished(destinationFilePath);
        }
        else
        {
            setState(Task::Failed);
            setMessage(tr("Metadata download failed, cache not available"));
        }
    }
    else
    {
        QFile destinationFile(destinationFilePath);
        QDir dir;
        if(dir.mkpath(fileInfo.path())
                && destinationFile.open(QFile::WriteOnly)
                && destinationFile.write(reply->readAll()))
        {
            setState(Task::Done);
            setMessage(tr("Metadata downloaded"));
            destinationFile.close();
            emit finished(destinationFilePath);
        }
        else
        {
            setState(Task::Failed);
            setMessage(tr("Failed to save metadata"));
        }
    }

    reply->deleteLater();
}