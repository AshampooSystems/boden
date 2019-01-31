#pragma once

#include <bdn/AppRunnerBase.h>

namespace bdn
{
    /**
     * @brief unhandledException Convenience function that calls
     * IAppRunner::unhandledException
     * @param canKeepRunningAfterException
     * @return The result of getAppRunner()->unhandledException()
     *
     * If there is no app runner object yet then this function does nothing.
     *
     */
    inline bool unhandledException(bool canKeepRunningAfterException)
    {
        if (auto appRunner = getAppRunner()) {
            return appRunner->unhandledException(canKeepRunningAfterException);
        }

        return false;
    }

    /**
     * @brief Marks the beginning of an entry function, i.e. a place where the
     * operating system passes control to the App.
     * @param function The function to call
     * @param canKeepRunningAfterException If true, application can continue to
     * run if an exception was caught
     * @param platformSpecific See notes
     *
     * This function is used to wrap our code whenever the operating system
     * calls into our process. If an exception is caught \ref
     * bdn::unhandledException will be called.
     *
     * \note On Android the platformSpecific parameter is a pointer to the
     * JNIEnv object that the JNI function received from the Java side.
     */
    void platformEntryWrapper(std::function<void()> function, bool canKeepRunningAfterException,
                              void *platformSpecific = nullptr);

    template <typename RETURN_TYPE>
    RETURN_TYPE nonVoidPlatformEntryWrapper(std::function<RETURN_TYPE()> function, bool canKeepRunningAfterException,
                                            void *platformSpecific = nullptr);
}