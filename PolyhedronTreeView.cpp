#include "PolyhedronTreeView.h"

Q_DECLARE_METATYPE(bstu::PolyhedronExtension*)

namespace bstu {

PolyhedronTreeView::PolyhedronTreeView(AbstractPolyhedronSet *set, QWidget *parrent, Tree* tree) : QTreeView(parrent)
{
    if(set == nullptr) {
        throw std::exception();
    }
    activePolyhedrons = set;

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

PolyhedronTreeView::~PolyhedronTreeView() {
    clearTree();
}

void PolyhedronTreeView::setTree(Tree* tree)
{
    clearTree();
    if(tree == nullptr) return;
    QStandardItemModel* model = (QStandardItemModel*)this->model();
    for (unsigned i = 0; i < tree->polyhedronsCount(); i++)
    {
        Polyhedron* polyhedron = tree->polyhedron(i);
        QStandardItem *item = new QStandardItem(QString("Многогранник %0").arg(i + 1));
        appendData(item, new PolyhedronExtension(polyhedron));
        model->setItem(i, 0, item);
        model->setItem(i, 1, new QStandardItem(QString("%0").arg(polyhedron->volume())));
    }
}

void PolyhedronTreeView::clearTree() {
    QStandardItemModel* model = (QStandardItemModel*)this->model();
    if(model == nullptr) return;
    for(int i = 0; i != model->rowCount(); ++i) {
        QStandardItem *item = model->item(i);
        if(item != nullptr && item->data().data() != nullptr) {
            PolyhedronExtension* polyhedron = qvariant_cast<PolyhedronExtension*>(item->data());
            delete polyhedron;
        }
    }
    model->removeRows(0, model->rowCount());
}

void PolyhedronTreeView::clickProcessing(QStandardItem* item)
{
    if(item->isCheckable())
    {
        PolyhedronExtension* polyhedron = qvariant_cast<PolyhedronExtension*>(item->data());
        if(item->checkState() == Qt::CheckState::Checked)
        {
            activePolyhedrons->append(polyhedron);
            emit polyhedronSelected(polyhedron);
        }
        else
        {
            activePolyhedrons->remove(polyhedron);
            emit polyhedronUnselected(polyhedron);
        }
    }
}

void PolyhedronTreeView::fillProcessing(const QModelIndex& index)
{
    QStandardItemModel* model = (QStandardItemModel*)this->model();
    QStandardItem* item = model->itemFromIndex(index);
    item->removeRows(0, item->row()); // Удаляем фиктивный элемент
    PolyhedronExtension* extension = qvariant_cast<PolyhedronExtension*>(item->data());
    Polyhedron* polyhedron = extension->data();
    int shift = 0;
    if(polyhedron->isHalve())
    {
        shift = 2;
        PolyhedronExtension* child1 = extension->child1();
        PolyhedronExtension* child2 = extension->child2();
        QStandardItem* item1 = new QStandardItem(QString("Многогранник 1"));
        QStandardItem* item2 = new QStandardItem(QString("Многогранник 2"));
        appendData(item1, child1);
        appendData(item2, child2);
        item->setChild(0, 0, item1);
        item->setChild(0, 1, new QStandardItem(QString("%0").arg(child1->data()->volume())));
        item->setChild(1, 0, item2);
        item->setChild(1, 1, new QStandardItem(QString("%0").arg(child2->data()->volume())));
    }

    for (int j = 0; j < polyhedron->size(); ++j)
    {
        Polygon* polygon = polyhedron->polygon(j);
        item->setChild(j + shift, 0, new QStandardItem(QString("Грань %0").arg(j + 1)));
        item->setChild(j + shift, 1, new QStandardItem(QString("%0").arg(polygon->mes())));
        item->setChild(j + shift, 2, new QStandardItem("(x1, y1, z1), (z2, y2, z2), (x3, y3, z3)"));
    }
}

void PolyhedronTreeView::clearProcessing(const QModelIndex& index)
{
    QStandardItemModel* model = (QStandardItemModel*)this->model();
    QStandardItem* item = model->itemFromIndex(index);
    item->removeRows(0, item->row());
    item->setChild(0, new QStandardItem()); // Добавляем фиктивный элемент
}

void PolyhedronTreeView::appendData(QStandardItem* item, PolyhedronExtension* polyhedron)
{
    item->setCheckable(true);
    item->setEditable(false);
    item->setData(QVariant::fromValue(polyhedron));
    if(activePolyhedrons->has(polyhedron))
        item->setCheckState(Qt::CheckState::Checked);
    item->setBackground(QColor(250, 180, 80));
    item->setChild(0, new QStandardItem()); // Добавляем фиктивный элемент
}

}

