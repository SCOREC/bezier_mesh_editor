#include <QKeyEvent>
#include <QDebug>

#include "meshwidget.h"
#include "edge.h"
#include "bezieredge.h"
#include "node.h"


MeshWidget::MeshWidget(MeshWrapper* m, qreal psize, qreal lwidth, QWidget* parent)
    : mesh(m), pointSize(psize), lineWidth(lwidth), QGraphicsView(parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Mesh Editor"));

    qInfo() << "calling getAllNodes";
    QList<Node*> allNodes = mesh->getAllNodes();
    qInfo() << "calling getAllEdges";
    QList<Edge*> allEdges = mesh->getAllEdges();
    qInfo() << "calling getAllBezierEdges";
    QList<BezierEdge*> allBezierEdges = mesh->getAllBezierEdges();
    qInfo() << "adding nodes and edges to the scene";
    int cnt = 0;
    for(auto n : allNodes)
    {
        // set the gometric properies of the nodes (e.g., radius, etc)
        n->setSize(pointSize);
        scene->addItem(n);
        cnt++;
    }
    for(auto e : allEdges)
    {
        // set the geometric properites of the edges (e.g., width, etc)
        e->setWidth(lineWidth);
        // the follwoing is needed here since when nodes are created geometric properies
        // of the nodes (e.g., raduis) are not set
        e->adjust();
        scene->addItem(e);
    }
    for(auto e : allBezierEdges)
    {
        // set the geometric properites of the edges (e.g., width, etc)
        e->setWidth(lineWidth);
        // the follwoing is needed here since when nodes are created geometric properies
        // of the nodes (e.g., raduis) are not set
        e->adjust();
        scene->addItem(e);
    }

}


void MeshWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        break;
    case Qt::Key_Down:
        break;
    case Qt::Key_Left:
        break;
    case Qt::Key_Right:
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
        mesh->write();
        break;
    case Qt::Key_Enter:
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

#if QT_CONFIG(wheelevent)
void MeshWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow(2., -event->angleDelta().y() / 240.0));
}
#endif

void MeshWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
//    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
//                    sceneRect.width() - 4, sceneRect.height() - 4);
//    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
//                       "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
//    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
//    painter->drawText(textRect, message);
}

void MeshWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void MeshWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void MeshWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

// void MeshWidget::shuffle()
// {
    // const QList<QGraphicsItem *> items = scene()->items();
    // for (QGraphicsItem *item : items) {
        // if (qgraphicsitem_cast<Node *>(item))
            // item->setPos(-150 + QRandomGenerator::global()->bounded(300), -150 + QRandomGenerator::global()->bounded(300));
    // }
// }

// void MeshWidget::itemMoved()
// {
    // if (!timerId)
        // timerId = startTimer(1000 / 25);
// }

