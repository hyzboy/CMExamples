#include<hgl/platform/CpuInfo.h>
#include<hgl/platform/BigLittleDetector.h>
#include<iostream>
#include<iomanip>

using namespace hgl;
using namespace std;

void PrintCpuInfo(const CpuInfo& ci)
{
    cout << "=== CPU Information ===" << endl;
    cout << "CPU Count: " << ci.cpu_count << endl;
    cout << "Core Count: " << ci.core_count << endl;
    cout << "Logical Core Count: " << ci.logical_core_count << endl;
    
    cout << "Architecture: ";
    switch(ci.arch)
    {
        case CpuArch::x86_64: cout << "x86_64"; break;
        case CpuArch::ARMv8: cout << "ARMv8"; break;
        case CpuArch::ARMv9: cout << "ARMv9"; break;
        default: cout << "Unknown"; break;
    }
    cout << endl << endl;

    #if defined(_M_AMD64) || defined(_M_X64) || defined(_M_IX86)

        const CpuFeatures& x86 = ci.features;
        
        cout << "=== X86 CPU Features ===" << endl;
        cout << "Vendor: " << x86.vendor << endl;
        cout << "Brand: " << x86.brand << endl;
        cout << "Family: " << x86.family << endl;
        cout << "Model: " << x86.model << endl;
        cout << "Stepping: " << x86.stepping << endl << endl;

        cout << "=== Instruction Set Support ===" << endl;
        cout << "MMX: " << (x86.has_mmx ? "Yes" : "No") << endl;
        cout << "SSE: " << (x86.has_sse ? "Yes" : "No") << endl;
        cout << "SSE2: " << (x86.has_sse2 ? "Yes" : "No") << endl;
        cout << "SSE3: " << (x86.has_sse3 ? "Yes" : "No") << endl;
        cout << "SSSE3: " << (x86.has_ssse3 ? "Yes" : "No") << endl;
        cout << "SSE4.1: " << (x86.has_sse4_1 ? "Yes" : "No") << endl;
        cout << "SSE4.2: " << (x86.has_sse4_2 ? "Yes" : "No") << endl;
        cout << "AVX: " << (x86.has_avx ? "Yes" : "No") << endl;
        cout << "AVX2: " << (x86.has_avx2 ? "Yes" : "No") << endl;
        cout << "AVX-512 Foundation: " << (x86.has_avx512_f ? "Yes" : "No") << endl;
        cout << "AVX-512 DQ: " << (x86.has_avx512_dq ? "Yes" : "No") << endl;
        cout << "AVX-512 IFMA: " << (x86.has_avx512_ifma ? "Yes" : "No") << endl;
        cout << "AVX-512 PF: " << (x86.has_avx512_pf ? "Yes" : "No") << endl;
        cout << "AVX-512 ER: " << (x86.has_avx512_er ? "Yes" : "No") << endl;
        cout << "AVX-512 CD: " << (x86.has_avx512_cd ? "Yes" : "No") << endl;
        cout << "AVX-512 BW: " << (x86.has_avx512_bw ? "Yes" : "No") << endl;
        cout << "AVX-512 VL: " << (x86.has_avx512_vl ? "Yes" : "No") << endl << endl;

        cout << "=== Additional Features ===" << endl;
        cout << "FMA3: " << (x86.has_fma3 ? "Yes" : "No") << endl;
        cout << "FMA4: " << (x86.has_fma4 ? "Yes" : "No") << endl;
        cout << "AES: " << (x86.has_aes ? "Yes" : "No") << endl;
        cout << "SHA: " << (x86.has_sha ? "Yes" : "No") << endl;
        cout << "PCLMULQDQ: " << (x86.has_pclmulqdq ? "Yes" : "No") << endl;
        cout << "RDRAND: " << (x86.has_rdrand ? "Yes" : "No") << endl;
        cout << "RDSEED: " << (x86.has_rdseed ? "Yes" : "No") << endl;
        cout << "ADX: " << (x86.has_adx ? "Yes" : "No") << endl;
        cout << "BMI1: " << (x86.has_bmi1 ? "Yes" : "No") << endl;
        cout << "BMI2: " << (x86.has_bmi2 ? "Yes" : "No") << endl;
        cout << "POPCNT: " << (x86.has_popcnt ? "Yes" : "No") << endl;
        cout << "LZCNT: " << (x86.has_lzcnt ? "Yes" : "No") << endl;
        cout << "PREFETCHW: " << (x86.has_prefetchw ? "Yes" : "No") << endl << endl;

        cout << "=== Cache Information ===" << endl;
        cout << "L1 Data Cache: " << x86.l1_data_cache_size << " KB" << endl;
        cout << "L1 Instruction Cache: " << x86.l1_inst_cache_size << " KB" << endl;
        cout << "L2 Cache: " << x86.l2_cache_size << " KB" << endl;
        cout << "L3 Cache: " << x86.l3_cache_size << " KB" << endl << endl;

        cout << "=== Frequency Information ===" << endl;
        cout << "Base Frequency: " << x86.base_frequency << " MHz" << endl;
        cout << "Max Frequency: " << x86.max_frequency << " MHz" << endl;
        cout << "Bus Frequency: " << x86.bus_frequency << " MHz" << endl;

    #elif defined(_M_ARM) || defined(_M_ARM64)

        const CpuFeatures& arm = ci.features;

        cout << "=== ARM CPU Features ===" << endl;
        cout << "Implementer: " << arm.implementer << endl;
        cout << "Part Number: 0x" << hex << arm.part_number << dec << endl;
        cout << "Variant: " << arm.variant << endl;
        cout << "Revision: " << arm.revision << endl << endl;

        cout << "=== Instruction Set Support ===" << endl;
        cout << "NEON: " << (arm.has_neon ? "Yes" : "No") << endl;
        cout << "VFP: " << (arm.has_vfp ? "Yes" : "No") << endl;
        cout << "VFPv3: " << (arm.has_vfpv3 ? "Yes" : "No") << endl;
        cout << "VFPv4: " << (arm.has_vfpv4 ? "Yes" : "No") << endl;
        cout << "AES: " << (arm.has_aes ? "Yes" : "No") << endl;
        cout << "SHA1: " << (arm.has_sha1 ? "Yes" : "No") << endl;
        cout << "SHA2: " << (arm.has_sha2 ? "Yes" : "No") << endl;
        cout << "CRC32: " << (arm.has_crc32 ? "Yes" : "No") << endl;
        cout << "PMULL: " << (arm.has_pmull ? "Yes" : "No") << endl;
        cout << "FP16: " << (arm.has_fp16 ? "Yes" : "No") << endl;
        cout << "Dot Product: " << (arm.has_dotprod ? "Yes" : "No") << endl;
        cout << "I8MM: " << (arm.has_i8mm ? "Yes" : "No") << endl;
        cout << "BF16: " << (arm.has_bf16 ? "Yes" : "No") << endl;
        cout << "SVE: " << (arm.has_sve ? "Yes" : "No") << endl << endl;

        cout << "=== Big.LITTLE Core Information ===" << endl;
        cout << "Has Big.LITTLE: " << (arm.has_big_little ? "Yes" : "No") << endl;
        if (arm.has_big_little)
        {
            cout << "Big Cores: " << arm.big_core_count << endl;
            cout << "Little Cores: " << arm.little_core_count << endl;
            cout << "Big Core Max Frequency: " << arm.big_core_max_freq << " MHz" << endl;
            cout << "Little Core Max Frequency: " << arm.little_core_max_freq << " MHz" << endl;
        }
        else
        {
            cout << "Unified Cores: " << arm.big_core_count << endl;
            cout << "Max Frequency: " << arm.big_core_max_freq << " MHz" << endl;
        }
        cout << endl;

        cout << "=== Cache Information ===" << endl;
        cout << "L1 Data Cache: " << arm.l1_data_cache_size << " KB" << endl;
        cout << "L1 Instruction Cache: " << arm.l1_inst_cache_size << " KB" << endl;
        cout << "L2 Cache: " << arm.l2_cache_size << " KB" << endl;
        cout << "L3 Cache: " << arm.l3_cache_size << " KB" << endl;
    #else
        cout<<"ARM CPU feature retrieval not supported on this platform."<<endl;
    #endif
}

