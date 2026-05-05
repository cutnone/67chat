#pragma once
#include "utils.h"

// Claude taught me this self-referential typedef workaround
typedef struct Component {
    Anchor anchor;
    BoundingBox *(*render)(struct Component *, BoundingBox *);
    void (*receiveInput)(struct Component *, int key);
} Component;