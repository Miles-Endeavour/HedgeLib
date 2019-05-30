#pragma once
#include "HedgeLib.h"
#include "Endian.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Offsets
typedef uint32_t hl_DataOff32;
typedef uint64_t hl_DataOff64;

// Arrays
struct hl_ArrOff32
{
    uint32_t Count;
    hl_DataOff32 Offset;
};

struct hl_ArrOff64
{
    uint64_t Count;
    hl_DataOff64 Offset;
};

#define HL_ENDIAN_SWAP_ARR32(type, v, be, swapCall) {\
    if (be) hl_SwapUint32(&(v.Count));\
\
    type* ptr = HL_GETPTR32(type, v.Offset);\
    for (uint32_t i = 0; i < v.Count; ++i)\
    {\
        swapCall;\
    }\
\
    if (!be) hl_SwapUint32(&(v.Count));\
}

#define HL_ENDIAN_SWAP_ARR64(type, v, be, swapCall) {\
    if (be) hl_SwapUint64(&(v.Count));\
\
    type* ptr = HL_GETPTR64(type, v.Offset);\
    for (uint64_t i = 0; i < v.Count; ++i)\
    {\
        swapCall;\
    }\
\
    if (!be) hl_SwapUint64(&(v.Count));\
}

#define HL_ENDIAN_SWAP_ARR32_RECURSIVE(type, v, be) HL_ENDIAN_SWAP_ARR32(\
    type, v, be, HL_ENDIAN_SWAP_RECURSIVE(type, v, be))

#define HL_ENDIAN_SWAP_ARR64_RECURSIVE(type, v, be) HL_ENDIAN_SWAP_ARR64(\
    type, v, be, HL_ENDIAN_SWAP_RECURSIVE(type, v, be))

// Offset Table
struct hl_OffsetTable;

HL_API struct hl_OffsetTable* hl_CreateOffsetTable();
HL_API void hl_AddOffset(struct hl_OffsetTable* offTable, long offset);
HL_API long* hl_GetOffsetsPtr(struct hl_OffsetTable* offTable);
HL_API size_t hl_GetOffsetCount(const struct hl_OffsetTable* offTable);
HL_API void hl_GetOffsets(struct hl_OffsetTable* offTable,
    long** offsets, size_t* count);

HL_API void hl_DestroyOffsetTable(struct hl_OffsetTable* offTable);

// Get/Set Macros & x64 Macros
#ifdef x86
#define HL_GETPTR32(type, off) ((type*)((uintptr_t)off))
#define HL_ADDPTR32(ptr) (hl_DataOff32)((uintptr_t)ptr)
#define HL_SETPTR32(off, ptr) off = HL_ADDPTR32(ptr)
#define HL_REMOVEPTR32(off)
#define HL_DECL_X64_ADD_OFFSETS(type)
#define HL_DECL_X64_REMOVE_OFFSETS(type)
#define HL_IMPL_X64_ADD_OFFSETS(type, v)
#define HL_IMPL_X64_REMOVE_OFFSETS(type, v)
#define HL_X64_ADD_OFFSETS(type, v)
#define HL_X64_REMOVE_OFFSETS(type, v)
#define HL_X64_ADD_OFFSETS_ARR(type, v, count)
#define HL_X64_REMOVE_OFFSETS_ARR(type, v, count)
#elif x64
HL_API uintptr_t hl_x64GetAbsPtr32(hl_DataOff32 index);
HL_API hl_DataOff32 hl_x64AddAbsPtr32(uintptr_t ptr);
HL_API void hl_x64SetAbsPtr32(hl_DataOff32 index, uintptr_t ptr);
HL_API void hl_x64RemoveAbsPtr32(hl_DataOff32 index);

#define HL_GETPTR32(type, off) ((type*)hl_x64GetAbsPtr32(off))
#define HL_ADDPTR32(ptr) hl_x64AddAbsPtr32((uintptr_t)ptr)
#define HL_SETPTR32(off, ptr) hl_x64SetAbsPtr32(off, (uintptr_t)ptr)
#define HL_REMOVEPTR32(off) hl_x64RemoveAbsPtr32(off)

