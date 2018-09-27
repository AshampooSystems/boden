#ifndef BDN_AsyncOpRunnable_H_
#define BDN_AsyncOpRunnable_H_

#include <bdn/IAsyncOp.h>
#include <bdn/IThreadRunnable.h>
#include <bdn/RequireNewAlloc.h>

namespace bdn
{

    template <class ResultType> class AsyncOpResultHelper_ : public Base
    {
      public:
        static void doOpAndInitResult(ResultType *&pResult,
                                      std::function<ResultType()> opFunc)
        {
            pResult = new ResultType(opFunc());
        }

        static ResultType getResultValue(ResultType *pResult)
        {
            return *pResult;
        }

        static void deleteResult(ResultType *pResult) { delete pResult; }
    };

    template <> class AsyncOpResultHelper_<void> : public Base
    {
      public:
        static void doOpAndInitResult(void *&pResult,
                                      std::function<void()> opFunc)
        {
            opFunc();
        }

        static void getResultValue(void *pResult)
        {
            // do nothing.
        }

        static void deleteResult(void *pResult)
        {
            // do nothing
        }
    };

    /** A helper for implementing an asynchronous operation.

        Instances of AsyncOpRunnable must be allocated with newObj or new.

        This creates a runnable object (IThreadRunnable) that implements the
       IAsyncOp interface. The runnable object can then be executed by a thread
       or thread pool.

        IAsyncOp can be used to control / abort the operation from other parts
       of the program. It is also used to register notification handlers that
       are executed when the operation is done and to retrieve results ( see
       onDone() ).

        You use this by deriving a class from AsyncOpRunnable and overriding
       doOp().

        Note that run() must be called at some point to actually perform the
       work. The easiest way to do this is to pass the AsyncOpRunnable object to
       a Thread or ThreadPool object to execute.

        The default implementation only implements the abort functionality (see
       signalStop()) for cases when signalStop is called before doOp() is
       started. If you want to support stopping during doOp then your doOp
       implementation must call isStopSignalled() regularly and abort with an
       AbortedError exception when it returns true.
    */
    template <class ResultType>
    class AsyncOpRunnable
        : public RequireNewAlloc<Base, AsyncOpRunnable<ResultType>>,
          BDN_IMPLEMENTS IAsyncOp<ResultType>,
          BDN_IMPLEMENTS IThreadRunnable
    {
      public:
        AsyncOpRunnable()
        {
            _pDoneNotifier =
                newObj<OneShotStateNotifier<P<IAsyncOp<ResultType>>>>();
        }

        ~AsyncOpRunnable()
        {
            if (_pResult != nullptr)
                AsyncOpResultHelper_<ResultType>::deleteResult(_pResult);
        }

        ResultType getResult() const override
        {
            if (!isDone())
                throw UnfinishedError();

            if (_error)
                std::rethrow_exception(_error);

            return AsyncOpResultHelper_<ResultType>::getResultValue(_pResult);
        }

        void signalStop() override
        {
            bool actuallyAborted = false;

            {
                Mutex::Lock lock(_mutex);

                _stopSignalled = true;

                // we cannot abort the operation when it is already in progress
                if (!_started && !_abortedBeforeStart) {
                    // not started yet. Mark as aborted and set the result
                    _abortedBeforeStart = true;
                    actuallyAborted = true;

                    _error = std::make_exception_ptr(AbortedError());
                }
            }

            if (actuallyAborted)
                setDone();
        }

        bool isDone() const override
        {
            Mutex::Lock lock(_mutex);

            return _done;
        }

        /** Performs the actual operation.

            Note that run() will not let exceptions thrown by doOp through.
            Any exception that occurs is stored and will be re-thrown when
           getResult() is called.*/
        void run() override
        {
            {
                Mutex::Lock lock(_mutex);
                if (_abortedBeforeStart) {
                    // aborted before we were started => do nothing
                    return;
                }

                // mark as started - from this point on aborting is not possible
                // anymore.
                _started = true;
            }

            try {
                AsyncOpResultHelper_<ResultType>::doOpAndInitResult(
                    _pResult, plainMethod(this, &AsyncOpRunnable::doOp));
            }
            catch (...) {
                _error = std::current_exception();
            }

            setDone();
        }

        IAsyncNotifier<P<IAsyncOp<ResultType>>> &onDone() const override
        {
            return *_pDoneNotifier;
        }

      protected:
        /** Override this in derived classes. This should perform the actual
         * synchronous operation.
         */
        virtual ResultType doOp() = 0;

        /** Returns true if signalStop() has been called, i.e. if the operation
           was asked to abort. This can be used by the doActualWork()
           implementation to detect when it should abort. That would allow the
           operation to be aborted while it is in progress.*/
        bool isStopSignalled()
        {
            Mutex::Lock lock(_mutex);
            return _stopSignalled;
        }

      private:
        void setDone()
        {
            {
                Mutex::Lock lock(_mutex);
                _done = true;
            }

            _pDoneNotifier->postNotification(this);
        }

        class DummySubscription : public Base,
                                  BDN_IMPLEMENTS INotifierSubscription
        {
          public:
        };

        mutable Mutex _mutex;
        bool _stopSignalled = false;
        bool _abortedBeforeStart = false;
        bool _started = false;
        bool _done = false;
        P<OneShotStateNotifier<P<IAsyncOp<ResultType>>>> _pDoneNotifier;

        std::exception_ptr _error;
        ResultType *_pResult = nullptr;
    };
}

#endif
