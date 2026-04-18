#include "utils.h"

typedef struct Component {
    Anchor anchor;
    void (*render)(struct Component *, BoundingBox *);
} Component;