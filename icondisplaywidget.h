#ifndef ICONDISPLAYWIDGET_H
#define ICONDISPLAYWIDGET_H

#include <QWidget>

class IconDisplayWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit IconDisplayWidget(QWidget *parent = 0);
    /*
    protected:
        // Override the paintEvent function to draw our own contents
        void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    */

signals:

public slots:

};

#endif // ICONDISPLAYWIDGET_H
