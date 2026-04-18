#include "../component.h"

void render(Component *s, BoundingBox *bb) {

}

Component *newConnectScene() {
    Component s;
    s.render = &render;
    return &s;
}