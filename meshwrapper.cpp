#include "meshwrapper.h"

MeshWrapper::MeshWrapper(QObject *parent)
    : QObject{parent}
{

}

QList<Node*>& MeshWrapper::getAllNodes()
{
   createAllNodes();
   return nodes;
}

QList<Edge*>& MeshWrapper::getAllEdges()
{
    createAllEdges();
    return edges;
}

int MeshWrapper::getNumNodes()
{
    return nodes.size();
}

int MeshWrapper::getNumEdges()
{
    return edges.size();
}
