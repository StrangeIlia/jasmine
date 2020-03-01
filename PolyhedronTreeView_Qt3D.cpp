#include "PolyhedronTreeView_Qt3D.h"

Q_DECLARE_METATYPE(Qt3DCore::QEntity*)

namespace bstu {

PolyhedronTreeView_Qt3D::PolyhedronTreeView_Qt3D(Tree* tree)
{
    setTree(tree);
}

PolyhedronTreeView_Qt3D::~PolyhedronTreeView_Qt3D()
{
    this->clearModelView();
}

void PolyhedronTreeView_Qt3D::setTree(Tree* tree)
{
    clearModelView();
    QStandardItemModel* model = (QStandardItemModel*)this->model();
    if(model == nullptr){
        model = modelPreparation();
        setModel(model);
    }
    else model->removeRows(0, model->rowCount());
    this->_tree = tree;
    modelViewer = new ModelViewerWidget_Qt3D();
    modelViewer->setTree(tree, false);
    modelViewer->container()->show();
    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(clickProcessing(QStandardItem*)));

    for (unsigned i = 0; i < tree->polyhedronsCount(); i++)
    {
        Polyhedron* P = tree->polyhedron(i);

        QStandardItem *item = new QStandardItem(QString("Многогранник %0").arg(i + 1));
        item->setCheckable(true);
        item->setEditable(false);
        item->setData(QVariant::fromValue(modelViewer->entities()[i]));
        for (int j=0; j < P->size(); j++)
        {
            Polygon* polygon = P->polygon(j);
            QStandardItem *test = new QStandardItem(QString("Грань %0").arg(j + 1));
            test->setCheckable(true);
            item->setChild(j, 0, test);
            item->setChild(j, 1, new QStandardItem(QString("%0").arg(polygon->mes())));
            item->setChild(j, 2, new QStandardItem("(x1, y1, z1), (z2, y2, z2), (x3, y3, z3)"));
        }
        item->setBackground(QColor(250, 180, 80));
        model->setItem(i, 0, item);
        model->setItem(i, 1, new QStandardItem(QString("%0").arg(P->volume())));
    }
}

void PolyhedronTreeView_Qt3D::clearModelView()
{
    if(modelViewer != nullptr)
    {
        modelViewer->deleteLater();
        modelViewer = nullptr;
    }
}

QStandardItemModel* PolyhedronTreeView_Qt3D::modelPreparation()
{
    QStandardItemModel* model = new QStandardItemModel(1, 3);
    model->setHorizontalHeaderItem( 0, new QStandardItem( "№" ) );
    model->setHorizontalHeaderItem( 1, new QStandardItem( "Объём/площадь" ) );
    model->setHorizontalHeaderItem( 2, new QStandardItem( "Координаты" ) );
    return model;
}

void PolyhedronTreeView_Qt3D::clickProcessing(QStandardItem* item)
{
    if(item->isCheckable())
    {
        QEntity* entity = qvariant_cast<QEntity*>(item->data());
        entity->setEnabled(item->checkState() == Qt::CheckState::Checked);
    }
}

}
