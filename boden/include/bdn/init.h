#ifndef BDN_init_H_
#define BDN_init_H_

#include <bdn/config.h>

#define BDN_IMPLEMENTS virtual public


// This is slightly complicated because of the intricacies of the
// C preprocessor. In order for the preprocessor to actually expand
// the parameters and concatenate them we need several layers of indirection.
#define BDN_APPEND_LINE_IMPL_1(dummy, finalName) finalName
#define BDN_APPEND_LINE_IMPL_2(a, b) BDN_APPEND_LINE_IMPL_1(~, a ## b)
#define BDN_APPEND_LINE_IMPL_3(a, b) BDN_APPEND_LINE_IMPL_2(a, b)


/** \def BDN_APPEND_LINE(baseName)

    Appends the line number of the source file that this is used on to the
    specified base name. This can be used to generate somewhat unique names.
    
    */
#define BDN_APPEND_LINE(baseName) BDN_APPEND_LINE_IMPL_3(baseName, __LINE__)






#include <bdn/IBase.h>
#include <bdn/P.h>
#include <bdn/Base.h>

#include <bdn/newObj.h>

#include <bdn/Mutex.h>
#include <bdn/MutexLock.h>
#include <bdn/MutexUnlock.h>

#include <bdn/SafeInit.h>

#include <bdn/String.h>

#include <bdn/ErrorImpl.h>

#include <bdn/ProgrammingError.h>
#include <bdn/InvalidArgumentError.h>
#include <bdn/OutOfRangeError.h>
#include <bdn/SystemError.h>

#include <bdn/cast.h>

#include <bdn/safeStatic.h>

#include <bdn/destruct.h>

#include <bdn/WeakP.h>

#endif
