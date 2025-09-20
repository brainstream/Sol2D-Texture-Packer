#pragma once

#ifdef _WIN32
#   ifdef __S2TP_LIB
#   define S2TP_EXPORT __declspec(dllexport)
#   else
#   define S2TP_EXPORT __declspec(dllimport)
#   endif
#else
#   define S2TP_EXPORT
#endif


