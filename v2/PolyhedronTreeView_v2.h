#ifndef POLYHEDRONTREEVIEW_H
#define POLYHEDRONTREEVIEW_H

#include "../geometry/Tree.h"

#include <QSet>
#include <QTreeView>
#include <QStandardItem>

namespace bstu {

class PolyhedronTreeView_v2 : public QTreeView
{
    Q_OBJECT
public:
    explicit PolyhedronTreeView_v2(Tree* tree = nullptr);

    const Tree* tree() const;
    void setTree(Tree* tree);

signals:
    void polyhedronSelected(Polyhedron*);
    void polyhedronUnselected(Polyhedron*);

private slots:
    void clickProcessing(QStandardItem* item);
    void fillProcessing(const QModelIndex& index);
    void clearProcessing(const QModelIndex& index);
private:
    void appendData(QStandardItem* item, Polyhedron* polyhedron);
    Tree* _tree = nullptr;
    QSet<Polyhedron*> _activePolyhedrons;
};

}


#endif // POLYHEDRONTREEVIEW_H
