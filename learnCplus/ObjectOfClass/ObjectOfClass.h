#pragma once

#ifdef OBJECTOFCLASS_API
#define OBJECTOFCLASS_API __declspec(dllexport)
#else
#define OBJECTOFCLASS_API __declspec(dllimport)
#endif

OBJECTOFCLASS_API void fun();