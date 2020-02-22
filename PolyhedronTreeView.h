#ifndef POLYHEDRONTREEVIEW_H
#define POLYHEDRONTREEVIEW_H
#include <QTreeView>
#include <QDebug>
#include <QMouseEvent>
#include "geometry/Tree.h"

namespace bstu {

class PolyhedronTreeView : public QTreeView
{
    Tree* tree;
public:

    void setTree(Tree* tree);

    PolyhedronTreeView();

    virtual void mousePressEvent(QMouseEvent *event)
    {
        QModelIndex index = indexAt(event->pos());
        QString text = index.data(Qt::DisplayRole).toString();
        qDebug() << index.row()  << " " << index.column() << " " << text;
        QTreeView::mousePressEvent(event);
            /*
            bool selected = selectionModel()->isSelected(indexAt(event->pos()));
            QTreeView::mousePressEvent(event);
            if ((item.row() == -1 && item.column() == -1) || selected)
            {
                clearSelection();
                const QModelIndex index;
                selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
            }*/
    }
};


} // namespace bstu

#endif // POLYHEDRONTREEVIEW_H


