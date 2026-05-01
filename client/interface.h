#pragma once
#include "ui/component.h"

void init();
void rerender();
void checkKeystrokes();
void cleanup();
void resize();

extern Component *screenComponent;