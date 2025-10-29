#include "FileManager.h"
#include <QDebug>
#include <QHostAddress>

FileManager::FileManager(QObject *parent)
    : QObject(parent),
    totalSendBytes(0),
    bytesWritten(0),
    bytesToWrite(0),
    totalRecvBytes(0),
    bytesReceived(0),
    fileNameSize(0),
    tcpSocket(nullptr),
    busy(false),
    winId(-1),
    curType(Unknow)
{

    recvPath = AppConfig::instance().filesDirectory();
    headPath = AppConfig::instance().dataDirectory();

    initSocket();
}

FileManager::~FileManager()
{
    if (tcpSocket) {
        tcpSocket->abort();
        tcpSocket->deleteLater();
    }
}

bool FileManager::isConnection() const
{
    return tcpSocket && tcpSocket->isOpen();
}

void FileManager::displayError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << "FileManager socket error:" << tcpSocket->errorString();
    if (tcpSocket) tcpSocket->close();
    // 重置状态（根据需要）
    resetTransferState();
}

void FileManager::StartTransferFile(const QString &filePath)
{
    if (busy) {
        qDebug() << "FileManager busy, reject new transfer";
        return;
    }

    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "StartTransferFile: file not exists:" << filePath;
        return;
    }

    // 打开准备读取（延迟真正读取直到写发生）
    fileToSend.close();
    fileToSend.setFileName(filePath);
    if (!fileToSend.open(QIODevice::ReadOnly)) {
        qDebug() << "StartTransferFile: open read failed:" << filePath;
        return;
    }

    curType = SendFile;
    busy = true;

    // 构造头部（两个 qint64 占位 + 文件名）
    outHeader.clear();
    QDataStream headerStream(&outHeader, QIODevice::WriteOnly);
    headerStream.setVersion(QDataStream::Qt_4_8);

    QString currentFileName = QFileInfo(filePath).fileName();
    headerStream << qint64(0) << qint64(0) << currentFileName; // 占位

    // 总大小 = header size + actual file size
    totalSendBytes = outHeader.size() + fileToSend.size();

    // 回写真正的总大小与文件名长度（替换前面两个占位）
    headerStream.device()->seek(0);
    headerStream << qint64(totalSendBytes) << qint64(outHeader.size() - sizeof(qint64) * 2);

    // 先发送头部
    qint64 written = 0;
    if (tcpSocket && tcpSocket->isOpen()) {
        written = tcpSocket->write(outHeader);
        if (written < 0) written = 0;
        bytesWritten += written;
    } else {
        // 如果未连接，可在外部先调用 ConnectToServer 或者这里触发连接（按需）
        qDebug() << "StartTransferFile: socket not connected, please ConnectToServer first.";
        // 清理
        fileToSend.close();
        busy = false;
        curType = Unknow;
        totalSendBytes = 0;
        bytesWritten = 0;
        return;
    }

    outHeader.clear();

    // 剩余需要从文件写入到 socket 的字节数（只算文件内容）
    bytesToWrite = totalSendBytes - bytesWritten;

    // 尝试立刻写第一块数据（如果 bytesToWrite > 0）
    if (bytesToWrite > 0) {
        QByteArray chunk = fileToSend.read(qMin(bytesToWrite, DEFAULT_CHUNK_SIZE));
        qint64 w = tcpSocket->write(chunk);
        if (w > 0) {
            bytesWritten += w;
            bytesToWrite -= w;
        }
        // 如果写不完，剩下将在 onBytesWritten 回调里继续
    }

    emit signalUpdateProgress(static_cast<quint64>(bytesWritten),
                              static_cast<quint64>(totalSendBytes));
}

void FileManager::ConnectToServer(const QString &ip, int port, int winId)
{
    if (!tcpSocket) initSocket();
    if (tcpSocket->isOpen()) return;

    winId = winId;
    winId = winId; // 避免未使用警告
    this->winId = winId;

    tcpSocket->connectToHost(QHostAddress(ip), port);
}

void FileManager::RequestFile(const QString &remoteFileName)
{
    if (!tcpSocket || !tcpSocket->isOpen()) {
        qDebug() << "RequestFile: socket not connected";
        return;
    }

    // 简单协议：发送一个命令码（使用 EType 的 ReceiveFile）和文件名
    QByteArray cmd;
    QDataStream out(&cmd, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    // 我们这里使用一个小型请求包：先写一个 qint8 类型命令，再写文件名
    quint8 reqType = static_cast<quint8>(ReceiveFile);
    out << reqType << remoteFileName;

    qint64 w = tcpSocket->write(cmd);
    if (w <= 0) {
        qDebug() << "RequestFile: write failed for" << remoteFileName;
    } else {
        qDebug() << "RequestFile: requested" << remoteFileName;
        // 标记当前正在接收文件（等待服务器回复）
        curType = ReceiveFile;
    }
}

void FileManager::CloseConnection()
{
    FileTransFinished();
    if (tcpSocket) tcpSocket->abort();
}

void FileManager::FileTransFinished()
{
    busy = false;
    curType = Unknow;

    // 关闭并复位 send/recv 文件
    if (fileToSend.isOpen()) fileToSend.close();
    if (fileToRecv.isOpen()) fileToRecv.close();

    totalSendBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;

    totalRecvBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    recvFileName.clear();
    inBuffer.clear();
}

void FileManager::SetUserId(QString id)
{
    winId = id;
}

void FileManager::initSocket()
{
    if (tcpSocket) {
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }
    tcpSocket = new QTcpSocket(this);

    // 新式 connect
    connect(tcpSocket, &QTcpSocket::bytesWritten, this, &FileManager::onBytesWritten);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &FileManager::onReadyRead);
    connect(tcpSocket, &QTcpSocket::connected, this, &FileManager::onConnected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &FileManager::onDisconnected);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &FileManager::displayError);

    // 初始化状态
    resetTransferState();
}

