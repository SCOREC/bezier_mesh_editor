#include <QDebug>
#include "apfmesh.h"


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

ApfMesh::ApfMesh(QObject* parent)
{
    apfMesh = apf::makeEmptyMdsMesh(0, 2, false);

    double vert_coords[4][6] = {
        {0.,0.,0., 0., 0., 0.},
        {100.,0.,0., 0., 0., 0.},
        {0.,100.,0., 0., 0., 0.},
        {100.,100.,0., 0., 0., 0.}
    };

    // each edge is defined by the bounding verts
    int edge_info[5][2] = {
        {0,1},
        {1,2},
        {2,0},
        {2,3},
        {3,1}
    };

    // each face is defined by the bounding edges
    int face_info[2][3] = {
        {0,1,2},
        {1,3,4}
    };


    apf::MeshEntity* verts[4];
    apf::MeshEntity* edges[5];
    apf::MeshEntity* faces[2];

    for (int i = 0; i < 4; i++) {
      apf::Vector3 coords(vert_coords[i][0],
                          vert_coords[i][1],
                          vert_coords[i][2]);
      apf::Vector3 params(vert_coords[i][3],
                          vert_coords[i][4],
                          vert_coords[i][5]);
      verts[i] = apfMesh->createVertex(0, coords, params);
    }
    for (int i = 0; i < 5; i++) {
      apf::MeshEntity* down_vs[2] = {verts[edge_info[i][0]],
                                     verts[edge_info[i][1]]};
      edges[i] = apfMesh->createEntity(apf::Mesh::EDGE, 0, down_vs);
    }
    for (int i = 0; i < 2; i++) {
      apf::MeshEntity* down_es[3] = {edges[face_info[i][0]],
                                     edges[face_info[i][1]],
                                     edges[face_info[i][2]]};
      faces[i] = apfMesh->createEntity(apf::Mesh::TRIANGLE, 0, down_es);
    }

    apfMesh->acceptChanges();

    apf::changeMeshShape(apfMesh, crv::getBezier(4),true);
    // mesh->setPoint(edges[0], 0, apf::Vector3(33.,  20., 0.));
    // mesh->setPoint(edges[0], 1, apf::Vector3(67., -20., 0.));
    apfMesh->acceptChanges();

    int order = apfMesh->getShape()->getOrder();
    if (order == 1)
        apf::writeVtkFiles("inital_mesh", apfMesh);
    else
    {
        crv::writeCurvedVtuFiles(apfMesh, apf::Mesh::TRIANGLE, order+2, "initial_mesh");
        crv::writeCurvedWireFrame(apfMesh, 2*order+2, "initial_mesh");
    }

    fieldShape = apfMesh->getShape();
    createNodeIds();
 }

ApfMesh::~ApfMesh()
{
    qInfo() << "apf mesh destructor is being called!";
    apfMesh->destroyNative();
    apf::destroyMesh(apfMesh);
}

void ApfMesh::createAllNodes()
{
    apf::MeshEntity* e;
    apf::MeshIterator* it;

    for (int d=0; d<=apfMesh->getDimension(); d++) {
        if (!fieldShape->hasNodesIn(d)) continue;
        it = apfMesh->begin(d);
        while ((e = apfMesh->iterate(it)))
        {
            apf::Mesh::Type etype = apfMesh->getType(e);
            int non = fieldShape->countNodesOn(etype);
            for (int n=0; n<non; n++) {
                Node* nd = new Node(this, (EntPtr)e, n, d);
                this->addNode(nd);
            }
        }
        apfMesh->end(it);
    }
    qInfo() << "total number of nodes created " << this->getNumNodes();
}

