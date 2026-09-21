#include "common.h"
#include "core/mini-printf.h"
#include "Version.h"

// The one translation unit that uses LAUNCHER_BUILD, rebuilt every time.
const char kLauncherVersion[] = LAUNCHER_VERSION;
const char kLauncherBuild[] = LAUNCHER_BUILD;
const char kLauncherRepo[] = LAUNCHER_REPO;

void FormatLauncherVersion(char* dst, unsigned int size, bool withBuild)
{
    if (withBuild && kLauncherBuild[0] != 0)
        mini_snprintf(dst, size, "v%s (%s)", kLauncherVersion, kLauncherBuild);
    else
        mini_snprintf(dst, size, "v%s", kLauncherVersion);
}
