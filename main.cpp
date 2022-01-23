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


#include <QApplication>
#include <QTime>
#include <QMainWindow>

#include <PCU.h>
#include <lionPrint.h>
#include <apf.h>
#include <apfMesh2.h>
#include <apfMDS.h>
#include <crv.h>

// #include <gmi.h>
// #include <gmi_mesh.h>
// #include <gmi_null.h>


#include "meshwidget.h"

int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    PCU_Comm_Init();
    lion_set_verbosity(1);
    QApplication app(argc, argv);


    apf::Mesh2* mesh = apf::makeEmptyMdsMesh(0, 1, false);

    double vert_coords[2][6] = {
        {0.,0.,0., 0., 0., 0.},
        {100.,0.,0., 0., 0., 0.}
    };

    int edge_info[1][2] = {
        {0,1}
    };

    apf::MeshEntity* verts[2];
    apf::MeshEntity* edges[1];

    for (int i = 0; i < 2; i++) {
      apf::Vector3 coords(vert_coords[i][0],
                          vert_coords[i][1],
                          vert_coords[i][2]);
      apf::Vector3 params(vert_coords[i][3],
                          vert_coords[i][4],
                          vert_coords[i][5]);
      verts[i] = mesh->createVertex(0, coords, params);
    }
    for (int i = 0; i < 1; i++) {
      apf::MeshEntity* down_vs[2] = {verts[edge_info[i][0]],
                                     verts[edge_info[i][1]]};
      edges[i] = mesh->createEntity(apf::Mesh::EDGE, 0, down_vs);
    }

    mesh->acceptChanges();

    apf::changeMeshShape(mesh, crv::getBezier(3),true);
    // apf::FieldShape* fs = mesh->getShape();

    mesh->setPoint(edges[0], 0, apf::Vector3(33.,  20., 0.));
    mesh->setPoint(edges[0], 1, apf::Vector3(67., -20., 0.));

    // mesh->acceptChanges();
    //  apf::writeVtkFiles("test_mesh", mesh);
    crv::writeCurvedWireFrame(mesh, 10, "test_mesh_wire");

    // mesh->destroyNative();
    // apf::destroyMesh(mesh);


    MeshWidget *widget = new MeshWidget(mesh);

    QMainWindow mainWindow;
    mainWindow.setCentralWidget(widget);

    mainWindow.show();


    return app.exec();
    PCU_Comm_Free();
    MPI_Finalize();
}
