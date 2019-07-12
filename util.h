#pragma once

#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;


/*often used macros and functions*/

#define DXRelease(x) { if(x){x->Release(); x = 0; }}

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