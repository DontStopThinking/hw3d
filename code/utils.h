#pragma once

// NOTE(sbalse): One level of indirection required to expand any macros given in the arguments first.
#define CONCAT_INTERNAL(a, b) a ## b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

// NOTE(sbalse): Use this when you require a unique name for a variable.
#define UNIQUENAME(name) CONCAT(name, __COUNTER__)

// NOTE(sbalse): Returns the size of a C-Style array. Will also fail to compile for non-array types.
template<typename T, size_t N>
constexpr size_t ArraySize(const T(&)[N]) { return N; }

// NOTE(sbalse): Code in this namespace is not meant to be used outside of this file.
namespace PrivateDefer
{
    template<typename F>
    struct Defer
    {
        F m_Func;
        Defer(const F func) : m_Func{ func } {}

        ~Defer() { m_Func(); }
    };

    template<typename F>
    Defer<F> DeferHelper(const F func) { return Defer(func); }
}

// NOTE(sbalse): Defer the execution of some code till the end of a block. Useful to make sure we don't
// forget to free some resources.
#define DEFER(code) auto UNIQUENAME(defervar) = PrivateDefer::DeferHelper([&]() { code; })
