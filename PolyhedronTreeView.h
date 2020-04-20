#ifndef POLYHEDRONTREEVIEW_H
#define POLYHEDRONTREEVIEW_H

#include "geometry/Tree.h"
#include "collections/AbstractPolyhedronSet.h"

#include <QSet>
#include <QTreeView>
#include <QStandardItem>

namespace bstu {

class PolyhedronTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit PolyhedronTreeView(AbstractPolyhedronSet *set, QWidget *parrent = nullptr, Tree* tree = nullptr);
    ~PolyhedronTreeView();

    void setTree(Tree* tree);
    void clearTree();
signals:
    void polyhedronSelected(PolyhedronExtension*);
    void polyhedronUnselected(PolyhedronExtension*);

private slots:
    void clickProcessing(QStandardItem* item);
    void fillProcessing(const QModelIndex& index);
    void clearProcessing(const QModelIndex& index);
private:
    void appendData(QStandardItem* item, PolyhedronExtension* polyhedron);
    AbstractPolyhedronSet *activePolyhedrons;
};

}


#endif // POLYHEDRONTREEVIEW_H
