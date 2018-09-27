#ifndef BDN_StdioUiProvider_H_
#define BDN_StdioUiProvider_H_

#include <bdn/IUiProvider.h>
#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/StdioTextUi.h>

namespace bdn
{

    /** IUiProvider implementation that provides uses stdio streams to
       communicate with the user

        The current implementation does not provide any bdn::View
       implementations. But it does support ITextUi (see getTextUi() ).
    */
    template <typename CharType>
    class StdioUiProvider : public Base, BDN_IMPLEMENTS IUiProvider
    {
      public:
        StdioUiProvider(std::basic_istream<CharType> *pInStream,
                        std::basic_ostream<CharType> *pOutStream,
                        std::basic_ostream<CharType> *pErrStream)
        {
            _pTextUi = newObj<StdioTextUi<CharType>>(pInStream, pOutStream,
                                                     pErrStream);
        }

        String getName() const { return "stdio"; }

        P<IViewCore> createViewCore(const String &coreTypeName, View *pView)
        {
            throw ViewCoreTypeNotSupportedError(coreTypeName);
        }

        P<ITextUi> getTextUi() { return _pTextUi; }

      private:
        P<StdioTextUi<CharType>> _pTextUi;
    };
}

#endif
