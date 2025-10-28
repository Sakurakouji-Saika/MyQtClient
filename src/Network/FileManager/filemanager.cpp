#include "filemanager.h"


#include <QDataStream>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

FileManager::FileManager(NetworkAdapter *msgClient,
                         const QString &host,
                         quint16 msgPort,
                         quint16 filePort,
                         QObject *parent)
    : QObject(parent),
    m_msgClient(msgClient),
    m_fileSocket(new QTcpSocket(this)),
    m_host(host),
    m_msgPort(msgPort),
    m_filePort(filePort),
    m_userId(-1),
    m_windowId(-2),
    m_sendFile(nullptr),
    m_totalSendBytes(0),
    m_bytesToWrite(0),
    m_bytesWritten(0),
    m_recvFile(nullptr),
    m_recvTotalBytes(0),
    m_recvBytesReceived(0),
    m_recvFileNameSize(0)
{
    connect(m_fileSocket, &QTcpSocket::connected, this, &FileManager::onFileSocketConnected);
    connect(m_fileSocket, &QTcpSocket::disconnected, this, &FileManager::onFileSocketDisconnected);
    connect(m_fileSocket, &QTcpSocket::readyRead, this, &FileManager::onFileSocketReadyRead);
    connect(m_fileSocket, &QTcpSocket::bytesWritten, this, &FileManager::onFileSocketBytesWritten);
}

FileManager::~FileManager()
{
    if (m_sendFile) {
        if (m_sendFile->isOpen()) m_sendFile->close();
        delete m_sendFile;
    }
    if (m_recvFile) {
        if (m_recvFile->isOpen()) m_recvFile->close();
        delete m_recvFile;
    }
    if (m_fileSocket->isOpen()) m_fileSocket->abort();
}

void FileManager::connectToFileServer()
{
    if (m_fileSocket->state() == QAbstractSocket::ConnectedState) return;
    if (m_host.isEmpty()) {
        emit errorOccured(QStringLiteral("FileManager: host is empty"));
        return;
    }
    m_fileSocket->connectToHost(m_host, m_filePort);
}

void FileManager::registerFileIdentity(int userId, int windowId)
{
    m_userId = userId;
    m_windowId = windowId;

    if (m_fileSocket->state() == QAbstractSocket::ConnectedState) {
        QDataStream out(m_fileSocket);
        out.setVersion(QDataStream::Qt_4_8);
        out << qint32(m_userId) << qint32(m_windowId);
        m_fileSocket->flush();
    }
}

bool FileManager::isFileServerConnected() const
{
    return m_fileSocket->state() == QAbstractSocket::ConnectedState;
}

// 上传普通文件并通知目标用户（简单阻塞实现，适合小文件）
void FileManager::uploadFile(const QString &localFilePath, const QString &remoteFileName, int targetUserId)
{
    if (!QFile::exists(localFilePath)) {
        emit errorOccured(QStringLiteral("uploadFile: local file not exist"));
        return;
    }

    if (m_fileSocket->state() != QAbstractSocket::ConnectedState) {
        connectToFileServer();
        if (!m_fileSocket->waitForConnected(5000)) {
            emit errorOccured(QStringLiteral("uploadFile: file server connect timeout"));
            return;
        }
        // register identity after connect
        if (m_userId >= 0) {
            QDataStream out(m_fileSocket);
            out.setVersion(QDataStream::Qt_4_8);
            out << qint32(m_userId) << qint32(m_windowId);
            m_fileSocket->flush();
        }
    }

    QFile file(localFilePath);
    if (!file.open(QFile::ReadOnly)) {
        emit errorOccured(QStringLiteral("uploadFile: open file failed"));
        return;
    }

    QString name = remoteFileName.isEmpty() ? QFileInfo(localFilePath).fileName() : remoteFileName;
    qint64 fileSize = file.size();

    // construct header
    QByteArray header;
    QDataStream sendOut(&header, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);
    sendOut << qint64(0) << qint64(0) << name;

    qint64 totalBytes = fileSize + header.size();
    sendOut.device()->seek(0);
    sendOut << qint64(totalBytes) << qint64(header.size() - sizeof(qint64)*2);

    // send header
    m_fileSocket->write(header);
    m_fileSocket->flush();

    // send file in chunks
    const qint64 chunkSize = 50 * 1024;
    qint64 bytesLeft = fileSize;
    while (bytesLeft > 0) {
        QByteArray chunk = file.read(qMin(bytesLeft, chunkSize));
        if (chunk.isEmpty()) break;
        m_fileSocket->write(chunk);
        if (!m_fileSocket->waitForBytesWritten(5000)) {
            emit errorOccured(QStringLiteral("uploadFile: send timeout"));
            file.close();
            return;
        }
        bytesLeft -= chunk.size();
    }

    file.close();

    // notify target user via message server using SendFile control (or SendFile message type)
    if (m_msgClient) {
        QJsonObject data;
        data.insert("from", m_userId);
        data.insert("to", targetUserId);
        data.insert("msg", name);
        m_msgClient->sendMessage(SendFile, data);
    }
}

void FileManager::requestDownloadFile(const QString &remoteFileName)
{
    if (!m_msgClient) {
        emit errorOccured(QStringLiteral("requestDownloadFile: message client not set"));
        return;
    }
    QJsonObject data;
    data.insert("from", m_userId);
    data.insert("id", m_windowId);
    data.insert("msg", remoteFileName);
    m_msgClient->sendMessage(GetFile, data);
}

