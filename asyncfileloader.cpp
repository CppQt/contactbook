#include "asyncfileloader.h"

#include <QFile>
#include <QTextStream>

AsyncFileLoader::AsyncFileLoader(const QString &fileName, QObject *parent) :
    QObject(parent),
    m_fileName(fileName)
{
}

AsyncFileLoader::~AsyncFileLoader()
{
}

QString AsyncFileLoader::error() const
{
    return m_error;
}

bool AsyncFileLoader::fileExists() const
{
    return QFile::exists(m_fileName);
}

void AsyncFileLoader::startLoading()
{
    if (!QFile::exists(m_fileName)) {
        m_error = "File not exists";
        emit errorOccured(m_error);
        return;
    }

    m_file.reset(new QFile(m_fileName));
    if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_error = "Cannot read file";
        emit errorOccured(m_error);
        return;
    }

    m_stream.reset(new QTextStream(m_file.get()));

    emit loadingStarted();
}

void AsyncFileLoader::stopLoading()
{
    m_stream.reset();
    m_file.reset();
    m_error.clear();
    emit loadingFinished();
}

void AsyncFileLoader::loadNextLine()
{
    if (!m_stream) {
        m_error = "No open file";
        emit errorOccured(m_error);
        return;
    }

    if (m_stream->atEnd()) {
        stopLoading();
        return;
    }

    QString line = m_stream->readLine();
    m_error.clear();
    emit lineLoaded(line);
}
