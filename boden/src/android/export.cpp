#include <bdn/JavaConnector.h>
#include <bdn/App.h>
#include <bdn/Button.h>

#include <jni.h>



extern "C" JNIEXPORT void JNICALL Java_com_example_hazard_hello_MainActivity_initApp( JNIEnv* env, jobject obj, jobject mainViewGroupObj)
{
    bdn::JavaConnector* pConnector = bdn::JavaConnector::get();

    pConnector->setEnv(env);
    pConnector->setMainActivityObject(obj);
    pConnector->setMainViewGroupObject(mainViewGroupObj);

    try
    {
        bdn::App* pApp = bdn::App::get();
        pApp->init();
    }
    catch(std::exception& e)
    {
        pConnector->convertToJavaException(e);
        return;
    }
}


extern "C" JNIEXPORT void JNICALL Java_com_example_hazard_hello_ClickListenerProxy_deliverClickEvent( JNIEnv* env, jobject obj, jobject wrappedButtonObject)
{
    bdn::JavaConnector* pConnector = bdn::JavaConnector::get();

    pConnector->setEnv(env);

    try
    {
        bdn::Button*         pButton = (bdn::Button*)pConnector->unwrapPointer(wrappedButtonObject);

        bdn::ClickEvent evt(pButton);

        pButton->getClickEventSource()->deliver(evt);
    }
    catch(std::exception& e)
    {
        pConnector->convertToJavaException(e);
        return;
    }
}




