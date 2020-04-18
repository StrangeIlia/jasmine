#include "Centering.hpp"

namespace bstu{
Centering::Centering(AbstractEntityMap* map, Enumerable<Qt3DCore::QTransform*> _set) : QObject(map), set(_set)
{
    this->map = map;
    connect(map, SIGNAL(appended(EntityPair)), SLOT(appended(EntityPair)));
    connect(map, SIGNAL(changed(EntityPair, QEntity*)), SLOT(appended(EntityPair, QEntity*)));
    connect(map, SIGNAL(removed(EntityPair)), SLOT(removed(EntityPair)));
}

void Centering::appended(EntityPair pair) {
    if(pair.key == nullptr) return;
    if(pair.value == nullptr) return;
    Vector oldCenter;
    if(map->count() == 1) {
        initVectors();
    } else {
        oldCenter = (minVector + maxVector) / 2;
    }
    bool changePosition = false;
    Polyhedron* polyhedron = pair.key->data();
    for(int i = 0; i != polyhedron->size(); ++i) {
        Polygon* polygon = polyhedron->polygon(i);
        for(unsigned j = 0; j != polygon->size(); ++j) {
            Vertex vertex = polygon->vertex(j);
            Vertex cvertex = vertex;
            BoolPair pair = minmax(minVector, vertex);
            if(pair) {
                changePosition = true;
                if(pair.x) minPol.x = polyhedron;
                if(pair.y) minPol.y = polyhedron;
                if(pair.z) minPol.z = polyhedron;
            }
            pair = minmax(cvertex, maxVector);
            if(pair) {
                changePosition = true;
                if(pair.x) maxPol.x = polyhedron;
                if(pair.y) maxPol.y = polyhedron;
                if(pair.z) maxPol.z = polyhedron;
            }
        }
    }
    if(changePosition) {
        Vector newCenter = (minVector + maxVector) / 2;
        changePositions(minus(newCenter, oldCenter));
    }
}

void Centering::changed(EntityPair oldPair, QEntity* newEntity) {
    if(newEntity != nullptr)
        appended(oldPair);
}

void Centering::removed(EntityPair pair) {
    if(pair.key == nullptr) return;
    if(pair.value == nullptr) return;
    if(map->count() == 0) return;
    Polyhedron* polyhedron = pair.key->data();
    if(!minPol.has(polyhedron) && !maxPol.has(polyhedron)) return;
    Vector oldCenter = (minVector + maxVector) / 2;
    if(minPol.x == polyhedron) minVector.x = std::numeric_limits<double>::max();
    if(minPol.y == polyhedron) minVector.y = std::numeric_limits<double>::max();
    if(minPol.z == polyhedron) minVector.z = std::numeric_limits<double>::max();
    if(maxPol.x == polyhedron) maxVector.x = std::numeric_limits<double>::min();
    if(maxPol.y == polyhedron) maxVector.y = std::numeric_limits<double>::min();
    if(maxPol.z == polyhedron) maxVector.z = std::numeric_limits<double>::min();
    Enumerator<EntityPair> enumerator = map->getEnumerator();
    while(enumerator->moveNext()) {
        polyhedron = enumerator->current().key->data();
        for(int i = 0; i != polyhedron->size(); ++i) {
            Polygon* polygon = polyhedron->polygon(i);
            for(unsigned j = 0; j != polygon->size(); ++j) {
                Vertex vertex = polygon->vertex(j);
                Vertex cvertex = vertex;
                BoolPair pair = minmax(minVector, vertex);
                if(pair) {
                    if(pair.x) minPol.x = polyhedron;
                    if(pair.y) minPol.y = polyhedron;
                    if(pair.z) minPol.z = polyhedron;
                }
                pair = minmax(cvertex, maxVector);
                if(pair) {
                    if(pair.x) maxPol.x = polyhedron;
                    if(pair.y) maxPol.y = polyhedron;
                    if(pair.z) maxPol.z = polyhedron;
                }
            }
        }
    }
    Vector newCenter = (minVector + maxVector) / 2;
    changePositions(minus(newCenter, oldCenter));
}

void Centering::initVectors() {
    minVector.x = minVector.y = minVector.z = std::numeric_limits<double>::max();
    maxVector.x = maxVector.y = maxVector.z = std::numeric_limits<double>::min();
    minPol.x = minPol.y = minPol.z = nullptr;
    maxPol.x = maxPol.y = maxPol.z = nullptr;
}

void Centering::changePositions(Vector dVector) {
    Enumerator<Qt3DCore::QTransform*> enumerator = set->getEnumerator();
    while(enumerator->moveNext()) {
        Qt3DCore::QTransform* transform = enumerator->current();
        transform->setTranslation(QVector3D(dVector.x, dVector.y, dVector.z));
    }
}

Centering::BoolPair Centering::minmax(Vertex& a, Vertex& b) {
    BoolPair result;
    if(a.x < b.x) {
        result.x = true;
        std::swap(a.x, b.x);
    }
    if(a.y < b.y) {
        result.y = true;
        std::swap(a.y, b.y);
    }
    if(a.z < b.z) {
        result.z = true;
        std::swap(a.z, b.z);
    }
    return result;
}

Vector Centering::minus(const Vector& x, const Vector& y) {
    Vector result = x;
    result.x -= y.x;
    result.y -= y.y;
    result.z -= result.z;
    return result;
}

}

