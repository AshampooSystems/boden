// we must include wrapper/Object.h BEFORE our include guards.
// We need JObject to be fully defined before define JClass (since JObject is a
// base class). That alone could also be solved in another way, but we also need
// JObject to include JClass at a specific place in the Object header. And from
// that include point on JClass must be fully defined. These two constrains
// together mean that we have to always include wrapper/Object.h first and then let
// wrapper/Object.h include wrapper/Class.h at the desired position in the code.
#include <bdn/java/wrapper/Object.h>

#ifndef BDN_JAVA_JClass_H_
#define BDN_JAVA_JClass_H_

namespace bdn::java::wrapper
{
    class Class;
}

#include <bdn/java/MethodCaller.h>
#include <bdn/java/MethodId.h>
#include <bdn/java/Reference.h>
#include <bdn/java/StaticMethodCaller.h>
#include <bdn/java/TypeConversion.h>

namespace bdn::java::wrapper
{
    /** Represents a java class object.*/
    class Class : public Object
    {
      private:
        static Reference findClass_(const std::string &nameInSlashNotation);

      public:
        /** Throws a corresponding JavaException if the specified class was
         * not found.
         *
         *  @param classNameInSlashNotation the fully qualified JAVA class
         * name with slashes instead of dots. E.g. java/lang/Object instead
         * of java.lang.Object. For Java arrays (e.g. MyClass[]) pass the
         * name of the element type (in slash notation) with [] appended.
         *      */
        explicit Class(const std::string &classNameInSlashNotation) : Object(findClass_(classNameInSlashNotation))
        {
            _nameInSlashNotation = classNameInSlashNotation;
            _nameInSlashNotationInitialized = true;
        }

        explicit Class(const Reference &javaRef) : Object(javaRef) {}

        /** Returns the class object of java.lang.Class*/
        static Class &getStaticClass_()
        {
            static Class cls("java/lang/Class");

            return cls;
        }

        Class &getClass_() override { return getStaticClass_(); }

        /** Returns the canonical name of the java class, as defined by the
         * java language specification.*/
        std::string getCanonicalName()
        {
            if (!_canonicalNameInitialized) {
                static MethodId methodId;

                _canonicalName =
                    getStaticClass_().invokeObjectMethod_<std::string>(methodId, getJObject_(), "getCanonicalName");
                _canonicalNameInitialized = true;
            }

            return _canonicalName;
        }

        /** Returns the name of this class in "slash notation". I.e. slashes
         * are used instead of dots.
         *
         *  For arrays this is the name of the element type, in slash
         * notation, with [] appended.
         *  */
        std::string getNameInSlashNotation_()
        {
            if (!_nameInSlashNotationInitialized) {
                _nameInSlashNotation = getCanonicalName();
                std::transform(_nameInSlashNotation.begin(), _nameInSlashNotation.end(), _nameInSlashNotation.begin(),
                               [](char ch) {
                                   if (ch == '.') {
                                       return '/';
                                   }
                                   return ch;
                               });
                //_nameInSlashNotation.findAndReplace('.', '/');
                _nameInSlashNotationInitialized = true;
            }

            return _nameInSlashNotation;
        }

        /** Returns the java signature string that corresponds to this
         * type.*/
        std::string getSignature_() { return nameInSlashNotationToSignature_(getNameInSlashNotation_()); }

        /** Invokes the specified method of the specified class instance.
         *
         *  @param methodId a method id object that is used to cache the
         * method id to make future calls more efficient. If this is
         * uninitialized then it will automatically be initialized. If it is
         * initialized then the existing method id will be used.     *
         *  @param obj the Java object instance that the method should be
         * called on
         *  @param name the method name
         *  @param args an arbitrary number of method arguments. If a java
         * object is to be passed to the method then you should pass an
         * instance of the corresponding JObject subclass here. The type of
         * the arguments you pass is used to determine the method signature,
         * so they must match the Java-side argument types exactly.
         *  @return the return value of the method. The type is determined
         * by the template parameter ReturnType.
         *  */
        template <typename ReturnType, typename... Arguments>
        ReturnType invokeObjectMethod_(MethodId &methodId, jobject obj, const std::string &name, Arguments... args)
        {
            initMethodId<ReturnType, Arguments...>(methodId, name);

            return MethodCaller<ReturnType>::template call<Arguments...>(obj, methodId.getId(), args...);
        }