void FileManager::resetTransferState()
{
    busy = false;
    curType = Login; // 初始为 Login（保持原逻辑）
    totalSendBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    totalRecvBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    outHeader.clear();
    inBuffer.clear();
    if (fileToSend.isOpen()) fileToSend.close();
    if (fileToRecv.isOpen()) fileToRecv.close();
}

/**
 * 当 socket 写入一定字节后会触发该回调，我们在这里尽可能继续把文件剩余部分写入 socket
 */
void FileManager::onBytesWritten(qint64 bytes)
{
    if (bytes <= 0) return;

    // 如果是 Login 状态（首次连接上服务器后有一次触发），保持兼容原逻辑
    if (curType == Login) {
        curType = Unknow;
        qDebug() << "onBytesWritten: switched from Login to Unknow";
        return;
    }

    // 更新已写计数
    bytesWritten += bytes;

    // 如果还有文件内容等待写入
    if (bytesToWrite > 0 && fileToSend.isOpen() && tcpSocket->isWritable()) {
        // 连续写，直到 socket 返回 0 或写完一块
        QByteArray chunk = fileToSend.read(qMin(bytesToWrite, DEFAULT_CHUNK_SIZE));
        if (!chunk.isEmpty()) {
            qint64 w = tcpSocket->write(chunk);
            if (w > 0) {
                bytesWritten += w;
                bytesToWrite -= w;
            }
        }
    }

    emit signalUpdateProgress(static_cast<quint64>(bytesWritten),
                              static_cast<quint64>(totalSendBytes));

    // 发送完成
    if (totalSendBytes > 0 && bytesWritten >= totalSendBytes) {
        if (fileToSend.isOpen()) fileToSend.close();

        bytesWritten = 0;
        totalSendBytes = 0;
        bytesToWrite = 0;

        emit signalSendFinished();
        FileTransFinished();
    }
}

/**
 * 处理接收（支持头部可能和文件内容粘在一起、多次到达等）
 */
void FileManager::onReadyRead()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_8);

    // 我们用一个简单的状态机来解析：
    // 1) 先读两个 qint64（total + filename size）
    // 2) 读 file name（fileNameSize）
    // 3) 剩下的数据写入文件直到 totalRecvBytes 完成

    while (tcpSocket->bytesAvailable() > 0) {
        // Step A: 读取头部（两个 qint64）

        if (bytesReceived <= static_cast<qint64>(sizeof(qint64) * 2)) {
            const qint64 headerLen = sizeof(qint64) * 2;

            if (tcpSocket->bytesAvailable() >= headerLen && fileNameSize == 0) {
                in >> totalRecvBytes >> fileNameSize;
                if (totalRecvBytes != 0) {
                    bytesReceived += headerLen;
                } else {
                    // 出错的 total，重置
                    qDebug() << "onReadyRead: invalid totalRecvBytes=0";
                    return;
                }
            }

            // 读取文件名
            if (fileNameSize > 0 &&
                tcpSocket->bytesAvailable() >= fileNameSize &&
                (totalRecvBytes > 0)) {
                in >> recvFileName;
                // 拼接目录（根据 winId 选择路径）
                QString targetPath = (winId == -2) ? headPath : recvPath;
                recvFileName = QDir(targetPath).filePath(recvFileName);

                bytesReceived += fileNameSize;

                fileToRecv.close();
                fileToRecv.setFileName(recvFileName);
                if (!fileToRecv.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                    qDebug() << "onReadyRead: failed to open recv file:" << recvFileName;
                    // 放弃本次接收
                    FileTransFinished();
                    return;
                }
                qDebug() << "Begin to recv file:" << recvFileName;
            }
        }

        // Step B: 读取文件内容 (如果 header 已经读完)
        if (bytesReceived < totalRecvBytes && fileToRecv.isOpen()) {
            qint64 avail = tcpSocket->bytesAvailable();
            QByteArray chunk = tcpSocket->read(avail);
            if (!chunk.isEmpty()) {
                qint64 wrote = fileToRecv.write(chunk);
                if (wrote > 0) {
                    bytesReceived += wrote;
                }
            }
        }

        emit signalUpdateProgress(static_cast<quint64>(bytesReceived),
                                  static_cast<quint64>(totalRecvBytes));

        // 完成接收
        if (totalRecvBytes > 0 && bytesReceived >= totalRecvBytes) {
            if (fileToRecv.isOpen()) fileToRecv.close();
            emit signalFileRecvOk( /* type */ static_cast<quint8>(0), fileToRecv.fileName());
            qDebug() << "File recv ok:" << fileToRecv.fileName();
            // 清理状态，准备下一次接收
            FileTransFinished();
            // 保持 while 循环的一致性，如果还有数据，继续处理（下一个包）
        } else {
            // 如果未完成，则退出循环，等待下次 readyRead
            break;
        }
    }
}

void FileManager::onConnected()
{
    curType = Login;

    // 连接后发送本机 ID 与 winId，保持原逻辑
    QByteArray header;
    QDataStream out(&header, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    // 假设 MyApp::m_nId 可用，这里示例用 0（请替换为实际 ID）
    qint32 localId = AppConfig::instance().getUserID().toInt();
    out << localId << QString(winId);

    tcpSocket->write(header);

    qDebug() << "onConnected: sent ID and winId";
    emit signalConnected();
}

void FileManager::onDisconnected()
{
    if (tcpSocket && tcpSocket->isOpen()) tcpSocket->close();
    qDebug() << "onDisconnected called";
    // 你可以在这里选择是否清理状态
    FileTransFinished();
}
