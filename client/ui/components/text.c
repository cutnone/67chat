#include "../component.h"
#include "../utils.h"
#include "../../../common/datastructures/arrayList.h"
#include "../../../common/datastructures/stringBuilder.h"
#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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


// this function will not be exported in the header,
// so it can only be directly accessed by this file.
// because of this, this render function will only be
// applied to rectangles. this means, outside of 
// malicious usage, the downcasting in this function 
// is safe.
BoundingBox *renderText(Component *component, BoundingBox *bbox) {
    
    TextComponent *text = (TextComponent*) component;
    ArrayList *lineLengths = newArrayListWithCapacity(sizeof(int), bbox->size.x/2);
    int x = 0;
    int y = 0;
    int newLine = 0; // 0 means off, 1 means on, 2 means turn me back off.
    for (int i = 0; i < text->instructions->length; i++) {
        if (bbox->clip && y > bbox->size.y) break; 
        TextRenderInstruction *instr = alGet(text->instructions, i);
        // printf("BEEP %d\n", instr->type);
        switch (instr->type) {
            case TR_WORD: { // using a block here to get around the 'label before declaration' warning
                int len = strlen(instr->text);
                if (len > bbox->size.x - x) {
                    if (mvinch(y, x - 1) == ' ') x--;
                    alAppend(lineLengths, &x);
                    x = 0;
                    y++;
                    newLine = 1;
                }
                for (int j = 0; j < len; j++) {
                    newLine = 0;
                    mvaddch(y + bbox->topLeft.y, x + bbox->topLeft.x, instr->text[j]);
                    x++;
                    if (x == bbox->size.x) {
                        alAppend(lineLengths, &x);
                        newLine = 1;
                        x = 0;
                        y++;
                    }
                }
                break;
            }
            case TR_SPACE:
                // skip if it's the first space after a new line
                if (!newLine) {
                    mvaddch(y + bbox->topLeft.y, x + bbox->topLeft.x, ' ');
                    x++;
                    if (x == bbox->size.x) {
                        x--;
                        alAppend(lineLengths, &x);
                        newLine = 1;
                        x = 0;
                        y++;
                    }
                }
                break;
            case TR_RETURN:
                if (mvinch(y, x - 1) == ' ') x--;
                alAppend(lineLengths, &x);
                x = 0;
                y++;
                newLine = 1;
                break;
            case TR_FORMAT:
                attrset(instr->attrs);
                break;
        }
        if (newLine == 2) newLine = 0;
        if (newLine == 1) newLine = 2;
    }
    attroff(A_ITALIC | A_BOLD | A_UNDERLINE);
    // bank the last line
    alAppend(lineLengths, &x);
    x = 0;
    y++;
    
    int maxX = 0;
    for (int i = 0; i < y; i++) {
        int lineLen = *((int*) alGet(lineLengths, i));
        if (lineLen > maxX) {
            maxX = lineLen;
        }
    }
    
    
    // align text. I should remove some of the code duplication here.
    // the rendered content is left aligned at this point.
    // to get it to where it needs to be, we read from the screen buffer and
    // shift characters over. twice. There is probably a better way to do this.
    if (text->direction & TRD_TOP_MIDDLE) {
        //              ^ bitwise or allows me to test for
        //                TRD_MIDDLE_MIDDLE and TRD_BOTTOM_MIDDLE too
        for (int i = 0; i < y; i++) {
            int lineLen = *((int*) alGet(lineLengths, i));
            int pad = (bbox->size.x - lineLen) / 2;
            for (int j = lineLen-1; j >= 0; j--) {
                chtype charHere = mvinch(bbox->topLeft.y + i, bbox->topLeft.x + j);
                addch(' ');
                mvaddch(bbox->topLeft.y + i, bbox->topLeft.x + j + pad, charHere);
            }
            bbox->topLeft.x = bbox->topLeft.x + ((bbox->size.x-maxX) / 2);
        }
    } else if (text->direction & TRD_TOP_RIGHT) {
        for (int i = 0; i < y; i++) {
            int lineLen = *((int*) alGet(lineLengths, i));
            int pad = (bbox->size.x - lineLen);
            for (int j = lineLen-1; j >= 0; j--) {
                chtype charHere = mvinch(bbox->topLeft.y + i, bbox->topLeft.x + j);
                addch(' ');
                mvaddch(bbox->topLeft.y + i, bbox->topLeft.x + j + pad, charHere);
            }
        }
        bbox->topLeft.x = bbox->topLeft.x + (bbox->size.x-maxX);
    }
    if (text->direction & TRD_MIDDLE_LEFT) {
        int pad = (bbox->size.y - y) / 2;
        for (int i = y - 1; i >= 0; i--) {
            int lineLen = *((int*) alGet(lineLengths, i));
            for (int j = 0; j < bbox->size.x; j++) {
                chtype charHere = mvinch(bbox->topLeft.y + i, bbox->topLeft.x + j);
                addch(' ');
                mvaddch(bbox->topLeft.y + i + pad, bbox->topLeft.x + j, charHere);
            }
        }
        bbox->topLeft.y = bbox->topLeft.y + ((bbox->size.y-y) /2);
    } else if (text->direction & TRD_BOTTOM_LEFT) {
        int pad = (bbox->size.y - y);
        for (int i = y - 1; i >= 0; i--) {
            int lineLen = *((int*) alGet(lineLengths, i));
            for (int j = 0; j < bbox->size.x; j++) {
                chtype charHere = mvinch(bbox->topLeft.y + i, bbox->topLeft.x + j);
                addch(' ');
                mvaddch(bbox->topLeft.y + i + pad, bbox->topLeft.x + j, charHere);
            }
        }
        bbox->topLeft.y = bbox->topLeft.y + (bbox->size.y-y);
    }
    alClear(lineLengths);
    attrset(A_NORMAL);

    bbox->size.y = y;
    bbox->size.x = maxX;
    return bbox;
}

