#pragma once
#include <cstdint>
bool screenInit();
int screenWidth();
int screenHeight();
uint32_t *screenBackBuffer();
void screenPresent();
