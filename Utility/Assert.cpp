#include "Utility/Assert.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if OS_WINDOWS
    #include "Windows.h"
#elif OS_LINUX
    #include <execinfo.h>
    #include <signal.h>
#endif

namespace Assert {

void Fail(const char* condition, const char* file, int line, const char* message)
{
    char buffer[2048];

    if (condition)
    {
        std::sprintf(buffer, "ASSERTION FAILURE\n  Summary: (%s) failed at (%s:%d)", condition, file, line);
    }
    else
    {
        std::sprintf(buffer, "ASSERTION FAILURE\n  Summary: Failed at (%s:%d)", file, line);
    }

    if (message)
    {
        std::strcat(buffer, "\n  Message: ");
        std::strcat(buffer, message);
    }

#if OS_WINDOWS
    void* stackTrace[20];
    int numCapturedFrames = CaptureStackBackTrace(0, 20, stackTrace, NULL);

    if (numCapturedFrames)
    {
        std::strcat(buffer, "\n  Backtrace:\n");
        for (int i = 0; i < numCapturedFrames; ++i)
        {
            char backtraceBuffer[32];
            std::sprintf(backtraceBuffer, "    [0x%p]\n", stackTrace[i]);
            std::strcat(buffer, backtraceBuffer);
        }
    }
#elif OS_LINUX
    void* stackTrace[20];
    int numCapturedFrames = backtrace(stackTrace, 20);

    if (numCapturedFrames)
    {
        char** resolvedFrames = backtrace_symbols(stackTrace, 20);
        std::strcat(buffer, "\n  Backtrace:\n");
        for (int i = 0; i < numCapturedFrames; ++i)
        {
            char backtraceBuffer[256];
            std::sprintf(backtraceBuffer, "    %s\n", resolvedFrames[i]);
            std::strcat(buffer, backtraceBuffer);
        }
    }
#endif // OS_WINDOWS

    std::fprintf(stderr, "%s", buffer);

    bool skipCrash = false;
    bool skipBreak = false;

#if OS_WINDOWS
    int response = MessageBox(GetActiveWindow(), buffer, "ASSERTION FAILURE", MB_ABORTRETRYIGNORE);

    switch (response)
    {
    case IDRETRY: // No crash, but break.
        skipCrash = true;
        break;

    case IDIGNORE: // No crash or break.
        skipCrash = true;
        skipBreak = true;
        break;

    case IDABORT:
    default: // Crash and break
        break;
    }
#endif // OS_WINDOWS

    if (!skipBreak)
    {
#if CMP_MSVC
        __debugbreak();
#elif OS_LINUX
        raise(SIGTRAP);
#endif
    }

    if (!skipCrash)
    {
        std::abort();
    }
}

}
