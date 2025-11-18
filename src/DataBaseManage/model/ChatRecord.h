#ifndef CHATRECORD_H
#define CHATRECORD_H

#pragma once
#include <QString>

struct ChatRecord
{
    int id = 0;                  // 自增主键
    QString msgId;               // 消息唯一ID
    int fromId;              // 发送者ID
    int toId;                // 接收者ID
    QString content;             // 消息内容
    int type = 0;                // 消息类型（例如：0=文本，1=图片，2=语音等）
    qint64 timestamp = 0;        // 消息时间戳（秒或毫秒）
    int status = 0;              // 消息状态（例如：0=未读，1=已读，2=发送中，3=失败）
};

#endif // CHATRECORD_H
