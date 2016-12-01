package io.boden.android;

import io.boden.java.NativeRunnable;
import io.boden.java.NativeStrongPointer;

import android.os.Handler;
import android.os.Looper;
import android.os.MessageQueue;

import java.util.LinkedList;
import java.util.TreeSet;
import java.util.Timer;

/** Implements the functionality of bdn::IDispatcher on the Java side.*/
public class NativeDispatcher
{
    private class InstallIdleHandlerAction implements Runnable
    {
        InstallIdleHandlerAction(IdleHandler idleHandler)
        {
            mIdleHandler = idleHandler;
        }

        public void run()
        {
            Looper.myQueue().addIdleHandler(mIdleHandler);
        }

        private IdleHandler mIdleHandler;
    };

    public NativeDispatcher(Looper looper)
    {
        mHandler = new Handler( looper );

        // install an idle listener for the queue so that we get notified.
        mIdleHandler = new IdleHandler(mHandler);

        // Looper.getQueue was added in API level 23, which is a little too high a target
        // for us. So we use myQueue instead, which unfortunately must be called from the
        // looper thread. So we have to post an action that installs the handler
        mHandler.post( new InstallIdleHandlerAction(mIdleHandler) );
        //looper.getQueue().addIdleHandler( mIdleHandler );

        mProcessNormalQueueItemAction = new ProcessNormalQueueItemAction(this);
    }

    private class ProcessNormalQueueItemAction implements Runnable
    {
        ProcessNormalQueueItemAction(NativeDispatcher dispatcher)
        {
            mDispatcher = dispatcher;
        }

        public void run()
        {
            mDispatcher.processNormalQueueItem();
        }

        private NativeDispatcher mDispatcher;
    };


    private class ProcessTimedItemAction implements Runnable
    {
        ProcessTimedItemAction(NativeDispatcher dispatcher, NativeRunnable runnable)
        {
            mDispatcher = dispatcher;
            mRunnable = runnable;
        }

        public void run()
        {
            mRunnable.run();
            mDispatcher.mPendingProcessTimedItemActions.remove(this);
        }

        public void dispose()
        {
            mRunnable.dispose();
        }

        private NativeDispatcher mDispatcher;
        private NativeRunnable   mRunnable;
    };

    private void processNormalQueueItem()
    {
        if(!mNormalQueue.isEmpty())
        {
            NativeRunnable item = mNormalQueue.removeFirst();

            item.run();
        }
    }

    public void enqueue(double delaySeconds, NativeRunnable item, boolean idlePriority)
    {
        int delayMillis = (int)(delaySeconds*1000);

        if(idlePriority)
            mIdleHandler.enqueue(item, delayMillis);
        else
        {
            if (delayMillis <= 0)
            {
                // note that we cannot post the NativeRunnable item directly.
                // We must be able to dispose all queued items, so we must keep
                // a reference until it is run.
                // So instead we manage the queue ourselves and
                // post an action that causes processNormalQueueItem to be called.
                mHandler.post( mProcessNormalQueueItemAction );
            }
            else
            {
                // for timed items we cannot have a simple queue, because their trigger order can be complex.
                // So instead we create a custom action to process the timed item and add that to the
                // timed item set.
                ProcessTimedItemAction action = new ProcessTimedItemAction(this, item);
                mPendingProcessTimedItemActions.add(action);

                mHandler.postDelayed(action, delayMillis);
            }
        }
    }



    private class NativeTimerDispatcherThreadCaller implements Runnable
    {
        NativeTimerDispatcherThreadCaller(NativeTimerTask task)
        {
            mTask = task;
        }

        public void run()
        {
            mTask.callFromDispatcherThread();
        }

        private NativeTimerTask mTask;
    };


    private class NativeTimerTask extends java.util.TimerTask
    {
        NativeTimerTask(NativeDispatcher dispatcher, NativeStrongPointer timerData )
        {
            mDispatcher = dispatcher;
            mTimerData = timerData;
            mCaller = new NativeTimerDispatcherThreadCaller(this);

            mCallPending = false;
        }

