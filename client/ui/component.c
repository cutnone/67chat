#include "utils.h"

typedef struct Component {
    Anchor anchor;
    BoundingBox *(*render)(struct Component *, BoundingBox *);
    void (*receiveInput)(struct Component *, int key);
} Component;