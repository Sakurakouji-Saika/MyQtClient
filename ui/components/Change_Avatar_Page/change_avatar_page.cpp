#include "change_avatar_page.h"
#include "ui_change_avatar_page.h"

#include "dragscrollarea.h"


// 辅助函数：安全地获取 QLabel 中的 QPixmap
static QPixmap getLabelPixmap(const QLabel* label)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return label->pixmap(Qt::ReturnByValue);
#else
    const QPixmap* pixmap = label->pixmap();
    return pixmap ? *pixmap : QPixmap();
#endif
}

Change_Avatar_Page::Change_Avatar_Page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Change_Avatar_Page)
{
    ui->setupUi(this);
    m_maskWidget =nullptr;

    initializeUI();
    connectSignals();
}

Change_Avatar_Page::~Change_Avatar_Page()
{
    delete ui;
}

void Change_Avatar_Page::initializeUI()
{
    ui->sliderScale->setEnabled(false);

    // 创建遮罩层
    if (ui->previewScrollArea) {
        m_maskWidget = new QWidget(ui->previewScrollArea->viewport());
        m_maskWidget->setAttribute(Qt::WA_TranslucentBackground);
        m_maskWidget->hide();
        m_maskWidget->installEventFilter(this);
    }
}

void Change_Avatar_Page::connectSignals()
{
    connect(ui->btnUpload, &QPushButton::clicked, this, &Change_Avatar_Page::onUpload);
    connect(ui->sliderScale, &QSlider::valueChanged, this, &Change_Avatar_Page::onScaleChanged);
    connect(ui->btnRotateCCW, &QPushButton::clicked, this, [this](){ rotateHeader(false); });
    connect(ui->btnRotateCW, &QPushButton::clicked, this, [this](){ rotateHeader(true); });
    connect(ui->btnOk, &QPushButton::clicked, this, &Change_Avatar_Page::onOk);
    connect(ui->btnCancel, &QPushButton::clicked, this, &Change_Avatar_Page::onCancel);
    connect(ui->previewScrollArea, &DragScrollArea::zoomChanged, [this](int num){
        int minV = ui->sliderScale->minimum();
        int maxV = ui->sliderScale->maximum();
        int cur  = ui->sliderScale->value();
        int next = qBound(minV, cur + num, maxV);

        {
            QSignalBlocker blocker(ui->sliderScale); // 阻止 valueChanged 信号触发回路
            ui->sliderScale->setValue(next);
        }

        // 手动触发一次缩放处理，和滑块变化保持一致
        onScaleChanged(next);
    });
}

void Change_Avatar_Page::rotateHeader(bool clockwise)
{
    if (m_sourceHeader.isNull()) return;

    // 更新旋转角度（保持在 0/90/180/270）
    m_rotation = (m_rotation + (clockwise ? 90 : -90)) % 360;
    if (m_rotation < 0) m_rotation += 360;

    // 重新应用当前滑块对应的缩放比例并保持视口中心
    int curValue = ui->sliderScale->value();
    const double scale = qMax(static_cast<double>(curValue) / ui->sliderScale->maximum(), MIN_SCALE);
    updateScaledPixmap(scale);
}

QPixmap Change_Avatar_Page::getRotatedSource() const
{
    if (m_sourceHeader.isNull()) return QPixmap();

    if (m_rotation % 360 == 0) return m_sourceHeader;

    QTransform transform;
    transform.rotate(m_rotation);
    return m_sourceHeader.transformed(transform, Qt::SmoothTransformation);
}

void Change_Avatar_Page::loadHeader(const QPixmap& pixmap)
{
    if (pixmap.isNull()) return;

    m_sourceHeader = pixmap;
    const QSize sourceSize = m_sourceHeader.size();

    // 计算初始缩放尺寸
    QPixmap scaledPixmap;
    int sliderMinimum = 0;

    if (sourceSize.width() > sourceSize.height()) {
        scaledPixmap = m_sourceHeader.scaledToHeight(ui->previewScrollArea->height(), Qt::SmoothTransformation);
        if (sourceSize.height() > 0) {
            sliderMinimum = scaledPixmap.height() * ui->sliderScale->maximum() / sourceSize.height();
        }
    } else {
        scaledPixmap = m_sourceHeader.scaledToWidth(ui->previewScrollArea->width(), Qt::SmoothTransformation);
        if (sourceSize.width() > 0) {
            sliderMinimum = scaledPixmap.width() * ui->sliderScale->maximum() / sourceSize.width();
        }
    }

    // 更新UI
    // 使用实际展示的 scaledPixmap 尺寸作为 content 尺寸，避免初始尺寸跳动
    ui->previewContent->resize(scaledPixmap.size());
    ui->sliderScale->setEnabled(true);
    ui->sliderScale->setMinimum(sliderMinimum);
    ui->sliderScale->setValue(sliderMinimum);
    ui->labelPreview->setPixmap(scaledPixmap);

    // 显示遮罩层
    if (m_maskWidget) {
        m_maskWidget->show();
        m_maskWidget->resize(ui->previewScrollArea->viewport()->size());
        m_maskWidget->raise();
    }
}

bool Change_Avatar_Page::eventFilter(QObject* watched, QEvent* event)
{
    if (m_maskWidget && watched == m_maskWidget && event->type() == QEvent::Paint) {
        m_maskWidget->resize(ui->previewScrollArea->viewport()->size());
        QPainter painter(m_maskWidget);
        drawMaskLayer(painter, m_maskWidget->rect());
        return true;
    }

    return QWidget::eventFilter(watched, event);
}