#define HL_DECL_X64_ADD_OFFSETS(type) HL_API void type##_x64AddOffsets(struct type* v)
#define HL_DECL_X64_REMOVE_OFFSETS(type) HL_API void type##_x64RemoveOffsets(struct type* v)
#define HL_IMPL_X64_ADD_OFFSETS(type, v) void type##_x64AddOffsets(struct type* v)
#define HL_IMPL_X64_REMOVE_OFFSETS(type, v) void type##_x64RemoveOffsets(struct type* v)

#define HL_X64_ADD_OFFSETS(type, v) type##_x64AddOffsets(v)
#define HL_X64_REMOVE_OFFSETS(type, v) type##_x64RemoveOffsets(v)
#define HL_X64_ADD_OFFSETS_ARR(type, v, count) \
    for (size_t i = 0; i < count; ++i) { type##_x64AddOffsets(&v[i]); }
#define HL_X64_REMOVE_OFFSETS_ARR(type, v, count) \
    for (size_t i = 0; i < count; ++i) { type##_x64RemoveOffsets(&v[i]); }
#endif

#define HL_GETPTR64(type, off) ((type*)((uintptr_t)off))
#define HL_GETOFF64(ptr) ((hl_DataOff64)((uintptr_t)ptr))
#define HL_SETPTR64(off, ptr) off = HL_GETOFF64(ptr)

#define HL_GETVAL32(type, off) *HL_GETPTR32(type, off)
#define HL_GETVAL64(type, off) *HL_GETPTR64(type, off)

#define HL_GETABS(type, baseAddress, offset) ((type*)((uint8_t*)baseAddress + offset))
#define HL_GETABSV(baseAddress, offset) HL_GETABS(void, baseAddress, offset)

#define HL_INIT_OBJECT(type, v) HL_X64_ADD_OFFSETS(type, v)
#define HL_CLEANUP_OBJECT(type, v) HL_X64_REMOVE_OFFSETS(type, v)
#define HL_CREATE_OBJECT(type, v) v = (type*)malloc(sizeof(type)); HL_INIT_OBJECT(type, v)
#define HL_DESTROY_OBJECT(type, v) HL_CLEANUP_OBJECT(type, v); free(v)

#define HL_INIT_ARR(type, v, count) HL_X64_ADD_OFFSETS_ARR(type, v, count)
#define HL_CREATE_ARR(type, v, count) v = (type*)malloc(sizeof(type) * count);\
    HL_INIT_ARR(type, v, count)

#define HL_DESTROY_ARR(type, v, count) HL_X64_REMOVE_OFFSETS_ARR(type, v, count); free(v)

#define HL_INIT_OBJECT32(type, off) HL_INIT_OBJECT(type, HL_GETPTR32(type, off))
#define HL_CREATE_OBJECT32(type, off) { type* hl_internal_ptr;\
    HL_CREATE_OBJECT(type, hl_internal_ptr);\
    off = HL_ADDPTR32(hl_internal_ptr); }

#define HL_DESTROY_OBJECT32(type, off) { type* hl_internal_ptr = HL_GETPTR32(type, off);\
    HL_DESTROY_OBJECT(type, hl_internal_ptr); }

#define HL_INIT_OBJECT64(type, off) HL_INIT_OBJECT(type, HL_GETPTR64(type, off))
#define HL_CREATE_OBJECT64(type, off) HL_CREATE_OBJECT(type, HL_GETPTR64(off))
#define HL_DESTROY_OBJECT64(type, off) HL_DESTROY_OBJECT(type, HL_GETPTR64(type, off))

#ifdef x86
#define HL_INIT_ARR32(type, arr)
#define HL_INIT_ARR64(type, arr)
#elif x64
#define HL_INIT_ARR32(type, arr) { type* hl_internal_ptr = HL_GETPTR32(type, arr.Offset);\
    HL_INIT_ARR(type, hl_internal_ptr, arr.Count); }

