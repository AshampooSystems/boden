
#include <bdn/java/wrapper/Class.h>

#include <bdn/java/Env.h>

namespace bdn::java::wrapper
{
    String Class::nameInSlashNotationToSignature_(const String &nameInSlashNotation)
    {
        if (cpp20::ends_with(nameInSlashNotation, "[]")) {
            return "[" +
                   nameInSlashNotationToSignature_(nameInSlashNotation.substr(0, nameInSlashNotation.length() - 2));
        }
        {
            return "L" + nameInSlashNotation + ";";
        }
    }

    Reference Class::findClass_(const String &nameInSlashNotation)
    {
        Env &env = Env::get();

        // FindClass wants the name in slash notation UNLESS it is an array.
        // For arrays it wants the type signature.

        String findArg = nameInSlashNotation;
        if (cpp20::ends_with(findArg, "[]")) {
            findArg = nameInSlashNotationToSignature_(nameInSlashNotation);
        }

        jclass clazz = env.getJniEnv()->FindClass(findArg.c_str());

        return Reference::convertAndDestroyOwnedLocal((jobject)clazz);
    }

    Reference Class::_newObject(jclass cls, jmethodID methodId, ...)
    {
        Env &env = Env::get();

        va_list argList;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        va_start(argList, methodId);

        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        jobject result = env.getJniEnv()->NewObjectV(cls, methodId, argList);

        va_end(argList); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        return Reference::convertAndDestroyOwnedLocal(result);
    }
}
