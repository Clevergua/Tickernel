#pragma once

#include <tickernelIO.h>
#include <stdarg.h>

FILE *const stream;

void Log(char const *const format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}
void Warning(char const *const format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}
void Error(char const *const format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}
