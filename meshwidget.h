#ifndef MESHWIDGET_H
#define MESHWIDGET_H

#include <QGraphicsView>

#include <meshwrapper.h>


class Node;

class MeshWidget : public QGraphicsView
{
    Q_OBJECT
public:
    MeshWidget(MeshWrapper* m, qreal psize = 1., qreal lwidth = 1., QWidget *parent = nullptr);

    void itemMoved();

public slots:
    // void shuffle();
    void zoomIn();
    void zoomOut();
    MeshWrapper* getMesh() {return mesh;}

protected:
    void keyPressEvent(QKeyEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void scaleView(qreal scaleFactor);

private:

    MeshWrapper* mesh;
    qreal pointSize;
    qreal lineWidth;
};

#endif // MESHWIDGET_H