        public void run()
        {
            // this is called from the timer thread. We want it to be called from
            // the dispatcher thread - so we have to post an action there.

            // if a call is pending then we do not schedule another call.
            if(mTimerData!=null && !mCallPending)
            {
                mCallPending = true;
                mDispatcher.mHandler.post(mCaller);
            }
        }

        void callFromDispatcherThread()
        {
            try
            {
                if(mTimerData!=null)
                {
                    if (!mDispatcher.nativeTimerEvent(mTimerData))
                        dispose();
                }
            }
            finally
            {
                mCallPending = false;
            }
        }

        public void dispose()
        {
            if(mTimerData!=null)
            {
                mTimerData.dispose();
                mTimerData = null;
                cancel();

                mDispatcher.mTimerTaskSet.remove(this);
            }
        }

        private NativeDispatcher                    mDispatcher;
        private NativeStrongPointer                 mTimerData;
        private NativeTimerDispatcherThreadCaller   mCaller;

        private boolean             mCallPending;
    };

    public void createTimer(double intervalSeconds, NativeStrongPointer timerData )
    {
        NativeTimerTask task = new NativeTimerTask( this, timerData );

        int intervalMillis = (int)(intervalSeconds*1000);
        if(intervalMillis<=0)
            intervalMillis = 1;

        if(mMasterTimer==null)
        {
            // note that each timer object has a thread running in the background to schedule the
            // events. So we only use a single timer instance for all of "our" timers.
            mMasterTimer = new Timer();
        }

        mMasterTimer.scheduleAtFixedRate( task, intervalMillis, intervalMillis);

        mTimerTaskSet.add(task);
    }

    public void dispose()
    {
        // we must release all queued runnable objects.

        // idle queue
        mIdleHandler.dispose();

        // normal queue
        for( NativeRunnable item: mNormalQueue)
            item.dispose();

        // timed items
        for( ProcessTimedItemAction action: mPendingProcessTimedItemActions)
            action.dispose();

        // timers
        while(!mTimerTaskSet.isEmpty())
        {
            NativeTimerTask task = mTimerTaskSet.first();
            task.dispose();
        }
    }


    private class IdleHandler implements MessageQueue.IdleHandler
    {
        IdleHandler( Handler handler)
        {
            mHandler = handler;
        }

        private class EnqueueAction implements Runnable
        {
            EnqueueAction(LinkedList<NativeRunnable> queue, NativeRunnable item)
            {
                mQueue = queue;
                mItem = item;
            }

            public void run()
            {
                mQueue.add(mItem);
            }

            private LinkedList<NativeRunnable> mQueue;
            private NativeRunnable             mItem;
        };


        /** Adds an item to the idle queue. This can be called from any thread.*/
        public void enqueue(NativeRunnable item, int delayMillis)
        {
            // we could lock a mutex here.
            // However, we also have the problem that we need to know if we are in idle state
            // right now. To know that we have to post a dummy item with normal priority, which
            // will force queueIdle to be called afterwards if we are in idle state.
            // Since we need to do that anyway, we can also simply use that dummy item to
            // enqueue the new item from the main thread.

            EnqueueAction action = new EnqueueAction(mQueue, item);

            if(delayMillis>0)
                mHandler.post( action );
            else
                mHandler.postDelayed( action, delayMillis );
        }


        public void dispose()
        {
            for(NativeRunnable item: mQueue)
                item.dispose();
        }


        public boolean queueIdle()
        {
            if(!mQueue.isEmpty())
            {
                NativeRunnable item = mQueue.removeFirst();

                // schedule the item with normal priority now. That will ensure
                // that queueIdle is called again afterwards, if there are no higher priority
                // items added in the meantime.
                mHandler.post(item);
            }

            return true;
        }

        private LinkedList<NativeRunnable>      mQueue;
        private Handler                         mHandler;
    };


    private Handler     mHandler;
    private IdleHandler mIdleHandler;

    private Timer       mMasterTimer;

    private LinkedList<NativeRunnable>          mNormalQueue;
    private TreeSet< ProcessTimedItemAction >   mPendingProcessTimedItemActions;
    private TreeSet< NativeTimerTask >          mTimerTaskSet;

    private ProcessNormalQueueItemAction        mProcessNormalQueueItemAction;



    private native boolean nativeTimerEvent(NativeStrongPointer timerData);
}



