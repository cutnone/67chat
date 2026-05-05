#include "../components/group.h"
#include "../components/text.h"
#include "../component.h"
#include "../../comms.h"
#include <stdio.h>
#include <stddef.h>

const int MIN_WIDTH = 50;
const int MIN_HEIGHT = 20;

TextComponent *resizeText;
Group *resizeScene;
void (*groupRenderer)(Component *, BoundingBox *);

void renderHook(Component *component, BoundingBox *bbox) {
    int maxY = getmaxy(curscr), maxX = getmaxx(curscr);
    TextRenderInstruction instr = {
        .type = TR_WORD,
        .text = "bigger."
    };
    if (maxX < MIN_WIDTH && maxY >= MIN_HEIGHT) {
        instr.text = "wider.";
    } else if (maxY < MIN_HEIGHT && maxX >= MIN_WIDTH) {
        instr.text = "taller.";
    }
    alReplace(resizeText->instructions, resizeText->instructions->length-1, &instr);
    groupRenderer(component, bbox);
}

void initializeResizeScene() {
    resizeScene = newGroup();
    resizeScene->component.anchor.size.xType = VEC_RELATIVE;
    resizeScene->component.anchor.size.yType = VEC_RELATIVE;
    resizeScene->component.anchor.size.relX = 1.0;
    resizeScene->component.anchor.size.relY = 1.0;

    groupRenderer = resizeScene->component.render;
    resizeScene->component.render = renderHook;
    resizeText = newTextComponent();
    resizeText->direction = TRD_TOP_MIDDLE;
    resizeText->component.anchor.size.absY = 1;
    resizeText->component.anchor.size.xType = VEC_RELATIVE;
    resizeText->component.anchor.size.relX = 1.0;
    resizeText->component.anchor.position.yType = VEC_RELATIVE;
    resizeText->component.anchor.position.relY = 0.5;
    alConcatAndFree(resizeText->instructions, stringToInstructions("Please make your terminal bigger."));

    alAppend(resizeScene->components, &resizeText);
}