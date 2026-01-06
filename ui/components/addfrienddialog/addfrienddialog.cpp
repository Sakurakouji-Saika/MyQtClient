#include "addfrienddialog.h"
#include "ui_addfrienddialog.h"
#include <QMessageBox>
#include <QDebug>
#include "../../src/Network/Service/servicemanager.h"
#include "../../Network/Service/friendservice.h"
#include "../../Network/Service/avatarservice.h"
#include <QString>

addfrienddialog::addfrienddialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::addfrienddialog)
    , m_sm(nullptr)
    , m_fs(nullptr)
    , isConnected(false)  // 初始化连接标志
{
    ui->setupUi(this);
    StyleLoader::loadWidgetStyle(this, ":/styles/addfrienddialog.css");
    ui->AF_userInfo->setVisible(false);

    QSize avatarSize;
    avatarSize.setWidth(40);
    avatarSize.setHeight(40);
}

addfrienddialog::~addfrienddialog()
{
    delete ui;
}

void addfrienddialog::setNetWork(ServiceManager *_sm)
{
    m_sm = _sm;
    m_fs = m_sm->friendApi();  // 保存 FriendService 指针

    // 一次性连接信号
    if (m_fs && !isConnected) {
        connect(m_fs, &FriendService::SearchFriednSuccessSignals,
                this, &addfrienddialog::onSearchFriendSuccess);
        isConnected = true;
        qDebug() << "FriendService 信号连接成功";
    }
}

void addfrienddialog::onSearchFriendSuccess(qint64 uid, QString userName, QString nickname, qint64 avatar_file_id, QString avatar)
{
    qDebug() << "addfrienddialog::onSearchFriendSuccess::username::" << userName;
    ui->Af_userName->setText(userName);
}

void addfrienddialog::on_add_friend_btn_clicked()
{
    // 开始查找搜索账号
    QString user_id = ui->AF_lineEdit->text();

    if(user_id.isEmpty()){
        QMessageBox::warning(this, "提示", "搜索账号不能为空");
        return;
    }

    // if(DataBaseManage::instance()->isFriend(user_id.toInt())){
    //     QMessageBox::warning(this, "提示", "该账号已是好友");
    //     return;
    // }

    // 如果想隐藏整个 AF_userInfo：
    if (!ui->AF_userInfo) return;
    ui->AF_userInfo->setVisible(true);

    // 确保 m_fs 已经初始化
    if (!m_fs) {
        if (m_sm) {
            m_fs = m_sm->friendApi();
            // 如果之前没连接过，现在连接
            if (m_fs && !isConnected) {
                connect(m_fs, &FriendService::SearchFriednSuccessSignals,
                        this, &addfrienddialog::onSearchFriendSuccess);
                isConnected = true;
            }
        } else {
            QMessageBox::warning(this, "错误", "网络服务未初始化");
            return;
        }
    }

    // 发起搜索
    m_fs->search_friends(user_id.toLongLong());

    // 设置加载头像
    ui->af_user_2_avatar->setAvatar(user_id.toLongLong(), 40);

    // 请求头像信息
    if (m_sm && m_sm->avatar()) {
        m_sm->avatar()->RequestAvatarInfoByUserID(user_id.toLongLong(),true);
    }

    ui->Af_userID->setText(user_id);

    // 移除了重复的 connect 调用
}

// 不知道有什么用的函数
void addfrienddialog::on_return_addFriend_Info(const QJsonValue &info)
{
    qDebug() << "void addfrienddialog::on_return_addFriend_Info(const QJsonValue &info) 触发";
    QJsonObject dataObj = info.toObject();
    int id = dataObj.value("id").toInt();
    QString name = dataObj.value("name").toString();
    QString avatar = dataObj.value("head").toString();
    int status = dataObj.value("status").toInt();

    QString headAvatar = dataObj.value("head").toString();

    QJsonObject json;
    json.insert("from", name);
    json.insert("id", -2);

    ui->Af_userName->setText(name);
    ui->Af_userID->setText("ID:" + QString::number(id));
}

void addfrienddialog::on_AF_use_2_btn_clicked()
{
    // 添加好友按钮点击逻辑
}
