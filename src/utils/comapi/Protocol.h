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

        // ===== 好友管理相关 =====
        AddFriend        = 2001,  // 添加好友请求
        AgreeFriend      = 2002,  // 同意好友请求
        DeleteFriend     = 2003,  // 删除好友
        FriendOnline     = 2004,  // 好友上线通知
        FriendOffline    = 2005,  // 好友下线通知
        GetMyFriends     = 2006,  // 获取好友列表及状态
        RefreshFriends   = 2007,  // 刷新好友状态
        SearchFriend     = 2008,  // 搜索好友

        // ===== 消息传输相关 =====
        SendText         = 3001,  // 发送文本消息
        ReceiveText      = 3002,  // 接收文本消息
        SendImage        = 3003,  // 发送图片
        ReceiveImage     = 3004,  // 接收图片
        SendFileStart    = 3005,  // 发送文件开始
        SendFileChunk    = 3006,  // 发送文件块
        ReceiveFileStart = 3007,  // 接收文件开始
        ReceiveFileChunk = 3008,  // 接收文件块

        // ===== 用户资料相关 =====
        UpdateAvatarSelf   = 4001,  // 自己更新头像
        UpdateAvatarFriend = 4002,  // 好友头像更新通知
        UpdateUserInfo     = 4003,  // 修改昵称、签名等资料

        // ===== 心跳与系统控制 =====
        HeartbeatPing    = 9001,  // 心跳包
        HeartbeatAck     = 9002   // 心跳回复

    };
}

#endif // PROTOCOL_H