TextComponent *newTextComponent() {
    TextComponent *text = malloc(sizeof(TextComponent));
    text->instructions = newArrayList(sizeof(TextRenderInstruction));
    text->direction = TRD_TOP_LEFT;
    text->component.anchor = zeroAnchor;
    text->component.render = renderText;
    return text;
}

void clearText(TextComponent *c) {
    alClear(c->instructions);
}

ArrayList *stringToInstructions(char *string) {
    ArrayList *list = newArrayList(sizeof(TextRenderInstruction));
    StringBuilder *wordBuffer = newStringBuilder();
    chtype currentFormat = A_NORMAL;
    bool nextIsEscaped = false;
    bool isFormat = false;
    for (int i = 0; i < strlen(string); i++) {
        char c = string[i];
        if ((c == '\n' || c == ' ') && wordBuffer->length > 0) {
            // flush buffer
            
            TextRenderInstruction instr;
            instr.type = TR_WORD;
            instr.text = strdup(wordBuffer->data);
            sbClear(wordBuffer);
            alAppend(list, &instr);
            
        }
        chtype tempFormat = 0;
        switch (c) {
            case '\n': { // adding scopes here allows me to declare variables closer to their usages.
                nextIsEscaped = false;
                TextRenderInstruction instr;
                instr.type = TR_RETURN;
                alAppend(list, &instr);
                break;
            }
            case ' ': {
                nextIsEscaped = false;
                TextRenderInstruction instr;
                instr.type = TR_SPACE;
                alAppend(list, &instr);
                break;
            }
            case '\\': {
                if (!nextIsEscaped) {
                    nextIsEscaped = true;
                } else {
                    nextIsEscaped = false;
                    sbAppendC(wordBuffer, '\\');
                }
                break;
            }
            case '*':
                isFormat = true;
                tempFormat = A_ITALIC;
                break;
            case '~':
                isFormat = true;
                tempFormat = A_UNDERLINE;
                break;
            case '^':
                isFormat = true;
                tempFormat = A_BOLD;
                break;
            default:
                sbAppendC(wordBuffer, c);
                break;
                
        }
        if (isFormat) {
            if (nextIsEscaped) {
                nextIsEscaped = false;
                sbAppendC(wordBuffer, c);
            } else {
                if (wordBuffer->length > 0) {
                    TextRenderInstruction instr;
                    instr.type = TR_WORD;
                    instr.text = strdup(wordBuffer->data);
                    sbClear(wordBuffer);
                    alAppend(list, &instr);
                }
                currentFormat ^= tempFormat;
                TextRenderInstruction instr;
                instr.type = TR_FORMAT;
                instr.attrs = currentFormat;
                alAppend(list, &instr);
            }
        }
        isFormat = false;
    }
    if (wordBuffer->length > 0) {
        TextRenderInstruction instr;
        instr.type = TR_WORD;
        instr.text = strdup(wordBuffer->data);
        sbClear(wordBuffer);
        alAppend(list, &instr);
    }
    free(wordBuffer);
    return list;
}

ArrayList *rawStringToInstructions(char *string) {
    ArrayList *list = newArrayList(sizeof(TextRenderInstruction));
    StringBuilder *wordBuffer = newStringBuilder();
    chtype currentFormat = A_NORMAL;
    for (int i = 0; i < strlen(string); i++) {
        char c = string[i];
        if ((c == '\n' || c == ' ') && wordBuffer->length > 0) {
            // flush buffer
            
            TextRenderInstruction instr;
            instr.type = TR_WORD;
            instr.text = strdup(wordBuffer->data);
            sbClear(wordBuffer);
            alAppend(list, &instr);
            
        }
        chtype tempFormat = 0;
        switch (c) {
            case '\n': { // adding scopes here allows me to declare variables closer to their usages.
                TextRenderInstruction instr;
                instr.type = TR_RETURN;
                alAppend(list, &instr);
                break;
            }
            case ' ': {
                TextRenderInstruction instr;
                instr.type = TR_SPACE;
                alAppend(list, &instr);
                break;
            }
            default:
                sbAppendC(wordBuffer, c);
                break;
                
        }
    }
    if (wordBuffer->length > 0) {
        TextRenderInstruction instr;
        instr.type = TR_WORD;
        instr.text = strdup(wordBuffer->data);
        sbClear(wordBuffer);
        alAppend(list, &instr);
    }
    free(wordBuffer);
    return list;
}