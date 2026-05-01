#pragma once
#include "../component.h"
#include "../utils.h"
#include "../../../common/datastructures/arrayList.h"
#include <curses.h>

typedef enum {
    // using a bitfield here.
    // the left two bits represent vertical alignment,
    // and the right two bits represent horizontal.
    TRD_TOP_LEFT = 0b0000,
    TRD_TOP_MIDDLE = 0b0001,
    TRD_TOP_RIGHT = 0b0010,
    TRD_MIDDLE_LEFT = 0b0100,
    TRD_MIDDLE_MIDDLE = 0b0101,
    TRD_MIDDLE_RIGHT = 0b0110,
    TRD_BOTTOM_LEFT = 0b1000,
    TRD_BOTTOM_MIDDLE = 0b1001,
    TRD_BOTTOM_RIGHT = 0b1010,
} TextRenderDirection;
typedef enum {
    TR_WORD,
    TR_SPACE,
    TR_RETURN,
    TR_FORMAT,
} TextRenderInstructionType;

typedef struct {
    TextRenderInstructionType type;
    union {
        char *text;
        chtype attrs;
    };
} TextRenderInstruction;

// got this "inheritance" pattern from Claude
typedef struct {
    Component component;
    TextRenderDirection direction;
    ArrayList *instructions;
} TextComponent;

TextComponent *newTextComponent();
void clearText(TextComponent *c);
ArrayList *stringToInstructions(char *string);
ArrayList *rawStringToInstructions(char *string);