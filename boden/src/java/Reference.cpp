#include <bdn/init.h>
#include <bdn/java/Reference.h>

#include <bdn/java/Env.h>


namespace bdn
{
namespace java
{

Reference Reference::toStrong() const
{
    if( _pShared->getType()==Type::strong)
        return Reference(*this);
    else
        return Reference(Type::strong, Env::get().getJniEnv()->NewGlobalRef( _pShared->getJObject() ) );
}


Reference Reference::toWeak() const
{
    if( _pShared->getType()==Type::weak)
        return Reference(*this);
    else
        return Reference(Type::weak, Env::get().getJniEnv()->NewWeakGlobalRef( _pShared->getJObject() ) );
}


bool Reference::isNull() const
{
    return (_pShared->getType()==Type::invalid
            || Env::get().getJniEnv()->IsSameObject( _pShared->getJObject(), NULL ) );
}

/** Returns true if this reference points to the same object as the other reference.*/
bool Reference::operator==(const Reference & o) const
{
    return Env::get().getJniEnv()->IsSameObject( _pShared->getJObject(), o._pShared->getJObject() );
}



Reference::Shared::~Shared()
{
    JNIEnv* pEnv = Env::get().getJniEnv();

    // note that externalLocal references must NEVER be deleted. They are under the control of
    // the java code. If we delete them then a crash may occur.
    if(_type==Type::ownedLocal)        
        pEnv->DeleteLocalRef( _ref );

    else if(_type==Type::strong)
        pEnv->DeleteGlobalRef( _ref );

    else if(_type==Type::weak)
        pEnv->DeleteWeakGlobalRef( _ref );
}


}
}


