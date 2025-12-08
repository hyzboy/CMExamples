// 几何库改进示例代码
// 这些是建议的改进实现，可以逐步集成到现有代码中

#pragma once
#include<hgl/math/Vector.h>
#include<hgl/math/geometry/AABB.h>
#include<hgl/math/geometry/OBB.h>
#include<optional>
#include<cassert>

namespace hgl::math
{
    // ============================================================================
    // 改进1: 添加边界检查的安全访问函数
    // ============================================================================
    
    /**
     * AABB 扩展 - 添加安全的索引访问
     */
    class AABBSafe : public AABB
    {
    public:
        using AABB::AABB;  // 继承构造函数
        
        /**
         * 获取面平面（带边界检查）
         * @param index 面索引 (0-5)
         * @return 平面的可选值，如果索引无效返回 nullopt
         */
        [[nodiscard]] std::optional<Plane> TryGetFacePlane(size_t index) const {
            if(index >= 6) return std::nullopt;
            return GetFacePlanes(index);
        }
        
        /**
         * 检查点是否在AABB内
         * @param point 待检查的点
         * @return true 如果点在AABB内
         */
        [[nodiscard]] bool Contains(const Vector3f& point) const {
            const Vector3f& min = GetMin();
            const Vector3f& max = GetMax();
            
            return point.x >= min.x && point.x <= max.x &&
                   point.y >= min.y && point.y <= max.y &&
                   point.z >= min.z && point.z <= max.z;
        }
        
        /**
         * 检查两个AABB是否相交
         * @param other 另一个AABB
         * @return true 如果两个AABB相交
         */
        [[nodiscard]] bool Intersects(const AABB& other) const {
            const Vector3f& min1 = GetMin();
            const Vector3f& max1 = GetMax();
            const Vector3f& min2 = other.GetMin();
            const Vector3f& max2 = other.GetMax();
            
            return !(max1.x < min2.x || min1.x > max2.x ||
                     max1.y < min2.y || min1.y > max2.y ||
                     max1.z < min2.z || min1.z > max2.z);
        }
        
        /**
         * 计算与另一个AABB的交集
         * @param other 另一个AABB
         * @return 交集AABB，如果不相交返回 nullopt
         */
        [[nodiscard]] std::optional<AABB> GetIntersection(const AABB& other) const {
            if(!Intersects(other)) return std::nullopt;
            
            const Vector3f& min1 = GetMin();
            const Vector3f& max1 = GetMax();
            const Vector3f& min2 = other.GetMin();
            const Vector3f& max2 = other.GetMax();
            
            AABB result;
            result.SetMinMax(
                MaxVector(min1, min2),
                MinVector(max1, max2)
            );
            return result;
        }
        
        /**
         * 计算到另一个AABB的距离
         * @param other 另一个AABB
         * @return 距离值（如果相交返回0）
         */
        [[nodiscard]] float Distance(const AABB& other) const {
            if(Intersects(other)) return 0.0f;
            
            const Vector3f& min1 = GetMin();
            const Vector3f& max1 = GetMax();
            const Vector3f& min2 = other.GetMin();
            const Vector3f& max2 = other.GetMax();
            
            Vector3f delta(0.0f);
            
            // X轴距离
            if(max1.x < min2.x) delta.x = min2.x - max1.x;
            else if(min1.x > max2.x) delta.x = min1.x - max2.x;
            
            // Y轴距离
            if(max1.y < min2.y) delta.y = min2.y - max1.y;
            else if(min1.y > max2.y) delta.y = min1.y - max2.y;
            
            // Z轴距离
            if(max1.z < min2.z) delta.z = min2.z - max1.z;
            else if(min1.z > max2.z) delta.z = min1.z - max2.z;
            
            return glm::length(delta);
        }
        
