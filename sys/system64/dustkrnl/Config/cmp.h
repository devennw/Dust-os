#ifndef _CMP_
#define _CMP_

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4324)   // alignment sensitive to declspec
#pragma warning(disable:4127)   // condition expression is constant
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4706)   // assignment within conditional expression
#pragma warning(disable:4869)   // unreachable code
#pragma warning(disable:4054)   // Cast of function pointer to PVOID
#pragma warning(disable:4055)   // Cast of function pointer
#pragma warning(disable:4115)   // named type definition in parentheses

#define _32k (32L*1024L)
#define _64k (64L*1024L)
#define _128k (128L*1024L)
#define SIZE_IO_BUFFER _64k

#include "dustos.h"
#include "hive.h"
#include "wchar.h"
#include "zwapi.h"
#include <profiles.h>

/* bug check description and defines */
#include "cmpbug.h"
#include "kddll.h"
