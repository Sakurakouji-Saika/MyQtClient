#ifndef FNPDATA_H
#define FNPDATA_H
#include <QString>
// 一个简单的数据结构体，存储一行所需的数据
struct FNPData {
    QString avatarPath;   // 头像路径（可为空，表示使用占位图）
    QString qq;
    QString userName;
    QString timeText;

    FNPData() = default;
    FNPData(QString a, QString q, QString u, QString t)
        : avatarPath(std::move(a)), qq(std::move(q)), userName(std::move(u)), timeText(std::move(t)) {}
};

#endif // FNPDATA_H