#define HL_INIT_ARR64(type, arr) { type* hl_internal_ptr = HL_GETPTR64(type, arr.Offset);\
    HL_INIT_ARR(type, hl_internal_ptr, arr.Count); }
#endif

#define HL_CREATE_ARR32(type, arr, count) { type* hl_internal_ptr;\
    HL_CREATE_ARR(type, hl_internal_ptr, count);\
    HL_SETPTR32(arr.Offset, hl_internal_ptr); arr.Count = count; }

#define HL_DESTROY_ARR32(type, arr) { type* hl_internal_ptr = HL_GETPTR32(type, arr.Offset);\
    HL_DESTROY_ARR(type, hl_internal_ptr, arr.Count); }

#define HL_CREATE_ARR64(type, arr, count) { type* hl_internal_ptr;\
    HL_CREATE_ARR(type, hl_internal_ptr, count);\
    HL_SETPTR64(arr.Offset, hl_internal_ptr); arr.Count = count; }

#define HL_DESTROY_ARR64(type, arr) { type* hl_internal_ptr = HL_GETPTR64(type, arr.Offset);\
    HL_DESTROY_ARR(type, hl_internal_ptr, arr.Count); }

#ifndef __cplusplus
// C Offset/Array Macros
#define HL_OFF32(type) hl_DataOff32
#define HL_OFF64(type) hl_DataOff64
#define HL_ARR32(type) hl_ArrOff32
#define HL_ARR64(type) hl_ArrOff64
#else
}

#include "HedgeLib/Managed/Endian.h"

namespace HedgeLib
{
    // C++ DataOffset template classes
    template<typename T>
    class DataOffset32
    {
        hl_DataOff32 off;

    public:
        constexpr DataOffset32() = default;
        constexpr DataOffset32(hl_DataOff32 off) : off(off) {}

        inline const T* Get()
        {
            return HL_GETPTR32(const T, off);
        }

        inline T* Get()
        {
            return HL_GETPTR32(T, off);
        }

        inline void Set(uintptr_t ptr)
        {
            HL_SETPTR32(off, ptr);
        }

        inline operator const hl_DataOff32&() const
        {
            return off;
        }

        inline operator hl_DataOff32&()
        {
            return off;
        }

        inline operator const T*() const
        {
            return HL_GETPTR32(const T, off);
        }

        inline operator T*()
        {
            return HL_GETPTR32(T, off);
        }

        inline const T* operator*() const
        {
            return HL_GETPTR32(const T, off);
        }

        inline T* operator*()
        {
            return HL_GETPTR32(T, off);
        }

        inline const T* operator->() const
        {
            return HL_GETPTR32(const T, off);
        }

        inline T* operator->()
        {
            return HL_GETPTR32(T, off);
        }

        HL_INLN_ENDIAN_SWAP_RECURSIVE_CPP()
        {
            SwapRecursive(isBigEndian, *Get());
        }
    };

    template<typename T>
    class DataOffset64
    {
        hl_DataOff64 off;

    public:
        constexpr DataOffset64() = default;
        constexpr DataOffset64(hl_DataOff64 off) : off(off) {}
        inline DataOffset64(T* ptr) : off(HL_GETOFF64(ptr)) {}

        inline const T* Get() const
        {
            return HL_GETPTR64(const T, off);
        }

        inline T* Get()
        {
            return HL_GETPTR64(T, off);
        }

        inline void Set(uintptr_t ptr)
        {
            HL_SETPTR64(off, ptr);
        }

        inline operator const hl_DataOff64&() const
        {
            return off;
        }

        inline operator hl_DataOff64&()
        {
            return off;
        }

        inline operator const T* () const
        {
            return HL_GETPTR64(const T, off);
        }

        inline operator T* ()
        {
            return HL_GETPTR64(T, off);
        }

        inline const T* operator*() const
        {
            return HL_GETPTR64(const T, off);
        }

        inline T* operator*()
        {
            return HL_GETPTR64(T, off);
        }

