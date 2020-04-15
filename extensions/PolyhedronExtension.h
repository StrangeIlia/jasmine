#ifndef POLYHEDRONEXTENSION_H
#define POLYHEDRONEXTENSION_H

#include "geometry/Polyhedron.h"

namespace bstu {
class PolyhedronExtension
{
    Polyhedron *_data;
    PolyhedronExtension *_parrent = nullptr;
    PolyhedronExtension *_child1 = nullptr;
    PolyhedronExtension *_child2 = nullptr;
public:
    explicit PolyhedronExtension(Polyhedron* polyhedron, Polyhedron* parrent = nullptr);
    explicit PolyhedronExtension(Polyhedron* polyhedron, PolyhedronExtension* parrent);
    ~PolyhedronExtension();

    Polyhedron* data();

    PolyhedronExtension* parrent();
    PolyhedronExtension* child1() const;
    PolyhedronExtension* child2() const;

    bool isDescendant(PolyhedronExtension* descendant); /// потомок
    bool isPredecessor(PolyhedronExtension* predecessor); /// предок
};
}
#endif // POLYHEDRONEXTENSION_H
