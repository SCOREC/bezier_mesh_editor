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
