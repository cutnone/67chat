#include "utils.h"

typedef struct Component {
    Anchor anchor;
    void (*render)(struct Component *, BoundingBox *);
    void (*receiveInput)(struct Component *, int key);
} Component;