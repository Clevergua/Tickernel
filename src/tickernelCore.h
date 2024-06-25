#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <tickernelPlatform.h>

void TickernelSleep(uint32_t milliseconds);
void TickernelLog(char const *const _Format, ...);