#ifndef C_RUNTIME_Z_LOG_H
#define C_RUNTIME_Z_LOG_H

#include <printf.h>

#define LINE_PS     "> "
#define NEWLINE     "\n"

#ifdef DISABLE_LOG
#define LOG(message, args...)
#else
#define LOG(message, args...) printf(LINE_PS message NEWLINE, ## args)
#endif

#endif //C_RUNTIME_Z_LOG_H
