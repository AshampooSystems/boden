#pragma once

#include <bdn/String.h>

#include <map>
#include <functional>
#include <optional>

namespace bdn
{
    template <class ConstructionBaseClass, class... ConstructionArguments> class Factory
    {
      public:
        using ConstructionFunction = std::function<ConstructionBaseClass(ConstructionArguments...)>;

        template <class ActualType> static ConstructionBaseClass instantiate(ConstructionArguments... args)
        {
            return ActualType(args...);
        }

        template <class ActualType> static ConstructionBaseClass instantiateNew(ConstructionArguments... args)
        {
            return new ActualType(args...);
        }

        bool registerConstruction(String name, ConstructionFunction func)
        {
            if (_constructionFunctions.find(name) != _constructionFunctions.end()) {
                return false;
            }

            _constructionFunctions[name] = func;
            return true;
        }

        std::optional<ConstructionBaseClass> create(String name, ConstructionArguments... args)
        {
            auto it = _constructionFunctions.find(name);
            if (it != _constructionFunctions.end()) {
                return it->second(args...);
            }

            return std::nullopt;
        }

      private:
        std::map<String, ConstructionFunction> _constructionFunctions;
    };
    /*
        namespace test
        {
            class BaseTest
            {
              protected:
                std::string s;

              public:
                void test() { std::cout << ": " << s << std::endl; }
            };

            class A : public BaseTest
            {
              public:
                A() { s = "A"; }
            };

            class B : public BaseTest
            {
              public:
                B() { s = "B"; }

                void init() { s = "B-Initted"; };
            };

            inline void testObject()
            {
                Factory<BaseTest> testFactory;
                testFactory.registerConstruction("A", &Factory<BaseTest>::instantiate<A>);
                testFactory.registerConstruction("B", &Factory<BaseTest>::instantiate<B>);

                testFactory.create("A")->test();
                testFactory.create("B")->test();
            }

            inline void testNew()
            {
                Factory<BaseTest *> testFactory;
                testFactory.registerConstruction("A", &Factory<BaseTest *>::instantiateNew<A>);
                testFactory.registerConstruction("B", &Factory<BaseTest *>::instantiateNew<B>);

                (*testFactory.create("A"))->test();
                (*testFactory.create("B"))->test();
            }

            inline void testSharedPtr()
            {
                Factory<std::shared_ptr<BaseTest>> testFactory;

                testFactory.registerConstruction("A", &std::make_shared<A>);
                testFactory.registerConstruction("B", &std::make_shared<B>);

                testFactory.registerConstruction("C", []() {
                    auto p = std::make_shared<B>();
                    p->init();
                    return p;
                });

                (*testFactory.create("A"))->test();
                (*testFactory.create("B"))->test();
                (*testFactory.create("C"))->test();
            }

            inline void testFactory()
            {
                testObject();
                testNew();
                testSharedPtr();
            }
        }
    */
}
