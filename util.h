#pragma once

#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;

/*often used macros and functions*/

/*release Dx11 COMs*/
#define DXRelease(x) { if(x && x != nullptr){x->Release(); x = 0; }}


/*print to vs output, only in debug mode*/
#ifdef _DEBUG
#define DBOUT( s )           \
{                            \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}
#else
#define DBOUT(s)
#endif

/*assertion, only debug mode*/

#ifdef _DEBUG

#define ASSERT(expr) \
    if(expr) { }\
    else\
    {\
        std::wostringstream os_;\
        os_ << "Assertion failed: " << #expr << " in " << __FILEW__ << " at line: " << __LINE__ <<endl;\
        OutputDebugString( os_.str().c_str() );\
        DebugBreak();\
    }
#else 
#define ASSERT(expr) //nothing
#endif


class DXMath
{
public:
    static XMMATRIX InverseTranspose(CXMMATRIX M)
    {
        // Inverse-transpose is just applied to normals.  So zero out 
        // translation row so that it doesn't get into our inverse-transpose
        // calculation--we don't want the inverse-transpose of the translation.
        XMMATRIX A = M;
        A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        XMVECTOR det = XMMatrixDeterminant(A);
        return XMMatrixTranspose(XMMatrixInverse(&det, A));
    }

    template<typename T>
    static T Min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    static T Max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }

    template<typename T>
    static T Lerp(const T& a, const T& b, float t)
    {
        return a + (b - a) * t;
    }

    template<typename T>
    static T Clamp(const T& x, const T& low, const T& high)
    {
        return x < low ? low : (x > high ? high : x);
    }

    // Returns random float in [0, 1).
    static float RandF()
    {
        return (float)(rand()) / (float)RAND_MAX;
    }

    // Returns random float in [a, b).
    static float RandF(float a, float b)
    {
        return a + RandF() * (b - a);
    }

};