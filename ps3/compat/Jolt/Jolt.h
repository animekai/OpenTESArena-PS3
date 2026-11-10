#pragma once
// Stub de Jolt Physics para PS3: solo declara los tipos que mencionan las cabeceras del motor.
#include <cstdint>

namespace JPH
{
    struct Vec3
    {
        float x = 0, y = 0, z = 0;
        Vec3() {}
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
        float GetX() const { return x; }
        float GetY() const { return y; }
        float GetZ() const { return z; }
    };
    using RVec3 = Vec3;
    using RVec3Arg = const Vec3&;

    struct Color
    {
        uint8_t r = 0, g = 0, b = 0, a = 255;
    };
    using ColorArg = const Color&;

    class BodyID
    {
    public:
        static constexpr uint32_t cInvalidBodyID = 0xffffffffu;
        BodyID() : id(cInvalidBodyID) {}
        explicit BodyID(uint32_t id) : id(id) {}
        uint32_t GetIndexAndSequenceNumber() const { return id; }
        bool IsInvalid() const { return id == cInvalidBodyID; }
        bool operator==(const BodyID &o) const { return id == o.id; }
        bool operator!=(const BodyID &o) const { return id != o.id; }
    private:
        uint32_t id;
    };

    class SubShapeID {};
    class Body {};
    class PhysicsSystem {};
    class CompoundShape {};
    class StaticCompoundShape : public CompoundShape {};
    class DebugRendererSimple {};
}