void ApfMesh::createAllEdges()
{
    apf::MeshEntity* e;
    apf::MeshIterator* it;
    // Add the (control net) Edges
    // 1. Edges corresponding to mesh edges
    it = apfMesh->begin(1);
    while ((e = apfMesh->iterate(it)))
    {
        int non = fieldShape->countNodesOn(apfMesh->getType(e));
        QList<Node*> ns(non+2);
        apf::MeshEntity* vs[2];
        apfMesh->getDownward(e, 0, vs);
        ns[0] = this->getNodeAt(apf::getNumber(nodeIds, vs[0], 0, 0));
        ns[non+2-1] = this->getNodeAt(apf::getNumber(nodeIds, vs[1], 0, 0));
        for (int n=0; n<non; n++) {
            ns[n+1] = this->getNodeAt(apf::getNumber(nodeIds, e, n, 0));
        }

        for (int n=0; n<non+1; n++) {
            this->addEdge(new Edge((EntPtr)e, ns[n], ns[n+1]));
        }
    }
    apfMesh->end(it);
    // 2. Edges corresponding to mesh faces
    int P = apfMesh->getShape()->getOrder();
    it = apfMesh->begin(2);
    while ((e = apfMesh->iterate(it)))
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

                Node* currentNode = this->getNodeAt(currentNodeIndex);
                Node*    leftNode = this->getNodeAt(leftNodeIndex);
                Node*    downNode = this->getNodeAt(downNodeIndex);
                this->addEdge(new Edge((EntPtr)e, leftNode, currentNode));
                this->addEdge(new Edge((EntPtr)e, currentNode, downNode));
                this->addEdge(new Edge((EntPtr)e, leftNode, downNode));
            }
    }
    apfMesh->end(it);
    qInfo() << "total number of edges created " << this->getNumEdges();
}

void ApfMesh::createAllBezierEdges()
{
    apf::MeshEntity* e;
    apf::MeshIterator* it;
    it = apfMesh->begin(1);
    while ((e = apfMesh->iterate(it)))
    {
        int non = fieldShape->countNodesOn(apfMesh->getType(e));
        QList<Node*> ns(non+2);
        apf::MeshEntity* vs[2];
        apfMesh->getDownward(e, 0, vs);
        ns[0] = this->getNodeAt(apf::getNumber(nodeIds, vs[0], 0, 0));
        ns[non+2-1] = this->getNodeAt(apf::getNumber(nodeIds, vs[1], 0, 0));
        for (int n=0; n<non; n++) {
            ns[n+1] = this->getNodeAt(apf::getNumber(nodeIds, e, n, 0));
        }
        this->addBezierEdge(new BezierEdge(this, (EntPtr)e, ns, 10));
    }
    apfMesh->end(it);
    qInfo() << "total number of edges created " << this->getNumBezierEdges();
}

void ApfMesh::createNodeIds()
{
    nodeIds = apf::createNumbering(apfMesh, "node_id", fieldShape, 1);
    PCU_ALWAYS_ASSERT(nodeIds);
    int id = 0;
    apf::MeshEntity* e;
    apf::MeshIterator* it;
    for (int d=0; d<=apfMesh->getDimension(); d++) {
        if (!fieldShape->hasNodesIn(d)) continue;
        it = apfMesh->begin(d);
        while ((e = apfMesh->iterate(it)))
        {
            int non = fieldShape->countNodesOn(apfMesh->getType(e));
            for (int n=0; n<non; n++) {
                apf::number(nodeIds, e, n, 0, id);
                id++;
            }
        }
        apfMesh->end(it);
    }
}

QPointF ApfMesh::getNodePosition(EntPtr e, int n)
{
    apf::MeshEntity* ent = (apf::MeshEntity*)e;
    apf::Vector3 p;
    apfMesh->getPoint(ent, n, p);
    return QPointF(p[0], p[1]);
}

void ApfMesh::setNodePosition(EntPtr e, int n, const QPointF &p)
{
    apf::MeshEntity* ent = (apf::MeshEntity*)e;
    apf::Vector3 pt(p.x(), p.y(),0.);
    apfMesh->setPoint(ent, n, pt);

}

void ApfMesh::sampleEdge(EntPtr e, int r, QList<QPointF>& points)
{
    QList<qreal> xis;
    for(int i=0; i<r+1; i++)
        xis.push_back(2.*i/((qreal)r) - 1.);

    apf::MeshEntity* ent = (apf::MeshEntity*)e;
    apf::MeshElement* me = apf::createMeshElement(apfMesh, ent);

    for(int i=0; i<r+1; i++)
    {
        apf::Vector3 x;
        apf::mapLocalToGlobal(me, apf::Vector3(xis[i],0.,0.), x);
        points[i] = QPointF(x[0], -x[1]);
    }
    apf::destroyMeshElement(me);
}

void ApfMesh::write()
{
    int order = fieldShape->getOrder();
    if (order == 1)
        apf::writeVtkFiles("changed_mesh_linear", apfMesh);
    else
    {
        crv::writeCurvedVtuFiles(apfMesh, apf::Mesh::TRIANGLE, order+2, "changed_mesh_curved");
        crv::writeCurvedWireFrame(apfMesh, 2*order+2, "changed_mesh_curved");

    }
}