        /**
         * 获取8个顶点
         * @param corners 输出的8个顶点数组
         */
        void GetCorners(Vector3f corners[8]) const {
            const Vector3f& min = GetMin();
            const Vector3f& max = GetMax();
            
            corners[0] = Vector3f(min.x, min.y, min.z);
            corners[1] = Vector3f(max.x, min.y, min.z);
            corners[2] = Vector3f(min.x, max.y, min.z);
            corners[3] = Vector3f(max.x, max.y, min.z);
            corners[4] = Vector3f(min.x, min.y, max.z);
            corners[5] = Vector3f(max.x, min.y, max.z);
            corners[6] = Vector3f(min.x, max.y, max.z);
            corners[7] = Vector3f(max.x, max.y, max.z);
        }
        
        /**
         * 获取表面积
         */
        [[nodiscard]] float GetSurfaceArea() const {
            const Vector3f& len = GetLength();
            return 2.0f * (len.x * len.y + len.y * len.z + len.z * len.x);
        }
        
        /**
         * 获取体积
         */
        [[nodiscard]] float GetVolume() const {
            const Vector3f& len = GetLength();
            return len.x * len.y * len.z;
        }
    };
    
    // ============================================================================
    // 改进2: OBB 与 OBB 碰撞检测（分离轴定理）
    // ============================================================================
    
