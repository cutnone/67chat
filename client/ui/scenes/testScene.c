#include "../component.h"
#include "../components/group.h"
#include "../components/text.h"
#include "../components/rectangle.h"
#include "../../comms.h"
#include "../../interface.h"
#include <curses.h>


Group *testScene;

void initializeTestScene() {
    testScene = newGroup();
    testScene->constraint = GROUP_VBOX;
    testScene->component.anchor.size.xType = VEC_RELATIVE;
    testScene->component.anchor.size.yType = VEC_RELATIVE;
    testScene->component.anchor.size.relX = 1;
    testScene->component.anchor.size.relY = 1;
    testScene->yPad = 1;

    RectangleComponent *rect = newRectangle();
    rect->color = 1;
    rect->component.anchor.size.absX = 35;
    // rect->component.anchor.size.yType = VEC_RELATIVE;
    rect->component.anchor.size.absY = 10;
    
    rect->component.anchor.position.xType = VEC_RELATIVE;
    rect->component.anchor.position.yType = VEC_RELATIVE;
    rect->component.anchor.position.relX = 0.5;
    rect->component.anchor.position.relY = 0;
    
    rect->component.anchor.origin.relX = 0.5;
    rect->component.anchor.origin.relY = 0;
    TextComponent *rect2 = newTextComponent();
    ArrayList *instructions = stringToInstructions("What the fuck did you just fucking say about me, you little bitch? I'll have you know I graduated top of my class in the Navy Seals, and I've been involved in numerous secret raids on Al-Quaeda, and I have over 300 confirmed kills. I am trained in gorilla warfare and I'm the top sniper in the entire US armed forces. You are nothing to me but just another target. I will wipe you the fuck out with precision the likes of which has never been seen before on this Earth, mark my fucking words. You think you can get away with saying that shit to me over the Internet? Think again, fucker. As we speak I am contacting my secret network of spies across the USA and your IP is being traced right now so you better prepare for the storm, maggot. The storm that wipes out the pathetic little thing you call your life. You're fucking dead, kid. I can be anywhere, anytime, and I can kill you in over seven hundred ways, and that's just with my bare hands. Not only am I extensively trained in unarmed combat, but I have access to the entire arsenal of the United States Marine Corps and I will use it to its full extent to wipe your miserable ass off the face of the continent, you little shit. If only you could have known what unholy retribution your little \"clever\" comment was about to bring down upon you, maybe you would have held your fucking tongue. But you couldn't, you didn't, and now you're paying the price, you goddamn idiot. I will shit fury all over you and you will drown in it. You're fucking dead, kiddo.");
    rect2->direction = TRD_TOP_MIDDLE;
    rect2->instructions = instructions;
    rect2->component.anchor = rect->component.anchor;
    rect2->component.anchor.size.yType = VEC_RELATIVE;
    rect2->component.anchor.size.relY = 1.0;
    rect2->component.anchor.size.xType = VEC_RELATIVE;
    rect2->component.anchor.size.relX= 0.5;
    rect2->component.anchor.position.relX = 0.5;
    rect2->component.anchor.position.relY = 0;
    
    RectangleComponent *rect3 = newRectangle();
    alAppend(testScene->components, &rect);
    rect3->component.anchor = rect->component.anchor;
    rect3->component.anchor.size.absY = 5;
    rect3->color = 3;
    rect3->component.anchor.origin.relY = 1;
    rect3->component.anchor.position.relX = 0.5;
    rect3->component.anchor.position.relY = 1;

    alAppend(testScene->components, &rect3);
    alAppend(testScene->components, &rect2);
}