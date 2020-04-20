#include "PolyhedronExtension.h"

namespace bstu {

PolyhedronExtension::PolyhedronExtension(Polyhedron* polyhedron, Polyhedron* parrent) {
    _data = polyhedron;
    if(parrent != nullptr) {
        _parrent = new PolyhedronExtension(parrent);
    }
    if(_data->child1() != nullptr) {
        _child1 = new PolyhedronExtension(_data->child1(), this);
    }
    if(_data->child2() != nullptr) {
        _child2 = new PolyhedronExtension(_data->child2(), this);
    }
}

PolyhedronExtension::PolyhedronExtension(Polyhedron* polyhedron, PolyhedronExtension* parrent) {
    _data = polyhedron;
    _parrent = parrent;
    if(_data->child1() != nullptr) {
        _child1 = new PolyhedronExtension(_data->child1(), this);
    }
    if(_data->child2() != nullptr) {
        _child2 = new PolyhedronExtension(_data->child2(), this);
    }
}

PolyhedronExtension::~PolyhedronExtension() {
    if(_child1 != nullptr) {
        delete _child1;
    }
    if(_child2 != nullptr) {
        delete _child2;
    }
    if(_parrent != nullptr) {
        if(_parrent->_child1 == this) {
            _parrent->_child1 = nullptr;
        }
        if(_parrent->_child2 == this) {
            _parrent->_child2 = nullptr;
        }
    }
}

Polyhedron* PolyhedronExtension::data() {
    return _data;
}

PolyhedronExtension* PolyhedronExtension::parrent() {
    return _parrent;
}

PolyhedronExtension* PolyhedronExtension::child1() const {
    return _child1;
}

PolyhedronExtension* PolyhedronExtension::child2() const {
    return _child2;
}

bool PolyhedronExtension::isDescendant(PolyhedronExtension* descendant) {
    return descendant->isPredecessor(this);
}

bool PolyhedronExtension::isPredecessor(PolyhedronExtension* predecessor) {
    PolyhedronExtension* iter = this->parrent();
    while(iter != nullptr) {
        if(iter == predecessor) {
            return true;
        }
    }
    return false;
}

}
