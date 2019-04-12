#include "asyncfileloader.h"

#include <QFile>
#include <QTextStream>
#include <QThread>

constexpr int MAX_LINE_LENGTH = 512;

AsyncFileLoader *AsyncFileLoader::m_instance = nullptr;

AsyncFileLoader::AsyncFileLoader(const QString &fileName, QObject *parent) :
    QObject(parent),
    m_fileName(fileName)
{
}

AsyncFileLoader::~AsyncFileLoader()
{
}

void AsyncFileLoader::setFileName(const QString &fileName)
{
    if (m_fileName != fileName) {
        m_fileName = fileName;
    }
}

QString AsyncFileLoader::error() const
{
    return m_error;
}

bool AsyncFileLoader::fileExists() const
{
    return QFile::exists(m_fileName);
}

AsyncFileLoader *AsyncFileLoader::instance()
{
    if (!m_instance) {
        m_instance = new AsyncFileLoader({});
        QThread* thread = new QThread();
        connect(thread, &QThread::finished, [] {
            AsyncFileLoader::m_instance->deleteLater();
            AsyncFileLoader::m_instance = nullptr;
        });
        m_instance->moveToThread(thread);
        thread->start();
    }
    return m_instance;
}

void AsyncFileLoader::destroyInstance()
{
    if (m_instance) {
        m_instance->thread()->quit();
        m_instance->thread()->wait();
    }
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

    QString line = m_stream->readLine(MAX_LINE_LENGTH);
    m_error.clear();
    emit lineLoaded(line);
}
