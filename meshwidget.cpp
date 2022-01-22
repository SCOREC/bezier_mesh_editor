#include <QKeyEvent>

#include <crv.h>

#include "meshwidget.h"
#include "edge.h"
#include "node.h"

MeshWidget::MeshWidget(apf::Mesh2* mesh, QWidget* parent)
    : m_mesh(mesh), QGraphicsView(parent)
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
    setWindowTitle(tr("Elastic Nodes"));

    QList<Node*> allNodes;
    QList<Edge*> allEdges;

    apf::MeshEntity* e;
    apf::MeshIterator* it = m_mesh->begin(1);

    while ((e = m_mesh->iterate(it)))
    {
        apf::MeshEntity* vs[2];
        m_mesh->getDownward(e, 0, vs);
        Node* ns[2];
        ns[0] = new Node(this, vs[0]);
        ns[1] = new Node(this, vs[1]);
        allNodes.push_back(ns[0]);
        allNodes.push_back(ns[1]);
        QList<Node*> iNodes;
        allEdges.push_back(new Edge(e, ns[0], ns[1], iNodes));
    }
    m_mesh->end(it);

    for (int i = 0; i<allNodes.size() ; ++i)
        scene->addItem(allNodes[i]);

    for (int i = 0; i<allEdges.size() ; ++i)
        scene->addItem(allEdges[i]);
}


void MeshWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
        crv::writeCurvedWireFrame(m_mesh, 10, "changed_mesh_wire");
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

