#ifndef APFMESH_H
#define APFMESH_H

#include <QObject>

#include <apf.h>
#include <apfMesh2.h>
#include <apfNumbering.h>
#include <apfMDS.h>

#include <crv.h>

#include "meshwrapper.h"
#include "node.h"
#include "edge.h"


class ApfMesh : public MeshWrapper
{
public:
    ApfMesh(QObject* parent=nullptr);
    // ApfMesh(const char* filename, QObject* parent=nullptr);
    // ApfMesh(apf::Mesh2* m, QObject* parent=nullptr);
    ~ApfMesh();
    virtual void createAllNodes();
    virtual void createAllEdges();
    virtual QPointF getNodePosition(EntPtr e, int n);
    virtual void setNodePosition(EntPtr e, int n, const QPointF& p);
    virtual void write();

private:
    void createNodeIds();

protected:
    apf::Mesh2* apfMesh;
    apf::FieldShape* fieldShape;
    apf::Numbering* nodeIds;
};

#endif // APFMESH_H
