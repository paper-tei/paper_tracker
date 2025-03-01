//
// Created by JellyfishKnight on 25-3-1.
//

#ifndef ROI_EVENT_HPP
#define ROI_EVENT_HPP

#include <QObject>
#include <QRect>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>
#include <QDebug>


class ROIEventFilter final : public QObject {
    // Q_OBJECT
public:
    explicit ROIEventFilter(std::function<void(QRect rect)>, QObject *parent = nullptr);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    QRect selectionRect;
    QPoint selectionStart;
    bool selecting;

    std::function<void(QRect rect)> func;
};


#endif //ROI_EVENT_HPP
