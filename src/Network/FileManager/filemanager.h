#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDataStream>
#include "../utils/appconfig.h"

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager() override;

    // 是否链接
    bool isConnection() const;

    // 开始传输文件（发送）
    void StartTransferFile(const QString &filePath);

    // 向服务器请求下载远程文件（例如："1001_avatar.png"）
    void RequestFile(const QString &remoteFileName);

    // 连接/断开
    void ConnectToServer(const QString &ip, int port, int usrId);
    void CloseConnection();

    // 文件传输完成（外部可以调用以重置状态）
    void FileTransFinished();

    // 设置当前 socket 的 id
    void SetUserId(QString id);

signals:
    void signalSendFinished();
    void signalFileRecvOk(quint8 type, const QString &filePath);
    void signalUpdateProgress(quint64 currSize, quint64 total);
    void signalConnected();

private slots:
    //显示错误
    void displayError(QAbstractSocket::SocketError socketError);
    void onBytesWritten(qint64 bytes);
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    enum EType : quint8 { Unknow = 0, Login = 1, SendFile = 2, ReceiveFile = 3 };

    // 分块大小（每次从文件读取并写入 socket 的最大字节数）
    static constexpr qint64 DEFAULT_CHUNK_SIZE = 50 * 1024; // 50 KB

    // send (发送)部分
    QFile fileToSend;
    qint64 totalSendBytes;   // 包头 + 文件实际大小
    qint64 bytesWritten;     // 已写入 socket 的总数（包含头）
    qint64 bytesToWrite;     // 还剩多少需要从文件读取并写入 socket（仅文件内容部分）
    QByteArray outHeader;    // 用于保存头部（文件名等）的 QByteArray

    // recv (接收)部分（state machine）
    QFile fileToRecv;
    qint64 totalRecvBytes;   // 本次接收的总字节数（头+文件）
    qint64 bytesReceived;    // 已接收字节数
    qint64 fileNameSize;     // 文件名部分大小（从头读取）
    QString recvFileName;
    QByteArray inBuffer;

    // 通信
    QTcpSocket *tcpSocket;

    // 状态
    bool busy;
    QString winId;
    quint8 curType;

    // 用户目录 / 路径
    QString recvPath;
    QString headPath;

    // internal helpers
    void initSocket();
    void resetTransferState();
};

#endif // FILEMANAGER_H