void FileManager::uploadAvatar(const QString &localFilePath, const QString &remoteFileName, const QJsonArray &friends)
{
    QString name = remoteFileName.isEmpty() ? QFileInfo(localFilePath).fileName() : remoteFileName;
    // 上传到 file server
    uploadFile(localFilePath, name, -1); // -1 表示不指定目标（服务器保存）

    // 通知消息服务器更新头像记录
    if (m_msgClient) {
        QJsonObject data;
        data.insert("id", m_userId);
        data.insert("head", name);
        data.insert("friends", friends);
        m_msgClient->sendMessage(UpdateHeadPic, data);
    }
}

void FileManager::downloadAvatar(const QString &remoteFileName)
{
    // Ensure file socket is connected and identity registered
    if (m_fileSocket->state() != QAbstractSocket::ConnectedState) {
        connectToFileServer();
        if (!m_fileSocket->waitForConnected(5000)) {
            emit errorOccured(QStringLiteral("downloadAvatar: file server connect timeout"));
            return;
        }
        if (m_userId >= 0) {
            QDataStream out(m_fileSocket);
            out.setVersion(QDataStream::Qt_4_8);
            out << qint32(m_userId) << qint32(m_windowId);
            m_fileSocket->flush();
        }
    }

    requestDownloadFile(remoteFileName);
}

// slots
void FileManager::onFileSocketConnected()
{
    qDebug() << "FileManager: connected to file server";
    if (m_userId >= 0) {
        QDataStream out(m_fileSocket);
        out.setVersion(QDataStream::Qt_4_8);
        out << qint32(m_userId) << qint32(m_windowId);
        m_fileSocket->flush();
    }
}

void FileManager::onFileSocketDisconnected()
{
    qDebug() << "FileManager: file socket disconnected";
    if (m_recvFile) {
        if (m_recvFile->isOpen()) m_recvFile->close();
        delete m_recvFile; m_recvFile = nullptr;
    }
    m_recvBytesReceived = 0;
    m_recvTotalBytes = 0;
    m_recvFileNameSize = 0;
}

void FileManager::onFileSocketBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes)
    // Could be used to report progress for large sends
}

void FileManager::onFileSocketReadyRead()
{
    // follow protocol: qint64 totalBytes, qint64 fileNameSize, QString fileName, file bytes...
    QDataStream in(m_fileSocket);
    in.setVersion(QDataStream::Qt_4_8);

    // read header
    if (m_recvBytesReceived <= (qint64)(sizeof(qint64) * 2)) {
        qint64 headerLen = sizeof(qint64) * 2;
        if ((m_fileSocket->bytesAvailable() >= headerLen) && (m_recvFileNameSize == 0)) {
            in >> m_recvTotalBytes >> m_recvFileNameSize;
            if (m_recvTotalBytes != 0) m_recvBytesReceived += headerLen;
        }

        if ((m_fileSocket->bytesAvailable() >= m_recvFileNameSize) && (m_recvFileNameSize != 0) && (m_recvTotalBytes != 0)) {
            in >> m_recvFileName;
            m_recvBytesReceived += m_recvFileNameSize;

            // ensure head dir exists
            QDir dir;
            QString headDir = AppConfig::instance().imagesDirectory();
            if (headDir.isEmpty()) headDir = QDir::homePath() + "/UserHeads/";
            dir.mkpath(headDir);

            QString fullPath = headDir + m_recvFileName;
            m_recvFile = new QFile(fullPath);
            if (!m_recvFile->open(QFile::WriteOnly | QFile::Truncate)) {
                emit errorOccured(QStringLiteral("onReadyRead: open recv file failed"));
                delete m_recvFile; m_recvFile = nullptr;
                return;
            }
            qDebug() << "Begin to recv file:" << fullPath;
        }
    }

    // read body
    if (m_recvBytesReceived < m_recvTotalBytes) {
        QByteArray inBlock = m_fileSocket->readAll();
        m_recvBytesReceived += inBlock.size();
        if (m_recvFile && m_recvFile->isOpen()) {
            m_recvFile->write(inBlock);
        }
    }

    if ((m_recvBytesReceived >= m_recvTotalBytes) && (m_recvTotalBytes != 0)) {
        if (m_recvFile && m_recvFile->isOpen()) {
            QString savedPath = m_recvFile->fileName();
            m_recvFile->close();
            delete m_recvFile; m_recvFile = nullptr;

            m_recvBytesReceived = 0;
            m_recvTotalBytes = 0;
            m_recvFileNameSize = 0;
            m_recvFileName.clear();

            emit fileReceived(savedPath);
        }
    }
}

void FileManager::sendFileHeader(const QString &fileName, qint64 fileSize)
{
    QByteArray header;
    QDataStream sendOut(&header, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);
    sendOut << qint64(0) << qint64(0) << fileName;
    qint64 totalBytes = fileSize + header.size();
    sendOut.device()->seek(0);
    sendOut << qint64(totalBytes) << qint64(header.size() - sizeof(qint64)*2);
    m_fileSocket->write(header);
}

void FileManager::startSendFile()
{
    if (!m_sendFile) return;
    const qint64 chunkSize = 50 * 1024;
    while (!m_sendFile->atEnd()) {
        QByteArray chunk = m_sendFile->read(qMin(chunkSize, m_sendFile->size() - m_sendFile->pos()));
        if (chunk.isEmpty()) break;
        m_fileSocket->write(chunk);
        if (!m_fileSocket->waitForBytesWritten(5000)) {
            emit errorOccured(QStringLiteral("startSendFile: write timeout"));
            return;
        }
    }
}
