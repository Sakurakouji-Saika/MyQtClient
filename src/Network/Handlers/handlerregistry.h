#ifndef HANDLERREGISTRY_H
#define HANDLERREGISTRY_H

#include <QObject>


class ServiceManager;

using JsonHandler = std::function<void(const QJsonObject&)>;

class handlerregistry : public QObject
{
    Q_OBJECT
public:
    explicit handlerregistry(ServiceManager *_sm);


    void registerAll();

    void registerHandler(int messageType,JsonHandler handler);

private:
    ServiceManager *m_sm;
};

#endif // HANDLERREGISTRY_H
