#pragma once
#include "./text.h"
#include "../utils.h"
#include "../../../common/datastructures/stringBuilder.h"
#include <stdbool.h>

// got this "inheritance" pattern from Claude
typedef struct {
    Component component;
    char *label;
    StringBuilder *value;
    TextComponent *text;
    unsigned int cursor;
    bool active;
} LineEditComponent;

void renderLineEdit(Component *component, BoundingBox *bbox);
void lineEditReceiveInput(Component *component, int c);

LineEditComponent *newLineEdit();