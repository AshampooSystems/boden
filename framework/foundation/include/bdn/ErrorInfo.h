#ifndef BDN_ErrorInfo_H_
#define BDN_ErrorInfo_H_

#include <bdn/ErrorFields.h>

namespace bdn
{

    /** Contains information about an error / an exception.

        ErrorInfo is a helper class that is used to encode structured additional
       information
        ("fields") in error messages and extract it again later.

        Encode the fields as a string (possibly to add them to an error
       message):

        \code
        String fields = ErrorInfo::encodeFields({ {"path", filePath}
                                                  {"context", "while doing X"} }
       );

        String combinedErrorMessage = "File not found" + fields;

        \endcode

        Extract information from an exception object:

        \code
        std::exception& e = ...;

        ErrorInfo info(e);

        // get the fields as a name-value map
        const std::map<String,String>& fields = info.getFields();

        // access an individual field
        String path = info.getField("path");

        // get the original error message without the encoded fields
        String message = info.getMessage();

        \endcode
    */
    class ErrorInfo : public Base
    {
      public:
        /** Constructs an ErrorInfo object from an error string. If the error
           string contains encoded fields (see encodeFields()) then they are
           automatically extracted and are accessible via getFields() or
           getField() afterwards.

            It is ok to pass an error message without encoded field data here.
           The effect will simply be that the fields map will be empty.
            */
        ErrorInfo(const String &errorString);

        /** Constructs an ErrorInfo object from an exception object. This is
           currently the same as ErrorInfo( error.what() ), but in the future
           additional information might be extracted from certain kinds of
           exception objects. So if you have an exception object it is
           recommended
            to pass the whole object to this constructed, rather than passing
           just the error message.*/
        ErrorInfo(const std::exception &error);

        /** Constructs an ErrorInfo object from a std::exception_ptr object.
         */
        ErrorInfo(const std::exception_ptr &exceptionPtr);

        /** Returns the value of a field. If the field is not defined then an
           empty string is returned.*/
        String getField(const String &fieldName) const
        {
            return _fields.get(fieldName);
        }

        /** Returns true if the specified field exists.*/
        bool hasField(const String &fieldName) const
        {
            return _fields.contains(fieldName);
        }

        /** Returns the additional information fields as a name-value string map
           in the form of an ErrorFields object.*/
        const ErrorFields &getFields() const { return _fields; }

        /** Returns the error message without the encoded additional
         * information.*/
        String getMessage() const { return _message; }

        /** Returns a combined error string that contains all the information in
           this error info object. This is intended for diagnostic messages. Use
           getMessage() if you want to display an error message for the user.*/
        String toString() const;

      protected:
        /** Initializes the object from a string that may or may not include
            encoded error fields with additional information.*/
        void initFromErrorString(const String &errorString);

        /** Initializes the object from a std::system_error object.*/
        void initFromSystemError(const std::system_error &e);

        String _message;
        ErrorFields _fields;
    };
}

#endif