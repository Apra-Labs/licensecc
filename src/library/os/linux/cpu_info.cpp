#include "../cpu_info.hpp"
#include <cstring>
#include <string>

namespace license {
namespace os {

CpuInfo::CpuInfo() : m_vendor("ARM"), m_brand("ARM Processor") {
    // ARM CPU - simplified implementation
}

CpuInfo::~CpuInfo() {}

uint32_t CpuInfo::model() const {
    return 0; // ARM model detection not implemented
}

bool CpuInfo::is_hypervisor_set() const {
    return false; // ARM hypervisor detection not implemented
}

}  // namespace os
}  // namespace license
