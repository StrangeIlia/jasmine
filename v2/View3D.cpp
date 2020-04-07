#include "View3D.h"

namespace bstu {


View3D::View3D(QWidget* parent)
{
    registerAspect(new Qt3DInput::QInputAspect);
    defaultFrameGraph()->setClearColor(Qt::black);
    _container = QWidget::createWindowContainer(this, parent);
    _rootEntity = new Qt3DCore::QEntity;
    setRootEntity(_rootEntity);
    /// Нашел настройку, для выбора элементов по пересечению с треугольниками
//    QRenderSettings* render_settings = renderSettings();
//    QPickingSettings* picking_settings = render_settings->pickingSettings();
//    picking_settings->setPickMethod(QPickingSettings::PrimitivePicking);

    /// Настройка политики сортировки (работает так себе)
//    QFrameGraphNode *framegraph = activeFrameGraph();
//    QSortPolicy* sort_policy = new QSortPolicy(_rootEntity);
//    framegraph->setParent(sort_policy);
//    sort_policy->setSortTypes(QVector<QSortPolicy::SortType>()
//                              << QSortPolicy::SortType::BackToFront
//                              << QSortPolicy::SortType::FrontToBack
//                              << QSortPolicy::StateChangeCost
//                              << QSortPolicy::Material
//    );
//    setActiveFrameGraph(sort_policy);
}

View3D::~View3D()
{
    _container->deleteLater();
    _rootEntity->deleteLater();
    if(_cameraController != nullptr)
        _cameraController->deleteLater();
}

QWidget* View3D::container() { return _container; }

void View3D::addEntity(QEntity* entity)
{
    entity->setParent(this->_rootEntity);
}

void View3D::removeEntity(QEntity* entity)
{
    // без преобразования к QNode* qt выбирал приватный setParent,
    // а потом критиковал меня, за попытку доступа к приватным членам
    entity->setParent((QNode*)nullptr);
}

void View3D::addComponent(QComponent *comp)
{
    _rootEntity->addComponent(comp);
}

void View3D::removeComponent(QComponent *comp)
{
    _rootEntity->removeComponent(comp);
}

void View3D::setCameraController(ICameraControllerFactory* factory)
{
    if(_cameraController != nullptr)
        _cameraController->deleteLater();
    _cameraController = factory->createCameraController(camera());
    _cameraController->setParent(_rootEntity);
}

QComponentVector View3D::components() const
{
    return _rootEntity->components();
}

template<class T> QVector<T*> View3D::componentsOfType() const
{
    return _rootEntity->componentsOfType<T>();
}

}
