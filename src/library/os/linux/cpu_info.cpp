/*
 * cpu_info.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: devel
 */

#include "../cpu_info.hpp"
#include <string>
#include <cstring>

// Include cpuid.h only for x86/x64 architectures
#if defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>
#include <unordered_set>
#include <memory.h>
#endif

namespace license {
namespace os {
using namespace std;

#if defined(__x86_64__) || defined(__i386__)
// x86/x64 implementation

struct CPUVendorID {
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;

	string toString() const { return string(reinterpret_cast<const char *>(this), 12); }
};

static string get_cpu_vendor() {
	unsigned int level = 0, eax = 0, ebx = 0, ecx = 0, edx = 0;
	// hypervisor flag false, try to get the vendor name, see if it's a virtual cpu
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	CPUVendorID vendorID;
	vendorID.ebx = ebx;
	vendorID.edx = edx;
	vendorID.ecx = ecx;
	return vendorID.toString();
}

// https://en.wikipedia.org/wiki/CPUID
static string get_cpu_brand() {
	string result;
	uint32_t brand[0x10];

	if (!__get_cpuid_max(0x80000004, NULL)) {
		result = "NA";
	} else {
		memset(brand, 0, sizeof(brand));
		__get_cpuid(0x80000002, brand + 0x0, brand + 0x1, brand + 0x2, brand + 0x3);
		__get_cpuid(0x80000003, brand + 0x4, brand + 0x5, brand + 0x6, brand + 0x7);
		__get_cpuid(0x80000004, brand + 0x8, brand + 0x9, brand + 0xa, brand + 0xb);
		result = string(reinterpret_cast<char *>(brand));
	}
	return result;
}

CpuInfo::CpuInfo() : m_vendor(get_cpu_vendor()), m_brand(get_cpu_brand()) {}

CpuInfo::~CpuInfo() {}

/**
 * Detect Virtual machine using hypervisor bit.
 * @return true if the cpu hypervisor bit is set to 1
 */
bool CpuInfo::is_hypervisor_set() const {
	uint32_t level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);

	bool is_virtual = (((ecx >> 31) & 1) == 1);  // hypervisor flag
	return is_virtual;
}

uint32_t CpuInfo::model() const {
	uint32_t level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	// ax bits 0-3 stepping,4-7 model,8-11 family id,12-13 processor type
	//        14-15 reserved, 16-19 extended model, 20-27 extended family, 27-31 reserved
	// bx bits 0-7 brand index
	return (eax & 0x3FFF) | (eax & 0x3FF8000) >> 2 | (ebx & 0xff) << 24;
}

#elif defined(__aarch64__) || defined(__arm__)
// ARM implementation

CpuInfo::CpuInfo() : m_vendor("ARM"), m_brand("ARM Processor") {
    // ARM CPU - simplified implementation
    // TODO: Could parse /proc/cpuinfo for more details if needed
}

CpuInfo::~CpuInfo() {}

uint32_t CpuInfo::model() const {
    return 0; // ARM model detection not implemented
    // TODO: Could parse /proc/cpuinfo for CPU part number
}

bool CpuInfo::is_hypervisor_set() const {
    return false; // ARM hypervisor detection not implemented
    // TODO: Could check /proc/cpuinfo for virtualization features
}

#else
// Generic/Unknown architecture fallback

CpuInfo::CpuInfo() : m_vendor("Unknown"), m_brand("Unknown Processor") {}

CpuInfo::~CpuInfo() {}

uint32_t CpuInfo::model() const {
    return 0;
}

bool CpuInfo::is_hypervisor_set() const {
    return false;
}

#endif

}  // namespace os
} /* namespace license */
