#include "memory.h"
#include "common/sysctl.h"

const char* ffDetectMemory(FFMemoryResult* ram)
{
    // vm.stats.vm.* are int values
    ram->bytesTotal = (uint64_t) ffSysctlGetInt("vm.stats.vm.v_page_count", 0);
    int32_t pagesFree = ffSysctlGetInt("vm.stats.vm.v_free_count", 0);

    ram->bytesUsed = (ram->bytesTotal - (uint64_t) pagesFree) * instance.state.platform.sysinfo.pageSize;

    return NULL;
}
