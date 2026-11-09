#include <cstdio>
#include <cstdlib>
#include <string>
#include "components/debug/Debug.h"

namespace Debug
{
    std::string makeOutputString(const char *__file__, int lineNumber, const char *messagePrefix, const std::string &message)
    {
        return std::string(messagePrefix) + message;
    }

    [[noreturn]] void exitApplication()
    {
        std::abort();
    }

    void write(const char *message)
    {
        printf("%s\n", message);
    }

    void showErrorMessageBox(const char *message)
    {
        printf("ERROR: %s\n", message);
    }
}
