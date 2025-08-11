#include <intrin.h>
int main() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 1); // Retrieve feature information
    return (cpuInfo[2] & (1 << 31)) != 0; // Hypervisor flag in ECX
}