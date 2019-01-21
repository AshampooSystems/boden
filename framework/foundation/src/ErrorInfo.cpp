
#include <bdn/ErrorInfo.h>
#include <bdn/Uri.h>

#include <regex>

namespace bdn
{

    ErrorInfo::ErrorInfo(const std::exception &error)
    {
        auto sysError = dynamic_cast<const std::system_error *>(&error);

        if (sysError != nullptr) {
            initFromSystemError(*sysError);
        } else {
            initFromErrorString(error.what());
        }
    }

    ErrorInfo::ErrorInfo(const String &errorString) { initFromErrorString(errorString); }

    ErrorInfo::ErrorInfo(const std::exception_ptr &exceptionPtr)
    {
        try {
            std::rethrow_exception(exceptionPtr);
        }
        catch (std::system_error &e) {
            initFromSystemError(e);
        }
        catch (std::exception &e) {
            initFromErrorString(e.what());
        }
        catch (...) {
            initFromErrorString("Exception of unknown type.");
        }
    }

    void ErrorInfo::initFromSystemError(const std::system_error &e)
    {
        initFromErrorString(e.what());

        const std::error_code &code = e.code();

        // add the error code to the error fields
        _fields.add("bdn.code", std::to_string(code.value()));
        _fields.add("bdn.category", code.category().name());
    }

    void ErrorInfo::initFromErrorString(const String &errorString)
    {
        _message = errorString;

        std::regex e("(.*)[ ]*(\\[\\[.*\\]\\])[: ]*(.*)");
        std::smatch match;

        if (std::regex_match(_message, match, e)) {
            if (match.size() == 4) {
                std::ssub_match preMsg = match[1];
                std::ssub_match errorFields = match[2];
                std::ssub_match postMsg = match[3];

                _fields = ErrorFields(errorFields.str());

                _message = rtrim_copy(preMsg.str());
                String postStr = postMsg.str();
                if (_message.size() > 0 && postStr.size() > 0) {
                    _message += " ";
                }
                _message += postStr;
            }
        }
    }

    String ErrorInfo::toString() const
    {
        String result = _message;

        String fieldString = _fields.toString();
        if (!fieldString.empty())
            result += " " + fieldString;

        return result;
    }
}
