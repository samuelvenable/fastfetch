#include "memory.h"

#include <string.h>
#include <mach/mach.h>
#include <sys/sysctl.h>
#include <unistd.h>

const char* ffDetectMemory(FFMemoryResult* ram)
{
    size_t length = sizeof(ram->bytesTotal);
    if (sysctl((int[]){ CTL_HW, HW_MEMSIZE }, 2, &ram->bytesTotal, &length, NULL, 0) != 0)
        return "Failed to read hw.memsize";

    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat;
    if(host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t) (&vmstat), &count) != KERN_SUCCESS)
        return "Failed to read host_statistics64";

    // Match what the official top(1) command does: https://github.com/apple-opensource/top/blob/e7979606cf63270663a62cfe69f82d35cef9ba58/globalstats.c#L433-L435
    ram->bytesUsed = ((uint64_t)vmstat.wire_count + vmstat.inactive_count + vmstat.active_count + vmstat.compressor_page_count) * instance.state.platform.sysinfo.pageSize;

    // Match Activity Monitor: https://github.com/st3fan/osx-10.9/blob/34e34a6a539b5a822cda4074e56a7ced9b57da71/system_cmds-597.1.1/vm_stat.tproj/vm_stat.c#L139
    ram->bytesUsed = ram->bytesTotal - ((uint64_t)(vmstat.free_count - vmstat.speculative_count) + vmstat.external_page_count) * instance.state.platform.sysinfo.pageSize;
    // Comment out the above line to use top(1)'s method of getting memory used instead of Activity Monitor's method.

    return NULL;
}
