#include "bezieredge.h"
#include "node.h"

#include <QPainter>
#include <QtMath>
#include <QDebug>

BezierEdge::BezierEdge(MeshWrapper* m, EntPtr e, QList<Node*> n, int r)
    :mesh(m), ent(e), nodes(n), res(r)
{
    setAcceptedMouseButtons(Qt::NoButton);
    samplePoints = QList<QPointF>(res+1, QPointF());
    for (auto nd:nodes)
        nd->addBezierEdge(this);
    // for(int i=0; i<r+1; i++)
     //         samplePoints.push_back(QPointF());
    adjust();
}

void BezierEdge::adjust()
{
    // set the sample points here using the mesh edge EntPtr
    mesh->sampleEdge(ent, res, samplePoints);
}

QRectF BezierEdge::boundingRect() const
{
    qreal xmin, ymin;
    qreal xmax, ymax;
    xmax = ymax = -1.e32;
    xmin = ymin =  1.e32;

    for (auto p:samplePoints)
    {
        if (p.x() > xmax)
            xmax = p.x();
        if (p.x() < xmin)
            xmin = p.x();
        if (p.y() > ymax)
            ymax = p.y();
        if (p.y() < ymin)
            ymin = p.y();
    }
    return QRectF(QPointF(xmin,ymin), QSizeF(xmax-xmin, ymax-ymin));
}

void BezierEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(QPen(Qt::gray, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    for(int i=0; i<samplePoints.size()-1;i++)
    {
        QLineF line(samplePoints[i], samplePoints[i+1]);
        painter->drawLine(line);
    }
}
