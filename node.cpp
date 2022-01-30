/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "edge.h"
#include "node.h"
#include "meshwrapper.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>



Node::Node(MeshWrapper* m,
           EntPtr e, // pointer to the mesh entity this node is a part of
           int n,    // this nodes numbert wrt e
           int d)    // the dimentions of e
    : mesh(m), ent(e), node(n), dim(d)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);

    QPointF p = mesh->getNodePosition(ent, node);
    setPos(p.x(), -p.y());
    // setSize(1.);
}

void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

QList<Edge *> Node::edges() const
{
    return edgeList;
}

QRectF Node::boundingRect() const
{
    qreal adjust = 1;
    return QRectF( -pointSize - adjust, -pointSize - adjust, 2*pointSize + adjust, 2*pointSize + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-pointSize, -pointSize, 2*pointSize, 2*pointSize);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // Shadow
    ///////////////////////////////////////////////
    // painter->setPen(Qt::NoPen);
    // painter->setBrush(Qt::darkGray);
    // painter->drawEllipse(-7, -7, 20, 20);

    // Gradient
    ///////////////////////////////////////////////
    // QRadialGradient gradient(-3, -3, 10);
    // if (option->state & QStyle::State_Sunken) {
        // gradient.setCenter(3, 3);
        // gradient.setFocalPoint(3, 3);
        // switch (dim) {
        // case 0:
            // gradient.setColorAt(1, QColor(Qt::yellow).lighter(120));
            // gradient.setColorAt(0, QColor(Qt::darkYellow).lighter(120));
            // break;
        // case 1:
            // gradient.setColorAt(1, QColor(Qt::green).lighter(120));
            // gradient.setColorAt(0, QColor(Qt::darkGreen).lighter(120));
            // break;
        // case 2:
            // gradient.setColorAt(1, QColor(Qt::red).lighter(120));
            // gradient.setColorAt(0, QColor(Qt::darkRed).lighter(120));
            // break;
        // default:
            // break;
        // }
    // } else {
        // switch (dim) {
        // case 0:
            // gradient.setColorAt(0, Qt::yellow);
            // gradient.setColorAt(1, Qt::darkYellow);
            // break;
        // case 1:
            // gradient.setColorAt(0, Qt::green);
            // gradient.setColorAt(1, Qt::darkGreen);
            // break;
        // case 2:
            // gradient.setColorAt(0, Qt::red);
            // gradient.setColorAt(1, Qt::darkRed);
            // break;
        // default:
            // break;
        // }
    // }
    // painter->setBrush(gradient);

    switch (dim) {
    case 0:
        painter->setBrush(Qt::yellow);
        break;
    case 1:
        painter->setBrush(Qt::green);
        break;
    case 2:
        painter->setBrush(Qt::red);
        break;
    default:
        break;
    }


    painter->setPen(QPen(Qt::black, 0));
    // painter->drawEllipse(-10, -10, 20, 20);
    painter->drawEllipse(-pointSize, -pointSize, 2*pointSize, 2*pointSize);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        for (Edge *edge : qAsConst(edgeList))
            edge->adjust();
        //mesh_widget->itemMoved();
        updateMeshNode(value.toPointF()); // pass a qfpoint made out of value
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void Node::updateMeshNode(const QPointF& pt)
{
    QPointF p(pt.x(), -pt.y());
    mesh->setNodePosition(ent, node, p);
}
