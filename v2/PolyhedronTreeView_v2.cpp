#include "PolyhedronTreeView_v2.h"

Q_DECLARE_METATYPE(bstu::Polyhedron*)

namespace bstu {

PolyhedronTreeView_v2::PolyhedronTreeView_v2(Tree* tree)
{
    QStandardItemModel* model = new QStandardItemModel(1, 3);
    model->setHorizontalHeaderItem( 0, new QStandardItem( "№" ) );
    model->setHorizontalHeaderItem( 1, new QStandardItem( "Объём/площадь" ) );
    model->setHorizontalHeaderItem( 2, new QStandardItem( "Координаты" ) );
    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(clickProcessing(QStandardItem*)));
    connect(this, SIGNAL(expanded(const QModelIndex&)), this, SLOT(fillProcessing(const QModelIndex&)));
    connect(this, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(clearProcessing(const QModelIndex&)));
    setModel(model);
    this->setTree(tree);
}

const Tree* PolyhedronTreeView_v2::tree() const
{
    return _tree;
}

void PolyhedronTreeView_v2::setTree(Tree* tree)
{
    QStandardItemModel* model = (QStandardItemModel*)this->model();
    if(_tree != nullptr)
        model->removeRows(0, model->rowCount());
    _tree = tree;

    for (unsigned i = 0; i < tree->polyhedronsCount(); i++)
    {
        Polyhedron* polyhedron = tree->polyhedron(i);
        QStandardItem *item = new QStandardItem(QString("Многогранник %0").arg(i + 1));
        appendData(item, polyhedron);
        model->setItem(i, 0, item);
        model->setItem(i, 1, new QStandardItem(QString("%0").arg(polyhedron->volume())));
    }
}

void PolyhedronTreeView_v2::clickProcessing(QStandardItem* item)
{
    if(item->isCheckable())
    {
        Polyhedron* polyhedron = qvariant_cast<Polyhedron*>(item->data());
        if(item->checkState() == Qt::CheckState::Checked)
        {
            _activePolyhedrons += polyhedron;
            emit polyhedronSelected(polyhedron);
        }
        else
        {
            _activePolyhedrons -= polyhedron;
            emit polyhedronUnselected(polyhedron);
        }
    }
}

void PolyhedronTreeView_v2::fillProcessing(const QModelIndex& index)
{
    QStandardItemModel* model = (QStandardItemModel*)this->model();
    QStandardItem* item = model->itemFromIndex(index);
    item->removeRows(0, item->row()); // Удаляем фиктивный элемент
    Polyhedron* polyhedron = qvariant_cast<Polyhedron*>(item->data());
    int shift = 0;
    if(polyhedron->isHalve())
    {
        shift = 2;
        Polyhedron* child1 = polyhedron->child1();
        Polyhedron* child2 = polyhedron->child2();
        QStandardItem* item1 = new QStandardItem(QString("Многогранник 1"));
        QStandardItem* item2 = new QStandardItem(QString("Многогранник 2"));
        appendData(item1, child1);
        appendData(item2, child2);
        item->setChild(0, 0, item1);
        item->setChild(0, 1, new QStandardItem(QString("%0").arg(child1->volume())));
        item->setChild(1, 0, item2);
        item->setChild(1, 1, new QStandardItem(QString("%0").arg(child2->volume())));
    }

    for (int j = 0; j < polyhedron->size(); ++j)
    {
        Polygon* polygon = polyhedron->polygon(j);
        item->setChild(j + shift, 0, new QStandardItem(QString("Грань %0").arg(j + 1)));
        item->setChild(j + shift, 1, new QStandardItem(QString("%0").arg(polygon->mes())));
        item->setChild(j + shift, 2, new QStandardItem("(x1, y1, z1), (z2, y2, z2), (x3, y3, z3)"));
    }
}

void PolyhedronTreeView_v2::clearProcessing(const QModelIndex& index)
{
    QStandardItemModel* model = (QStandardItemModel*)this->model();
    QStandardItem* item = model->itemFromIndex(index);
    item->removeRows(0, item->row());
    item->setChild(0, new QStandardItem()); // Добавляем фиктивный элемент
}

void PolyhedronTreeView_v2::appendData(QStandardItem* item, Polyhedron* polyhedron)
{
    item->setCheckable(true);
    item->setEditable(false);
    item->setData(QVariant::fromValue(polyhedron));
    if(_activePolyhedrons.contains(polyhedron))
        item->setCheckState(Qt::CheckState::Checked);
    item->setBackground(QColor(250, 180, 80));
    item->setChild(0, new QStandardItem()); // Добавляем фиктивный элемент
}

}

