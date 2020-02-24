#ifndef POLYHEDRONTREEVIEW_QT3D_H
#define POLYHEDRONTREEVIEW_QT3D_H

#include <QTreeView>
#include <QStandardItem>
#include <QDebug>
#include <QMouseEvent>
#include "geometry/Tree.h"
#include "modelviewerwidget_qt3d.h"

namespace bstu {

class PolyhedronTreeView_Qt3D : public QTreeView
{
    Q_OBJECT
public:
    PolyhedronTreeView_Qt3D(Tree* tree = nullptr);
    ~PolyhedronTreeView_Qt3D();

    void setTree(Tree* tree);

private:
    void clearModelView();
    static QStandardItemModel* modelPreparation();

    Tree* _tree = nullptr;
    ModelViewerWidget_Qt3D* modelViewer = nullptr;

protected slots:
    void clickProcessing(QStandardItem* item);
};

}
#endif // POLYHEDRONTREEVIEW_QT3D_H
