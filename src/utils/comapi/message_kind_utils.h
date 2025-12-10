#ifndef MESSAGE_KIND_UTILS_H
#define MESSAGE_KIND_UTILS_H

#include <QHash>

namespace Protocol {

    // 消息内容类型（text / image / file / audio / video）
    enum class MessageKind : int {
        Text    = 0,
        Image   = 1,
        File    = 2,
        Audio   = 3,
        Video   = 4,
        Unknown = -1
    };
}

static Protocol::MessageKind msgKindStringToKind(const QString &t)
{
    static const QHash<QString, Protocol::MessageKind> map = {
        { "text",  Protocol::MessageKind::Text  },
        { "image", Protocol::MessageKind::Image },
        { "file",  Protocol::MessageKind::File  },
        { "audio", Protocol::MessageKind::Audio },
        { "video", Protocol::MessageKind::Video }
    };

    const QString key = t.trimmed().toLower();
    auto it = map.find(key);
    if (it != map.end())
        return it.value();

    // 与原逻辑保持一致：默认 text
    return Protocol::MessageKind::Text;
}

static int msgKindStringToInt(const QString &t)
{
    return static_cast<int>(msgKindStringToKind(t));
}

static QString messageKindToString(Protocol::MessageKind k)
{
    switch (k) {
        case Protocol::MessageKind::Text:  return QStringLiteral("text");
        case Protocol::MessageKind::Image: return QStringLiteral("image");
        case Protocol::MessageKind::File:  return QStringLiteral("file");
        case Protocol::MessageKind::Audio: return QStringLiteral("audio");
        case Protocol::MessageKind::Video: return QStringLiteral("video");
        default:                           return QStringLiteral("text");
    }
};



#endif // MESSAGE_KIND_UTILS_H
