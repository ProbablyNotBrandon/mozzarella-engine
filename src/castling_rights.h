#ifndef CASTLING_RIGHTS_H
#define CASTLING_RIGHTS_H

#include <cstdint>
#include <type_traits>

enum class CastlingRights : uint8_t {
    KING  = 1,
    QROOK = 2,
    KROOK = 4
};

// Enable bitwise ops for CastlingRights
inline constexpr CastlingRights operator|(CastlingRights lhs, CastlingRights rhs) {
    using T = std::underlying_type_t<CastlingRights>;
    return static_cast<CastlingRights>(
        static_cast<T>(lhs) | static_cast<T>(rhs)
    );
}

inline constexpr CastlingRights operator&(CastlingRights lhs, CastlingRights rhs) {
    using T = std::underlying_type_t<CastlingRights>;
    return static_cast<CastlingRights>(
        static_cast<T>(lhs) & static_cast<T>(rhs)
    );
}

inline constexpr CastlingRights operator^(CastlingRights lhs, CastlingRights rhs) {
    using T = std::underlying_type_t<CastlingRights>;
    return static_cast<CastlingRights>(
        static_cast<T>(lhs) ^ static_cast<T>(rhs)
    );
}

inline constexpr CastlingRights operator~(CastlingRights val) {
    using T = std::underlying_type_t<CastlingRights>;
    return static_cast<CastlingRights>(
        ~static_cast<T>(val)
    );
}

inline constexpr CastlingRights& operator|=(CastlingRights& lhs, CastlingRights rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline constexpr CastlingRights& operator&=(CastlingRights& lhs, CastlingRights rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline constexpr CastlingRights& operator^=(CastlingRights& lhs, CastlingRights rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

#endif
