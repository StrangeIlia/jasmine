#ifndef SIMPLEMATERIALFACTORY_H
#define SIMPLEMATERIALFACTORY_H

#include "utils/factories/AbstractMaterialFactory.hpp"

namespace bstu {
class SimpleMaterialFactory : public AbstractMaterialFactory
{
public:
    SimpleMaterialFactory(QNode* root);
    QMaterial* create(Polyhedron* polyhedron) override;
private:
    QMaterial* material;
};
}


#endif // SIMPLEMATERIALFACTORY_H
