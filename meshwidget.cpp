#include <QKeyEvent>

#include "meshwidget.h"
#include "edge.h"
#include "node.h"

static void reordreData(
    int const dataIn[],
    int dataOut[],
    int const order[],
    int n)
{
  for (int i = 0; i < n; i++)
    dataOut[order[i]] = dataIn[i];
}

static void getNodeIds(apf::Numbering* n, apf::MeshEntity* e, apf::NewArray<int>& ids)
{
  apf::Mesh* m = apf::getMesh(n);
  int etype = m->getType(e);
  int edim = apf::Mesh::typeDimension[etype];
  apf::FieldShape* fs = apf::getShape(n);
  apf::EntityShape* es = fs->getEntityShape(etype);
  int nen = es->countNodes(); // total nodes

  apf::DynamicArray<int> order;
  apf::DynamicArray<int> data;

  ids.allocate(nen);

  int s = 0;
  for (int d = 0; d <= edim; d++){
    if (!fs->hasNodesIn(d)) continue;
    apf::Downward a;
    int na = m->getDownward(e, d, a);
    for (int i = 0; i < na; i++)
    {
      int nan = fs->countNodesOn(m->getType(a[i]));
      order.setSize(nan);
      data.setSize(nan);
      for (int j = 0; j < nan; j++)
        data[j] = apf::getNumber(n, a[i], j, 0);
      if (nan > 1 && edim !=d)
      {
        es->alignSharedNodes(m, e, a[i], &order[0]);
        reordreData(&data[0], &ids[s], &order[0], nan);
      }
      else if(nan)
      {
        for (int j = 0; j < nan; j++)
          ids[s+j] = data[j];
      }
      s += nan;
    }
  }
}

static int computeTriNodeIndex(int P, int i, int j)
{
  int k = P-i-j;
  if(i == P) return 0;
  if(j == P) return 1;
  if(k == P) return 2;
  if(k == 0) return 2+j; // 0-1
  if(i == 0) return 2+(P-1)+k; // 1-2
  if(j == 0) return 2+(P-1)*2+i; // 2-0
  return k*(P-1)-k*(k-1)/2+j+2*P;
}

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
    setWindowTitle(tr("Mesh Editor"));

    apf::FieldShape* fs = m_mesh->getShape();
    apf::Numbering* nodeIds = apf::createNumbering(m_mesh, "node_id", fs, 1);
    int id = 0;
    apf::MeshEntity* e;
    apf::MeshIterator* it;
    for (int d=0; d<=m_mesh->getDimension(); d++) {
        if (!fs->hasNodesIn(d)) continue;
        it = m_mesh->begin(d);
        while ((e = m_mesh->iterate(it)))
        {
            int non = fs->countNodesOn(m_mesh->getType(e));
            for (int n=0; n<non; n++) {
                apf::number(nodeIds, e, n, 0, id);
                id++;
            }

        }
        m_mesh->end(it);
    }

    QList<Node*> allNodes;

    // Add the (control net) Nodes
    // They all can be added in a single loop (i.e., vert nodes, edge nodes and face nodes)
    for (int d=0; d<=m_mesh->getDimension(); d++) {
        if (!fs->hasNodesIn(d)) continue;
        it = m_mesh->begin(d);
        while ((e = m_mesh->iterate(it)))
        {
            apf::Mesh::Type mtype = m_mesh->getType(e);
            int non = fs->countNodesOn(mtype);
            for (int n=0; n<non; n++) {
                Node* nd = new Node(this, mtype, n, e);
                scene->addItem(nd);
                allNodes.push_back(nd);
            }
        }
        m_mesh->end(it);
    }

    // Add the (control net) Edges
    // 1. Edges corresponding to mesh edges
    it = m_mesh->begin(1);
    while ((e = m_mesh->iterate(it)))
    {
        int non = fs->countNodesOn(m_mesh->getType(e));
        QList<Node*> ns(non+2);
        apf::MeshEntity* vs[2];
        m_mesh->getDownward(e, 0, vs);
        ns[0] = allNodes[apf::getNumber(nodeIds, vs[0], 0, 0)];
        ns[non+2-1] = allNodes[apf::getNumber(nodeIds, vs[1], 0, 0)];
        for (int n=0; n<non; n++) {
            ns[n+1] = allNodes[apf::getNumber(nodeIds, e, n, 0)];
        }

        for (int n=0; n<non+1; n++) {
            scene->addItem(new Edge(e, ns[n], ns[n+1]));
        }
    }
    m_mesh->end(it);
    // 2. Edges corresponding to mesh faces
    int P = m_mesh->getShape()->getOrder();
    it = m_mesh->begin(2);
    while ((e = m_mesh->iterate(it)))
    {
        apf::NewArray<int> ids;
        getNodeIds(nodeIds, e, ids);
        for (int k=1; k<P; k++)
            for (int i=P-k-1; i>=0; i--)
            {
                int j = P-i-k;
                printf("node C_%d,%d,%d\n", i, j, k);
                int currentNodeIndex = ids[computeTriNodeIndex(P,i,j)];
                int    leftNodeIndex = ids[computeTriNodeIndex(P,i+1,j-1)];
                int    downNodeIndex = ids[computeTriNodeIndex(P,i+1,j)];

                Node* currentNode = allNodes[currentNodeIndex];
                Node*    leftNode = allNodes[leftNodeIndex];
                Node*    downNode = allNodes[downNodeIndex];
                scene->addItem(new Edge(e, leftNode, currentNode));
                scene->addItem(new Edge(e, currentNode, downNode));
                scene->addItem(new Edge(e, leftNode, downNode));
            }
    }
    m_mesh->end(it);

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
        crv::writeCurvedVtuFiles(m_mesh, apf::Mesh::TRIANGLE, 5, "changed_mesh");
        crv::writeCurvedWireFrame(m_mesh, 10, "changed_mesh");
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