        /** Invokes the specified static method of the specified class
         * instance.
         *
         *  @param methodId a method id object that is used to cache the
         * method id to make future calls more efficient. If this is
         * uninitialized then it will automatically be initialized. If it is
         * initialized then the existing method id will be used.
         *  @param name the static method's name
         *  @param args an arbitrary number of method arguments. If a java
         * object is to be passed to the method then you should pass an
         * instance of the corresponding JObject subclass here. The type of
         * the arguments you pass is used to determine the method signature,
         * so they must match the Java-side argument types exactly.
         *  @return the return value of the method. The type is determined
         * by the template parameter ReturnType.
         *  */
        template <typename ReturnType, typename... Arguments>
        ReturnType invokeStaticMethod_(MethodId &methodId, const std::string &name, Arguments... args)
        {
            initStaticMethodId<ReturnType, Arguments...>(methodId, name);

            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            return StaticMethodCaller<ReturnType>::template call<Arguments...>((jclass)getJObject_(), methodId.getId(),
                                                                               args...);
        }

        /** Creates a new instance of the Java class.
         *
         *  @param constructorId a method id object that is used to cache
         * the constructor's id to make future calls more efficient. If this
         * is uninitialized then it will automatically be initialized. If it
         * is initialized then the existing method id will be used.
         *  @param args an arbitrary number of constructor arguments. If a
         * java object is to be passed to the constructor then you should
         * pass a JObject instance here.
         *  @return reference of the new object instance. Note that this is
         * a local reference, i.e. it will become invalid after the current
         * JNI call returns. You can call toStrong() to make the reference
         *      permanent.
         *  */
        template <typename... Arguments> Reference newInstance_(MethodId &constructorId, Arguments... args)
        {
            initMethodId<void, Arguments...>(constructorId, "<init>");

            std::list<Reference> tempObjects;

            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            return _newObject((jclass)getJObject_(), constructorId.getId(), nativeToJava(args, tempObjects)...);
        }

        template <typename ReturnType, typename... Arguments> void initMethodId(MethodId &id, const std::string &name)
        {
            if (!id.isInitialized()) {
                std::string methodSignature =
                    "(" + _makeTypeSignatureList<Arguments...>() + ")" + getTypeSignature<ReturnType>();

                id.init(*this, name, methodSignature);
            }
        }

      private:
        /** Converts a name in slash notation to the corresponding signature
         * string.*/
        static std::string nameInSlashNotationToSignature_(const std::string &nameInSlashNotation);

        template <typename Dummy> static std::string _makeTypeSignatureListImpl() { return ""; }

        template <typename Dummy, typename FirstType, typename... RemainingTypes>
        static std::string _makeTypeSignatureListImpl()
        {
            std::string firstSig = getTypeSignature<FirstType>();

            std::string remainingSigs = _makeTypeSignatureListImpl<Dummy, RemainingTypes...>();

            return firstSig + remainingSigs;
        }

        template <typename... Types> static std::string _makeTypeSignatureList()
        {
            return _makeTypeSignatureListImpl<int, Types...>();
        }

        template <typename ReturnType, typename... Arguments>
        void initStaticMethodId(MethodId &id, const std::string &name)
        {
            if (!id.isInitialized()) {
                std::string methodSignature =
                    "(" + _makeTypeSignatureList<Arguments...>() + ")" + getTypeSignature<ReturnType>();

                id.initStatic(*this, name, methodSignature);
            }
        }

        template <typename T> static std::string getTypeSignature()
        {
            return TypeConversion<typename std::decay<T>::type>::getJavaSignature();
        }

        static Reference _newObject(jclass cls, jmethodID methodId, ...);

        mutable std::string _nameInSlashNotation;
        mutable bool _nameInSlashNotationInitialized = false;
        mutable std::string _canonicalName;
        mutable bool _canonicalNameInitialized = false;
    };
}
#endif
