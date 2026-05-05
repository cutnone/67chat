#include "./text.h"
#include "../utils.h"
#include "../../../common/datastructures/stringBuilder.h"
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// got this "inheritance" pattern from Claude
typedef struct {
    Component component;
    char *label;
    StringBuilder *value;
    TextComponent *text;
    unsigned int cursor;
    bool active;
} LineEditComponent;

void renderLineEdit(Component *component, BoundingBox *bbox) {
    LineEditComponent *le = (LineEditComponent*) component;
    int spaceForValue = bbox->size.x-strlen(le->label);
    int firstVisibleValueChar;
    int nSpaces = 0;
    if (le->cursor < spaceForValue/2 || (le->value->length <= spaceForValue && le->cursor < spaceForValue)) {
        // cursor near beginning
        firstVisibleValueChar = 0;
        if (spaceForValue > le->value->length) nSpaces = spaceForValue - le->value->length;
        if (le->cursor == le->value->length) nSpaces--;
    } else if (le->value->length - le->cursor < spaceForValue/2) {
        // cursor near end
        firstVisibleValueChar = le->value->length - spaceForValue + 1;
        if (le->cursor < le->value->length) nSpaces = 1;
    } else {
        // cursor somewhere else
        firstVisibleValueChar = le->cursor - (spaceForValue/2);
    }

    TextRenderInstruction colorInstr = {
        .type = TR_FORMAT,
        .attrs = COLOR_PAIR(1),
    };
    TextRenderInstruction cursorColorInstr = {
        .type = TR_FORMAT,
        .attrs = COLOR_PAIR(2),
    };

    alClear(le->text->instructions);

    // add label
    alAppend(le->text->instructions, &colorInstr);
    alConcatAndFree(le->text->instructions, rawStringToInstructions(le->label));
    
    // add value before cursor
    StringBuilder *valueBuilder = newStringBuilder();
    for (; firstVisibleValueChar < le->cursor; firstVisibleValueChar++) {
        sbAppendC(valueBuilder, le->value->data[firstVisibleValueChar]);
    }
    alConcatAndFree(le->text->instructions, rawStringToInstructions(valueBuilder->data));

    
    // add value at cursor
    if (le->active) alAppend(le->text->instructions, &cursorColorInstr);
    char cursorStr[2] = {' ', '\0'};
    TextRenderInstruction cursorCharInstr = {
        .type = TR_WORD,
        .text = cursorStr,
    };
    if (le->cursor < le->value->length) {
        cursorStr[0] = le->value->data[firstVisibleValueChar++];
    }
    if (le->active) alAppend(le->text->instructions, &cursorCharInstr);
    alAppend(le->text->instructions, &colorInstr);
    
    // add value after cursor
    int currentLen = valueBuilder->length + 1;
    sbClear(valueBuilder);
    for (; valueBuilder->length < spaceForValue-currentLen; firstVisibleValueChar++) {
        sbAppendC(valueBuilder, le->value->data[firstVisibleValueChar]);
    }
    alConcatAndFree(le->text->instructions, rawStringToInstructions(valueBuilder->data));

    if (nSpaces > 0) {
        TextRenderInstruction space = {
            .type = TR_SPACE,
        };
        for (int i = 0; i < nSpaces; i++) {
            alAppend(le->text->instructions, &space);
        }
    }

    le->text->component.render((Component *) le->text, bbox);
}

void lineEditReceiveInput(Component *component, int c) {
    LineEditComponent *le = (LineEditComponent*) component;
    switch (c) {
        case '\b':
        case 127: // macos backspace
        case KEY_DC:
        case KEY_BACKSPACE:
            if (le->cursor > 0) {
                sbRemove(le->value, le->cursor-1);
                le->cursor--;
            }
            break;
        case KEY_RIGHT:
            if (le->cursor < le->value->length) le->cursor++;
            break;
        case KEY_LEFT:
            if (le->cursor > 0) le->cursor--;
            break;
        default:
            if (isprint(c)) {
                sbInsertC(le->value, le->cursor, (char) c);
                le->cursor++;
            }
            break;
    }
}

LineEditComponent *newLineEdit() {
    LineEditComponent *le = malloc(sizeof(LineEditComponent));

    le->component.anchor = zeroAnchor;
    le->label = "";
    le->value = newStringBuilder();
    le->text = newTextComponent();
    le->component.render = renderLineEdit;
    le->component.receiveInput = lineEditReceiveInput;
    le->cursor = 0;
    le->active = false;
    return le;
}