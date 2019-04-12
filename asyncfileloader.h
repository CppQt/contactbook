#ifndef ASYNCFILELOADER_H
#define ASYNCFILELOADER_H

#include <QObject>

class QFile;
class QTextStream;

class AsyncFileLoader : public QObject
{
    Q_OBJECT
public:
    explicit AsyncFileLoader(const QString &fileName, QObject *parent = nullptr);
    virtual ~AsyncFileLoader() override;

    void setFileName(const QString &fileName);

    QString error() const;
    bool fileExists() const;

    static AsyncFileLoader *instance();
    static void destroyInstance();

public slots:
    void startLoading();
    void stopLoading();

    void loadNextLine();

signals:
    void loadingStarted();
    void loadingFinished();
    void lineInvalid();
    void lineLoaded(QString line);
    void errorOccured(QString reason);

private:
    QString m_fileName;
    QString m_error;
    QScopedPointer<QFile> m_file;
    QScopedPointer<QTextStream> m_stream;
    static AsyncFileLoader *m_instance;
};

#endif // ASYNCFILELOADER_H
