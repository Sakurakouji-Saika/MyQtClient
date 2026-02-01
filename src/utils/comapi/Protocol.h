#ifndef PROTOCOL_H
#define PROTOCOL_H
namespace Protocol  {
// 消息类型

enum class MessageType: int   {
    // ===== 用户认证相关 =====
    Login            = 1001,  // 登录
    Register         = 1002,  // 注册
    LoginRepeat      = 1003,  // 重复登录
    ChangePassword   = 1004,  // 修改密码
    RecoverPassword  = 1005,  // 找回密码
    KickNotification = 1006,  // 异地登录,提示客户端被强制下线


    // ===== 好友管理相关 =====
    AddFriend        = 2001,  // 添加好友请求
    AgreeFriend      = 2002,  // 同意好友请求
    DeleteFriend     = 2003,  // 删除好友
    FriendOnline     = 2004,  // 好友上线通知
    FriendOffline    = 2005,  // 好友下线通知
    GetMyFriends     = 2006,  // 获取好友列表及状态
    RefreshFriends   = 2007,  // 刷新好友状态
    SearchFriend     = 2008,  // 搜索好友
    FriendRequestList    = 2009,  // 好友申请列表信息
    RemovedByFriend  = 2010,  // 被好友删除（别人删自己）
    friendAddedByRequest = 2011,  // 你的好友请求被某个好友接受



    // ===== 文本传输相关 =====
    SendText            = 3001,  // 发送文本消息
    ReceiveText         = 3002,  // 接收文本消息
    MessageAck          = 3003,  // 表示设备已接受消息
    MessageReadReceipt  = 3004,  // 消息已读
    OfflineMessage      = 3005,  // 离线消息


    // ===== 文件传输相关 =====
    SendFileStart    = 3005,  // 发送文件开始
    SendFileChunk    = 3006,  // 发送文件块
    ReceiveFileStart = 3007,  // 接收文件开始
    ReceiveFileChunk = 3008,  // 接收文件块

    // ===== 图片传输相关 =====
    SendImageStart    = 3009,  // 发送图片开始
    SendImageChunk    = 3010,  // 发送图片块
    ReceiveImageStart = 3011,  // 接收图片开始
    ReceiveImageChunk = 3012,  // 接收图片块

    // ===== 用户资料相关 =====
    UpdateAvatarSelf   = 4001,  // 自己更新头像
    UpdateAvatarFriend = 4002,  // 好友头像更新通知
    UpdateUserInfo     = 4003,  // 修改昵称、签名等资料

    // ===== 心跳与系统控制 =====
    HeartbeatPing    = 9001,  // 心跳包
    HeartbeatAck     = 9002,   // 心跳回复

    // ===== 其他杂乱功能 =====
    AvatarFilename   = 9501,   // 查找

    avatarUploadSucceeded   = 9502,   // 用户上传头像成功
    avatarUploadFailed      = 9503,   // 用户上传头像失败
    avatarUploadInfo        = 9504,   // 用户上传过程的信息(比如每个文件块 返回的结果信息)
    RelationDeleteEven      = 9505,   // 关系删除事件

};


    // 聊天对话中的消息类型
    enum class MessageKind : int {
        Text    = 1,
        Image   = 2,
        File    = 3,
        Audio   = 4,
        Video   = 5,
        Unknown = -1   // 如果需要区分未识别类型可使用 Unknown
    };
}

#endif // PROTOCOL_H