    /**
     * 检查两个OBB是否相交（使用分离轴定理）
     * @param obb1 第一个OBB
     * @param obb2 第二个OBB
     * @return true 如果两个OBB相交
     */
    [[nodiscard]] inline bool OBBIntersectsOBB(const OBB& obb1, const OBB& obb2) {
        const Vector3f& c1 = obb1.GetCenter();
        const Vector3f& c2 = obb2.GetCenter();
        const Vector3f t = c2 - c1;
        
        const Vector3f& e1 = obb1.GetHalfExtend();
        const Vector3f& e2 = obb2.GetHalfExtend();
        
        // 15个分离轴测试
        // 3个来自obb1的轴
        for(int i = 0; i < 3; i++) {
            const Vector3f& axis = obb1.GetAxis(i);
            float ra = e1[i];
            float rb = std::abs(glm::dot(obb2.GetAxis(0), axis)) * e2.x +
                       std::abs(glm::dot(obb2.GetAxis(1), axis)) * e2.y +
                       std::abs(glm::dot(obb2.GetAxis(2), axis)) * e2.z;
            if(std::abs(glm::dot(t, axis)) > ra + rb) return false;
        }
        
        // 3个来自obb2的轴
        for(int i = 0; i < 3; i++) {
            const Vector3f& axis = obb2.GetAxis(i);
            float ra = std::abs(glm::dot(obb1.GetAxis(0), axis)) * e1.x +
                       std::abs(glm::dot(obb1.GetAxis(1), axis)) * e1.y +
                       std::abs(glm::dot(obb1.GetAxis(2), axis)) * e1.z;
            float rb = e2[i];
            if(std::abs(glm::dot(t, axis)) > ra + rb) return false;
        }
        
        // 9个来自轴的叉积
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                Vector3f axis = glm::cross(obb1.GetAxis(i), obb2.GetAxis(j));
                float len = glm::length(axis);
                if(len < 1e-6f) continue;  // 轴平行，跳过
                axis /= len;
                
                float ra = std::abs(glm::dot(obb1.GetAxis((i+1)%3), axis)) * e1[(i+1)%3] +
                          std::abs(glm::dot(obb1.GetAxis((i+2)%3), axis)) * e1[(i+2)%3];
                float rb = std::abs(glm::dot(obb2.GetAxis((j+1)%3), axis)) * e2[(j+1)%3] +
                          std::abs(glm::dot(obb2.GetAxis((j+2)%3), axis)) * e2[(j+2)%3];
                          
                if(std::abs(glm::dot(t, axis)) > ra + rb) return false;
            }
        }
        
        return true;
    }
    
    // ============================================================================
    // 改进3: 球体碰撞检测
    // ============================================================================
    
    /**
     * 检查两个球体是否相交
     */
    [[nodiscard]] inline bool SphereIntersectsSphere(
        const Vector3f& center1, float radius1,
        const Vector3f& center2, float radius2)
    {
        float distSq = glm::length2(center2 - center1);
        float radiusSum = radius1 + radius2;
        return distSq <= radiusSum * radiusSum;
    }
    
    /**
     * 检查球体与AABB是否相交
     */
    [[nodiscard]] inline bool SphereIntersectsAABB(
        const Vector3f& sphereCenter, float sphereRadius,
        const AABB& aabb)
    {
        // 找到AABB上最接近球心的点
        const Vector3f& min = aabb.GetMin();
        const Vector3f& max = aabb.GetMax();
        
        Vector3f closestPoint = glm::clamp(sphereCenter, min, max);
        
        // 计算距离
        float distSq = glm::length2(closestPoint - sphereCenter);
        return distSq <= sphereRadius * sphereRadius;
    }
    
    // ============================================================================
    // 改进4: 射线与几何体相交测试（返回距离）
    // ============================================================================
    
    /**
     * 射线与AABB相交测试
     * @param rayOrigin 射线起点
     * @param rayDir 射线方向（应该是归一化的）
     * @param aabb AABB
     * @return 相交距离，如果不相交返回 nullopt
     */
    [[nodiscard]] inline std::optional<float> RayIntersectAABB(
        const Vector3f& rayOrigin,
        const Vector3f& rayDir,
        const AABB& aabb)
    {
        const Vector3f& min = aabb.GetMin();
        const Vector3f& max = aabb.GetMax();
        
        float tmin = 0.0f;
        float tmax = std::numeric_limits<float>::infinity();
        
        for(int i = 0; i < 3; i++) {
            if(std::abs(rayDir[i]) < 1e-8f) {
                // 射线平行于平板
                if(rayOrigin[i] < min[i] || rayOrigin[i] > max[i])
                    return std::nullopt;
            } else {
                float invD = 1.0f / rayDir[i];
                float t1 = (min[i] - rayOrigin[i]) * invD;
                float t2 = (max[i] - rayOrigin[i]) * invD;
                
                if(t1 > t2) std::swap(t1, t2);
                
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
                
                if(tmin > tmax) return std::nullopt;
            }
        }
        
        return tmin >= 0.0f ? std::make_optional(tmin) : std::nullopt;
    }
    
    // ============================================================================
    // 改进5: 调试和验证辅助函数
    // ============================================================================
    
    /**
     * 验证AABB数据一致性
     */
    [[nodiscard]] inline bool ValidateAABB(const AABB& aabb) {
        const Vector3f& min = aabb.GetMin();
        const Vector3f& max = aabb.GetMax();
        const Vector3f& len = aabb.GetLength();
        const Vector3f& center = aabb.GetCenter();
        
        // 检查min <= max
        if(min.x > max.x || min.y > max.y || min.z > max.z)
            return false;
        
        // 检查长度计算
        Vector3f expectedLen = max - min;
        if(!glm::all(glm::epsilonEqual(len, expectedLen, 1e-5f)))
            return false;
        
        // 检查中心计算
        Vector3f expectedCenter = (min + max) * 0.5f;
        if(!glm::all(glm::epsilonEqual(center, expectedCenter, 1e-5f)))
            return false;
        
        return true;
    }
    
    /**
     * 验证OBB数据一致性
     */
    [[nodiscard]] inline bool ValidateOBB(const OBB& obb) {
        // 检查轴是否正交归一化
        for(int i = 0; i < 3; i++) {
            const Vector3f& axis = obb.GetAxis(i);
            float len = glm::length(axis);
            if(std::abs(len - 1.0f) > 1e-5f)
                return false;
        }
        
        // 检查轴之间的正交性
        for(int i = 0; i < 3; i++) {
            for(int j = i + 1; j < 3; j++) {
                float dot = glm::dot(obb.GetAxis(i), obb.GetAxis(j));
                if(std::abs(dot) > 1e-5f)
                    return false;
            }
        }
        
        // 检查半长度为非负
        const Vector3f& halfLen = obb.GetHalfExtend();
        if(halfLen.x < 0 || halfLen.y < 0 || halfLen.z < 0)
            return false;
        
        return true;
    }
    
}//namespace hgl::math
