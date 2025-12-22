#ifndef CHANGE_AVATAR_PAGE_H
#define CHANGE_AVATAR_PAGE_H

#include <QWidget>

#include <QFileDialog>
#include <QPainter>
#include <QTransform>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QScrollBar>
#include <QTimer>
#include <QPainterPath>
#include <QPixmap>
#include <qDebug>
#include <QLabel>

#include "../../src/utils/StyleLoader.h"
#include "../../Src/utils/utils.h"


namespace Ui {
class Change_Avatar_Page;
}

class Change_Avatar_Page : public QWidget
{
    Q_OBJECT

public:
    explicit Change_Avatar_Page(QWidget *parent = nullptr);
    ~Change_Avatar_Page();

    void loadHeader(const QPixmap& pixmap);
    void rotateHeader(bool clockwise);
    QPixmap getRotatedSource() const;

signals:
    void avatarUploaded(const QString header);

private slots:
    void onUpload();
    void onScaleChanged(int value);
    void onOk();
    void onCancel();



private:
    Ui::Change_Avatar_Page *ui;

    // 辅助函数
    void initializeUI();
    void connectSignals();
    void updateScaledPixmap(double scale);
    void drawMaskLayer(QPainter& painter, const QRect& rect);
    QPixmap getCurrentPixmap() const;
    std::optional<QString> saveHeader(const QPixmap& pixmap);


    // 常量
    static constexpr int BORDER_WIDTH = 1;  //圆圈边框线
    static constexpr double MIN_SCALE = 0.01;

    QWidget*            m_maskWidget;           // 选择头像区域的遮罩层
    QPixmap             m_sourceHeader;         // 上传的原始头像文件
    int                 m_rotation = 0 ;        // 当前旋转角度（单位：度）
    // QPointF             viewCenterOffset;       // 记录视口中心相对位置



protected:
    virtual bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // CHANGE_AVATAR_PAGE_H
