#ifndef RECENT_MESSAGES_H
#define RECENT_MESSAGES_H

#pragma once
#include <string>


// 这里表有问题，再插入的时候，ID会随意更改。反正有问题，测试的时候，应该是测试语句的问题，或者说不是问题，当插入一个已有的时候，会更改已有的ID。
struct RecentMessage {
    int id;                 // 自增主键
    std::string peer_id;    // 唯一标识
    std::string last_msg;   // 最后一条消息
    long long last_time;    // 时间戳
    int unread_count;       // 未读消息数
    int direction;          // 消息方向

    // 构造函数（带默认值）
    RecentMessage(
        int id_ = 0,
        const std::string& peer_id_ = "",
        const std::string& last_msg_ = "",
        long long last_time_ = 0,
        int unread_count_ = 0,
        int direction_ = 0
        ) :
        id(id_),
        peer_id(peer_id_),
        last_msg(last_msg_),
        last_time(last_time_),
        unread_count(unread_count_),
        direction(direction_)
    {}
};


#endif // RECENT_MESSAGES_H
