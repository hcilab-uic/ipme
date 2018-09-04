#ifndef IPME_WB_SCENE_VISUALIZATION_H
#define IPME_WB_SCENE_VISUALIZATION_H

namespace ipme {
namespace wb {
class Scene_visualization {
public:
    virtual void show_frame(int frame_index) = 0;
};
} // namespace wb
} // namespace ipme

#endif // IPME_WB_SCENE_VISUALIZATION_H
