#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QQueue>
#include <QPair>
#include <QJsonArray>

#include "../networkadapter.h"
#include "../../utils/comapi/unit.h"


class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(NetworkAdapter *msgClient = nullptr,
                         const QString &host = QString(),
                         quint16 msgPort = 60100,
                         quint16 filePort = 60101,
                         QObject *parent = nullptr);
    ~FileManager();

    // 普通文件上传（用于转发给其他用户）
    void uploadFile(const QString &localFilePath, const QString &remoteFileName, int targetUserId);

    // 请求服务器把文件发给当前已注册的 file 连接（用于下载）
    void requestDownloadFile(const QString &remoteFileName);

    // 上传头像（会把文件发送到文件服务器，并通过消息服务器发送 UpdateHeadPic）
    void uploadAvatar(const QString &localFilePath, const QString &remoteFileName = QString(), const QJsonArray &friends = QJsonArray());

    // 下载头像（确保已 connectToFileServer 并 registerIdentity）
    void downloadAvatar(const QString &remoteFileName);

    // 连接并注册到文件服务器
    void connectToFileServer();
    void registerFileIdentity(int userId, int windowId = -2);

    bool isFileServerConnected() const;

signals:
    void fileReceived(const QString &localPath);
    void uploadProgress(qint64 bytesSent, qint64 totalBytes);
    void errorOccured(const QString &msg);

private slots:
    void onFileSocketConnected();
    void onFileSocketDisconnected();
    void onFileSocketReadyRead();
    void onFileSocketBytesWritten(qint64 bytes);

private:
    NetworkAdapter *m_msgClient; // 用于发送控制消息到消息服务器
    QTcpSocket *m_fileSocket; // 与文件服务器的连接

    QString m_host;
    quint16 m_msgPort;
    quint16 m_filePort;

    int m_userId;
    int m_windowId; // -2 表示头像目录

    // 发送相关
    QFile *m_sendFile;
    qint64 m_totalSendBytes;
    qint64 m_bytesToWrite;
    qint64 m_bytesWritten;
    QByteArray m_outBlock;

    // 接收相关
    QFile *m_recvFile;
    qint64 m_recvTotalBytes;
    qint64 m_recvBytesReceived;
    qint64 m_recvFileNameSize;
    QString m_recvFileName;
    QByteArray m_inBlock;

    // 内部工具
    void sendFileHeader(const QString &fileName, qint64 fileSize);
    void startSendFile();
};

#endif // FILEMANAGER_H