        inline const T* operator->() const
        {
            return HL_GETPTR64(const T, off);
        }

        inline T* operator->()
        {
            return HL_GETPTR64(T, off);
        }

        HL_INLN_ENDIAN_SWAP_RECURSIVE_CPP()
        {
            SwapRecursive(isBigEndian, *Get());
        }
    };

#ifdef x64
    inline void x64AddAbsPtrs32(hl_DataOff32& value)
    {
        value = hl_x64AddAbsPtr32(0);
    }

    template<typename... Args>
    inline void x64AddAbsPtrs32(hl_DataOff32& value, Args& ... args)
    {
        x64AddAbsPtrs32(value);
        x64AddAbsPtrs32(args...);
    }

    inline void x64RemoveAbsPtrs32(hl_DataOff32 value)
    {
        hl_x64RemoveAbsPtr32(value);
    }

    template<typename... Args>
    inline void x64RemoveAbsPtrs32(hl_DataOff32 value, Args& ... args)
    {
        x64RemoveAbsPtrs32(value);
        x64RemoveAbsPtrs32(args...);
    }
#endif

// C++ Offset/Array Macros
#define HL_OFF32(type) HedgeLib::DataOffset32<type>
#define HL_OFF64(type) HedgeLib::DataOffset64<type>

// TODO: Use template in C++ variant?
#define HL_ARR32(type) hl_ArrOff32
#define HL_ARR64(type) hl_ArrOff64
}
#endif

// String macros
#define HL_STR32 HL_OFF32(char)
#define HL_STR64 HL_OFF64(char)

// C++ x64 macros
#if defined(x64) && defined(__cplusplus)
#define HL_INLN_X64_ADD_OFFSETS_CPP(...) inline void x64AddAbsPtrs32() { \
    HedgeLib::x64AddAbsPtrs32(__VA_ARGS__); }

#define HL_INLN_X64_REMOVE_OFFSETS_CPP(...) inline void x64RemoveAbsPtrs32() { \
    HedgeLib::x64RemoveAbsPtrs32(__VA_ARGS__); }

#define HL_INLN_X64_OFFSETS_CPP(...) HL_INLN_X64_ADD_OFFSETS_CPP(__VA_ARGS__) \
    HL_INLN_X64_REMOVE_OFFSETS_CPP(__VA_ARGS__)

#define HL_DECL_X64_OFFSETS(type) HL_DECL_X64_ADD_OFFSETS(type); HL_DECL_X64_REMOVE_OFFSETS(type)
#define HL_IMPL_X64_OFFSETS(type) HL_IMPL_X64_ADD_OFFSETS(type, v) { v->x64AddAbsPtrs32(); } \
    HL_IMPL_X64_REMOVE_OFFSETS(type, v) { v->x64RemoveAbsPtrs32(); }
#else
#define HL_INLN_X64_ADD_OFFSETS_CPP(...)
#define HL_INLN_X64_REMOVE_OFFSETS_CPP(...)
#define HL_INLN_X64_OFFSETS_CPP(...)
#define HL_DECL_X64_OFFSETS(type)
#define HL_IMPL_X64_OFFSETS(type)
#endif

// Size asserts
#ifdef x64
HL_STATIC_ASSERT_SIZE(uintptr_t, 8);
HL_STATIC_ASSERT_SIZE(void*, 8);
HL_STATIC_ASSERT_SIZE(int*, 8);
#elif x86
HL_STATIC_ASSERT_SIZE(uintptr_t, 4);
HL_STATIC_ASSERT_SIZE(void*, 4);
HL_STATIC_ASSERT_SIZE(int*, 4);
#endif

HL_STATIC_ASSERT_SIZE(HL_OFF32(void), 4);
HL_STATIC_ASSERT_SIZE(HL_OFF32(int), 4);
HL_STATIC_ASSERT_SIZE(HL_OFF64(void), 8);
HL_STATIC_ASSERT_SIZE(HL_OFF64(int), 8);
