#pragma once
//!< [HLSL Scalar Types](https://github.com/microsoft/DirectXShaderCompiler/wiki/16-Bit-Scalar-Types#dxil-scalar-type)

// note: 16-bit型はhlslのcompile時のversionが"-HV 2018(s_6_2)"以上でないと使えないので注意.

//-----------------------------------------------------------------------------------------
// integral types
//-----------------------------------------------------------------------------------------

typedef int16_t int16_t;
typedef int32_t int32_t; //!< intと同等.
typedef int64_t int64_t;

typedef uint16_t uint16_t;
typedef uint32_t uint32_t; //!< uintと同等.
typedef uint64_t uint64_t;

//-----------------------------------------------------------------------------------------
// floating-point types
//-----------------------------------------------------------------------------------------

typedef float16_t float16_t; //!< halfと同等.
typedef float32_t float32_t; //!< floatと同等.
typedef float64_t float64_t; //!< doubleと同等.
