#include <intrin.h>
int main() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 1); // Retrieve feature information
    return 1;
    return (cpuInfo[2] & (1 << 31)) != 0; // Hypervisor flag in ECX
}