void Change_Avatar_Page::drawMaskLayer(QPainter& painter, const QRect& rect)
{
    painter.setRenderHint(QPainter::Antialiasing);  //设置渲染方式为：抗锯齿


    const int diameter = qMin(rect.width(), rect.height());
    //diameter 是直径,qmin是返回2个值里面的最小的。

    const QRect circleRect(
        (rect.width() - diameter) / 2,   // x 坐标，这里应该的结果应该是为0，但这样写是为了兼容
        (rect.height() - diameter) / 2,  // y 坐标，这里应该的结果应该是为0，但这样写是为了兼容
        diameter,                        // 宽度
        diameter                         // 高度
        );
    // circleRect 是圆形矩形

    // 方法1：使用路径绘制环形
    QPainterPath outerPath;
    outerPath.addRect(rect);

    QPainterPath innerPath;
    innerPath.addEllipse(circleRect);
    // 用 addEllipse(circleRect) 向其中加入一个椭圆形（如果是正方形就是圆形）

    // 外矩形减去内圆形 = 环形
    QPainterPath ringPath = outerPath - innerPath;

    // 填充环形区域为半透明黑色
    painter.fillPath(ringPath, QColor(0, 0, 0, 120));

    // 填充圆形区域为透明
    painter.fillPath(innerPath, QColor(255, 255, 255, 0));

    // 创建一个画笔对象，并设置颜色为白色，用于绘制圆形边框
    QPen pen(Qt::white);

    // 设置画笔宽度为 BORDER_WIDTH 像素，控制边框粗细
    pen.setWidth(BORDER_WIDTH);

    // 将画笔应用到 QPainter 上，后续绘制使用这个画笔
    painter.setPen(pen);

    // 设置画刷为空，表示绘制时不填充图形内部
    painter.setBrush(Qt::NoBrush);

    // 在 circleRect 区域内绘制圆形（椭圆），
    // adjusted(1, 1, -1, -1) 将矩形向内缩 1 像素，
    // 避免边框被遮罩层覆盖或超出边界
    painter.drawEllipse(circleRect.adjusted(1, 1, -1, -1));
}

void Change_Avatar_Page::onUpload()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("选择上传图片"),
        "./",
        tr("Images (*.jpg *.png *.bmp)")
        );

    if (!filePath.isEmpty()) {
        QPixmap pixmap(filePath);
        if (!pixmap.isNull()) {
            loadHeader(pixmap);
        } else {
            qDebug() <<"图片加载失败，请选择有效的图片文件";
        }
    }
}

void Change_Avatar_Page::onScaleChanged(int value)
{
    qDebug()<< "::onScaleChanged(int value)" << value;
    if (m_sourceHeader.isNull()) return;


    const double scale = qMax(static_cast<double>(value) / ui->sliderScale->maximum(), MIN_SCALE);
    updateScaledPixmap(scale);
}

void Change_Avatar_Page::updateScaledPixmap(double scale)
{
    // 记录视口中心相对位置（比例），缩放后按比例恢复以避免抖动
    QScrollBar* hBar = ui->previewScrollArea->horizontalScrollBar();
    QScrollBar* vBar = ui->previewScrollArea->verticalScrollBar();
    const QSize curr = ui->previewContent->size();
    QPoint center(hBar->value() + ui->previewScrollArea->viewport()->width()/2,
                  vBar->value() + ui->previewScrollArea->viewport()->height()/2);
    const double cx = curr.width() ? (double)center.x() / curr.width() : 0.5;
    const double cy = curr.height() ? (double)center.y() / curr.height() : 0.5;

    // 缩放旋转后的源图并应用
    QPixmap base = getRotatedSource();
    QPixmap scaled = base.scaled(base.size() * scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelPreview->setPixmap(scaled);
    ui->previewContent->resize(scaled.size());

    // 在事件循环末尾按比例恢复滚动条位置
    const QSize updated = ui->previewContent->size();
    const int cxPos = qBound(0, int(qRound(cx * updated.width())), updated.width());
    const int cyPos = qBound(0, int(qRound(cy * updated.height())), updated.height());
    const int hTarget = qMax(0, cxPos - ui->previewScrollArea->viewport()->width()/2);
    const int vTarget = qMax(0, cyPos - ui->previewScrollArea->viewport()->height()/2);
    QTimer::singleShot(0, this, [this, hTarget, vTarget]{
        ui->previewScrollArea->horizontalScrollBar()->setValue(hTarget);
        ui->previewScrollArea->verticalScrollBar()->setValue(vTarget);
    });

}

void Change_Avatar_Page::onOk()
{
    QPixmap currentPixmap = getLabelPixmap(ui->labelPreview);
    if (!currentPixmap.isNull()) {
        emit updateHeader(currentPixmap);

        if (saveHeader(currentPixmap)) {
            qDebug() <<"修改头像成功，已保存到用户目录";
        } else {
            qDebug() <<"修改头像成功，但保存失败";

        }
    } else {
        qDebug() <<"还未加载头像";
    }
}

void Change_Avatar_Page::onCancel()
{
    qDebug() <<"取消修改头像";
    if (m_maskWidget) {
        m_maskWidget->hide();
    }
    ui->labelPreview->clear();

    m_rotation = 0;
}

bool Change_Avatar_Page::saveHeader(const QPixmap& pixmap)
{
    const QString savePath = QCoreApplication::applicationDirPath() + "/user_header.png";

    QPoint currPos = ui->previewScrollArea->GetLastDragPos();

    QRect rect(currPos.x(), currPos.y(), ui->previewScrollArea->width(), ui->previewScrollArea->height());
    QPixmap cropped = pixmap.copy(rect);

    return cropped.save(savePath);
}

