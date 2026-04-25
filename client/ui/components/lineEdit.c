#include "./text.h"
#include "../utils.h"
#include "../../../common/datastructures/stringBuilder.h"
#include <string.h>
#include <curses.h>

// got this "inheritance" pattern from Claude
typedef struct {
    Component component;
    char *label;
    StringBuilder *value;
    TextComponent text;
} LineEditComponent;

void renderLineEdit(Component *component, BoundingBox *bbox) {
    LineEditComponent *le = (LineEditComponent*) component;
    int textSpace = bbox->size.x-strlen(le->label);
    char str[bbox->size.x];
    strcpy(str, le->label);
    char *firstVisibleValueChar = le->value->data;
    if (le->value->length > textSpace) {
        firstVisibleValueChar += le->value->length-textSpace;
    }
    strcat(str, firstVisibleValueChar);
    le->text.instructions = *stringToInstructions(str);
    le->text.component.render(&le->text, bbox);
}

void lineEditReceiveInput(Component *component, chtype c) {
    LineEditComponent *le = (LineEditComponent*) component;
    switch (c) {
        case KEY_BACKSPACE:
            sbClip(&le->value, 1);
            break;
        default:
            sbAppendC(&le->value, c);
            break;
    }
}