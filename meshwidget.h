#ifndef MESHWIDGET_H
#define MESHWIDGET_H

#include <QGraphicsView>

#include <apf.h>
#include <apfMesh2.h>
#include <apfNumbering.h>

#include <crv.h>

class Node;

class MeshWidget : public QGraphicsView
{
    Q_OBJECT
public:
    MeshWidget(apf::Mesh2* mesh, QWidget *parent = nullptr);

    void itemMoved();

public slots:
    // void shuffle();
    void zoomIn();
    void zoomOut();
    apf::Mesh2* getMesh() {return m_mesh;}

protected:
    void keyPressEvent(QKeyEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void scaleView(qreal scaleFactor);

private:

    apf::Mesh2* m_mesh;
    Node *centerNode;
};

#endif // MESHWIDGET_H
