#include "../component.h"
#include "../components/group.h"
#include "../components/text.h"
#include "../components/lineEdit.h"
#include "../../comms.h"
#include "../../interface.h"
#include "../../../common/datastructures/stringBuilder.h"
#include "optionsScene.h"
#include <curses.h>
#include <stdbool.h>
#include <stdio.h>

Group *chatScene;
Group *chatHeader;
Group *chatFooter;
TextComponent *chatUsernameText;
TextComponent *chatChannelText;
TextComponent *chatOptionsText;
LineEditComponent *chatMessageBox;
TextRenderInstruction headerColorInstruction = {
    .type = TR_FORMAT,
    .attrs = COLOR_PAIR(3),
};

void chatSceneReceiveInput(Component *component, int c) {
    printf("PRESSSED %c %d", c, c);
    switch (c) {
        case 27: // escape
            screenComponent = (Component *) optionsScene;
            break; 
        case '\r':
        case '\n':
            sbClear(chatMessageBox->value);
            break;
        default:
            chatMessageBox->component.receiveInput((Component *) chatMessageBox, c);
            break;
    }
}

void updateChatHeader() {
    printf("%s %s \n", username, activeChannel);
    alClear(chatUsernameText->instructions);
    alAppend(chatUsernameText->instructions, &headerColorInstruction);
    alConcatAndFree(chatUsernameText->instructions, stringToInstructions(username));
    alClear(chatChannelText->instructions);
    alAppend(chatChannelText->instructions, &headerColorInstruction);
    alConcatAndFree(chatChannelText->instructions, stringToInstructions(activeChannel));
}

void initializeChatScene() {
    

    chatHeader = newGroup();
    chatHeader->component.anchor.size.xType = VEC_RELATIVE;
    chatHeader->component.anchor.size.relX = 1.0;
    chatHeader->component.anchor.size.absY = 3;
    chatHeader->constraint = GROUP_HBOX;
    chatHeader->xPad = 1;
    chatHeader->leftMargin = 1;
    chatHeader->rightMargin = 1;
    chatHeader->topMargin = 1;
    groupAddBackground(chatHeader, 3);

    chatOptionsText = newTextComponent();
    char *chatOptionsString = "ESC For Options";
    alAppend(chatOptionsText->instructions, &headerColorInstruction);
    alConcatAndFree(chatOptionsText->instructions, stringToInstructions(chatOptionsString));
    chatOptionsText->component.anchor.size.absX = MAX_USERNAME_LENGTH;

    chatUsernameText = newTextComponent();
    chatUsernameText->component.anchor.size.absX = MAX_USERNAME_LENGTH;
    chatUsernameText->direction = TRD_TOP_RIGHT;
    chatUsernameText->component.anchor.origin.relX = 1.0;
    chatUsernameText->component.anchor.position.xType = VEC_RELATIVE;
    chatUsernameText->component.anchor.position.relX = 1.0;

    chatChannelText = newTextComponent();
    chatChannelText->component.anchor.size.xType = VEC_RELATIVE;
    chatChannelText->component.anchor.size.relX = 1.0;
    chatChannelText->direction = TRD_TOP_MIDDLE;

    alAppend(chatHeader->components, &chatOptionsText);
    alAppend(chatHeader->components, &chatUsernameText);
    alAppend(chatHeader->components, &chatChannelText);

    chatFooter = newGroup();
    chatFooter->component.anchor.size.xType = VEC_RELATIVE;
    chatFooter->component.anchor.size.relX = 1.0;
    chatFooter->component.anchor.size.absY = 3;
    chatFooter->component.anchor.origin.relY = 1.0;
    chatFooter->component.anchor.position.yType = VEC_RELATIVE;
    chatFooter->component.anchor.position.relY = 1.0;
    chatFooter->constraint = GROUP_HBOX;
    chatFooter->xPad = 1;
    chatFooter->leftMargin = 1;
    chatFooter->rightMargin = 1;
    chatFooter->topMargin = 1;
    groupAddBackground(chatFooter, 3);
    
    chatMessageBox = newLineEdit();
    chatMessageBox->component.anchor.size.xType = VEC_RELATIVE;
    chatMessageBox->component.anchor.size.relX = 1.0;
    chatMessageBox->component.anchor.size.absY = 1;
    chatMessageBox->label = "Message: ";
    chatMessageBox->active = true;
    alAppend(chatFooter->components, &chatMessageBox);

    chatScene = newGroup();
    chatScene->component.receiveInput = chatSceneReceiveInput;
    chatScene->component.anchor.size.xType = VEC_RELATIVE;
    chatScene->component.anchor.size.yType = VEC_RELATIVE;
    chatScene->component.anchor.size.relX = 1.0;
    chatScene->component.anchor.size.relY = 1.0;
    alAppend(chatScene->components, &chatHeader);
    alAppend(chatScene->components, &chatFooter);

}