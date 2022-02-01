#ifndef MESHWRAPPER_H
#define MESHWRAPPER_H

#include <QObject>

// #include "node.h"
// #include "edge.h"

// #include <apf.h>
// #include <apfMesh2.h>
// #include <apfNumbering.h>


typedef int* EntPtr;
class Node;
class Edge;
class BezierEdge;

class MeshWrapper : public QObject
{
    Q_OBJECT
public:
    explicit MeshWrapper(QObject *parent = nullptr);
    virtual void createAllNodes() = 0;
    virtual void createAllEdges() = 0;
    virtual void createAllBezierEdges() = 0;
    virtual QPointF getNodePosition(EntPtr e, int n) = 0;
    virtual void setNodePosition(EntPtr e, int n, const QPointF& p) = 0;
    virtual void sampleEdge(EntPtr e, int r, QList<QPointF>& points) = 0;
    virtual void write() = 0;
    virtual void addNode(Node* n) {nodes.push_back(n);}
    virtual Node* getNodeAt(int i) {return nodes[i];}
    virtual void addEdge(Edge* e) {edges.push_back(e);}
    virtual void addBezierEdge(BezierEdge* e) {bedges.push_back(e);}
    virtual QList<Node*>& getAllNodes();
    virtual QList<Edge*>& getAllEdges();
    virtual QList<BezierEdge*>& getAllBezierEdges();
    int getNumNodes();
    int getNumEdges();
    int getNumBezierEdges();

private:
    QList<Node*> nodes;
    QList<Edge*> edges;
    QList<BezierEdge*> bedges;

signals:

};

#endif // MESHWRAPPER_H
