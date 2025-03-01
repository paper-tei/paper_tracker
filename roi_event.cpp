//
// Created by JellyfishKnight on 25-3-1.
//
#include "roi_event.hpp"

#include <iostream>
#include <winsock2.h>

ROIEventFilter::ROIEventFilter(std::function<void(QRect rect, bool is_end)> func, QObject *parent)
    : QObject(parent), selecting(false)
{
    this->func = func;
}

bool ROIEventFilter::eventFilter(QObject *obj, QEvent *event) {
    QLabel *label = qobject_cast<QLabel*>(obj);
    if (!label)
        return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            selecting = true;
            selectionStart = mouseEvent->pos();
            selectionRect = QRect(selectionStart, QSize());
            return true;  // 拦截事件
        }
    } else if (event->type() == QEvent::MouseMove && selecting) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        selectionRect = QRect(selectionStart, mouseEvent->pos()).normalized();
        func(selectionRect, false);
        label->update();  // 触发重绘，绘制选区
        return true;
    } else if (event->type() == QEvent::MouseButtonRelease && selecting) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            selecting = false;
            selectionRect = QRect(selectionStart, mouseEvent->pos()).normalized();
            func(selectionRect, true);
            label->update();
            return true;
        }
    } else if (event->type() == QEvent::Paint) {
        // 首先让 QLabel 完成它自己的绘制
        bool ret = QObject::eventFilter(obj, event);
        // 然后在其上面绘制 ROI 选区
        QPainter painter(label);
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        if (!selectionRect.isNull()) {
            painter.drawRect(selectionRect);
        }
        return ret;
    }
    return QObject::eventFilter(obj, event);
}
