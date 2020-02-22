#include "polyhedrontreeview.h"
#include <QStandardItemModel>

Q_DECLARE_METATYPE(bstu::Polyhedron*)
Q_DECLARE_METATYPE(bstu::VolumePolygon*)

namespace bstu {

PolyhedronTreeView::PolyhedronTreeView()
{
    setWindowTitle("Дерево многогранников");
}

void PolyhedronTreeView::setTree(Tree* _tree)
{    
    tree = _tree;

    QStandardItemModel* model = new QStandardItemModel ( 1, 3 );

    for (int i=0; i < tree->polyhedronsCount();i++)
    {
        Polyhedron* P = tree->polyhedron(i);

        QStandardItem *item = new QStandardItem(QString("Многогранник %0").arg(i + 1));

        item->setData(QVariant::fromValue(P));          // Сохраним указатель на многогранник как пользовательские данные внутрь дерева
        P = qvariant_cast<Polyhedron*>(item->data());   // Возьмём обратно (для проверки)

        item->setEditable(false);
        for (int j=0; j < P->size(); j++)
        {
            Polygon* polygon = P->polygon(j);
            item->setChild(j, 0, new QStandardItem(QString("Грань %0").arg(j + 1)));
            item->setChild(j, 1, new QStandardItem(QString("%0").arg(polygon->mes())));
            item->setChild(j, 2, new QStandardItem("(x1, y1, z1), (z2, y2, z2), (x3, y3, z3)"));
        }
        item->setBackground(QColor(250, 180, 80));
        model->setItem(i, 0, item);
        model->setItem(i, 1, new QStandardItem(QString("%0").arg(P->volume())));
    }

   /* for( int r=0; r<5; r++ )
      for( int c=0; c<2; c++)
      {
        QStandardItem *item = new QStandardItem( QString("Row:%0, Column:%1").arg(r).arg(c) );

        if( c == 0 )
          for( int i=0; i<3; i++ )
          {
            QStandardItem *child = new QStandardItem( QString("Item %0").arg(i) );
            //child->setData();
            child->setEditable( false );
            child->setChild(0, new QStandardItem("Объём "));
            child->setChild(1, new QStandardItem("Вершины"));
            child->setChild(1, 1, new QStandardItem("x"));
            child->setChild(2, new QStandardItem("Лист2"));
            item->appendRow( child );
          }

        model->setItem(r, c, item);
      }

*/

    model->setHorizontalHeaderItem( 0, new QStandardItem( "№" ) );
    model->setHorizontalHeaderItem( 1, new QStandardItem( "Объём/площадь" ) );
    model->setHorizontalHeaderItem( 2, new QStandardItem( "Координаты" ) );

    setModel( model );
}

} // namespace bstu
