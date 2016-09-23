#include "icondisplaywidget.h"
#include <QIcon>

IconDisplayWidget::IconDisplayWidget(QWidget *parent) :
    QWidget(parent)
{
    // Initialize a null icon
    QIcon icon;
}

/* XXX: icon display will be done later; it's actually sort of complicated
void IconDisplayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //QRectF myrect = this->boundingRect();
    painter.drawText("hi");
}
*/
