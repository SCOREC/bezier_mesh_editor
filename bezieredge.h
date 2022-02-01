#ifndef BEZIEREDGE_H
#define BEZIEREDGE_H

#include <QGraphicsItem>

#include "meshwrapper.h"

class BezierEdge : public QGraphicsItem
{
public:
    BezierEdge(MeshWrapper* m, EntPtr e, QList<Node*> n, int r=10);
    void adjust();
    void setWidth(qreal w) { lineWidth = w; }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    MeshWrapper* mesh;
    EntPtr ent;
    QList<Node*> nodes;
    int res;
    QList<QPointF> samplePoints;
    qreal lineWidth;
};

#endif // BEZIEREDGE_H
