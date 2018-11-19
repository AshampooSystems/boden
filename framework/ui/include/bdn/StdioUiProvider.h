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
    template <typename CharType> class StdioUiProvider : public Base, BDN_IMPLEMENTS IUiProvider
    {
      public:
        StdioUiProvider(std::basic_istream<CharType> *inStream, std::basic_ostream<CharType> *outStream,
                        std::basic_ostream<CharType> *errStream)
        {
            _textUi = newObj<StdioTextUi<CharType>>(inStream, outStream, errStream);
        }

        String getName() const { return "stdio"; }

        P<IViewCore> createViewCore(const String &coreTypeName, View *view)
        {
            throw ViewCoreTypeNotSupportedError(coreTypeName);
        }

        P<ITextUi> getTextUi() { return _textUi; }

      private:
        P<StdioTextUi<CharType>> _textUi;
    };
}

#endif
