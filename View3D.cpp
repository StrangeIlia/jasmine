#include "View3D.h"

namespace bstu {


View3D::View3D(QWidget* parent)
{
    registerAspect(new Qt3DInput::QInputAspect);
    defaultFrameGraph()->setClearColor(Qt::black);

    /// Смотрел в исходники Qt
    /// Удалять _rootEntity не нужно, поскольку Qt делает это сам
    _rootEntity = new Qt3DCore::QEntity;
    setRootEntity(_rootEntity);
}

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