#if defined(_M_ARM) || defined(_M_ARM64)
void TestBigLittleDetection()
{
    cout << "\n=== Big.LITTLE Detection Test ===" << endl;

    CpuInfo ci;
    if (!GetCpuInfo(&ci)) {
        cout << "Failed to get CPU info!" << endl;
        return;
    }

    // 使用新的检测器
    BigLittleDetector::DetectionResult result = BigLittleDetector::Detect(ci);

    cout << "Architecture: ";
    switch(ci.arch) {
        case CpuArch::x86_64: cout << "x86_64 (No Big.LITTLE)"; break;
        case CpuArch::ARMv8: cout << "ARMv8"; break;
        case CpuArch::ARMv9: cout << "ARMv9"; break;
        default: cout << "Unknown"; break;
    }
    cout << endl;

    cout << "Big.LITTLE Architecture: " << (result.has_big_little ? "Detected" : "Not Detected") << endl;

    if (result.has_big_little) {
        cout << "Configuration Analysis:" << endl;
        cout << "  - Big Cores: " << result.big_cores << endl;
        cout << "  - Little Cores: " << result.little_cores << endl;
        cout << "  - Total Cores: " << (result.big_cores + result.little_cores) << endl;
        cout << "  - Big/Little Ratio: " << fixed << setprecision(2)
             << (double)result.big_cores / result.little_cores << endl;

        cout << "Frequency Analysis:" << endl;
        cout << "  - Big Core Max: " << result.big_freq << " MHz" << endl;
        cout << "  - Little Core Max: " << result.little_freq << " MHz" << endl;
        cout << "  - Frequency Ratio: " << fixed << setprecision(2)
             << (double)result.big_freq / result.little_freq << endl;
        cout << "  - Performance Ratio: " << fixed << setprecision(2)
             << result.performance_ratio << endl;

        cout << "Recommendation: " << result.recommendation << endl;

        // 测试核心分配建议
        uint32 big_compute, little_bg, unified;
        BigLittleDetector::GetRecommendedCoreAllocation(ci, big_compute, little_bg, unified);

        cout << "Recommended Core Allocation:" << endl;
        cout << "  - Big cores for compute: " << big_compute << endl;
        cout << "  - Little cores for background: " << little_bg << endl;
        cout << "  - Unified cores: " << unified << endl;

        // 性能分数
        double perf_score = BigLittleDetector::CalculatePerformanceScore(ci);
        cout << "Performance Score: " << fixed << setprecision(1) << perf_score << endl;

        // 使用建议测试
        cout << "Usage Recommendations:" << endl;
        cout << "  - Compute-intensive task should use big cores: "
             << (BigLittleDetector::ShouldUseBigCores(ci, true, false) ? "Yes" : "No") << endl;
        cout << "  - Power-sensitive task should use big cores: "
             << (BigLittleDetector::ShouldUseBigCores(ci, false, true) ? "Yes" : "No") << endl;
    } else {
        cout << result.recommendation << endl;

        // 对于统一架构，显示一些基本信息
        if (ci.arch == CpuArch::ARMv8 || ci.arch == CpuArch::ARMv9) {
            cout << "  - All cores run at: " << ci.arm.big_core_max_freq << " MHz" << endl;
            cout << "  - Suitable for general-purpose workloads" << endl;
        }
    }

    // 便捷函数测试
    cout << "\nQuick Checks:" << endl;
    cout << "  - Has Big.LITTLE: " << (HasBigLittleCores(ci) ? "Yes" : "No") << endl;
    cout << "  - Big cores count: " << GetTotalBigCores(ci) << endl;
    cout << "  - Little cores count: " << GetTotalLittleCores(ci) << endl;
}
#endif// defined(_M_ARM) || defined(_M_ARM64)

int main()
{
    CpuInfo ci{};
    
    if(GetCpuInfo(&ci))
    {
        PrintCpuInfo(ci);
    }
    else
    {
        cout << "Failed to get CPU information!" << endl;
        return 1;
    }

#if  defined(_M_ARM) || defined(_M_ARM64)
    TestBigLittleDetection();       // 测试大小核检测
#endif// defined(_M_ARM) || defined(_M_ARM64)

    return 0;
}