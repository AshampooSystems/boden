Text streams: technical implementation background
==================================================

This section explains the technical details of how the stream classes (bdn::TextOutStream) are implemented and why
it is done this way.
Most programmers do not need to concern themselves with this - they can simply use
TextOutStream like any other standard I/O output stream (std::basic_ostream).

Most C++ standard libraries do not provide a specialization for std::basic_ostream<char32_t>,
so 32 bit unicode character streams cannot normally be used.

Because of this the Boden framework provides the implementation (specialization)
for std::basic_ostream<char32_t, UnicodeCharTraits>.

Why is the specialization for UnicodeCharTraits, rather than the normal std::char_traits<char32_t>?
---------------------------------------------------

The short answer to this is that this is necessary so that
the Boden framework can provide a standard conformant implementation for std::basic_ostream<char32_t>.
The standard only allows third party libraries to provide a specializations of templates in the "std"
namespace for their own types, not for basic types like char32_t. So to make the implementation legal,
it has to depend on a Boden type, which is UnicodeCharTraits in this case.

This also makes the implementation future proof: if the C++ standard is amended in the future and
basic_ostream<char32_t> is implemented by the standard libraries, then the Boden implementation
and the standard implementation can exist side by side.

Differences to standard library basic_ostream implementations
--------------------------------------------------

The Boden implementation of TextOutStream / std::basic_ostream<char32_t, UnicodeCharTraits> differs
in a few ways from the normal basic_ostream implementations in the standard library:

- the default multibyte encoding for the stream is UTF-8
- when multibyte (char) strings are written to the stream, they are properly decoded from the multibyte
  encoding of the stream's locale (UTF-8 with the default locale). This is in contrast to the 
  standard library streams, which cannot properly transcode any encoded multibyte characters (they simply
  cast each individual encoded byte to the stream's character type)
- strings of all character types can be written to the stream (char, wchar_t, char16_t and char32_t).
  They are properly transcoded.
- bool values are printed as words by default ("true" or "false", or the locale's translation of these when
  a custom locale is set). The standard library implementations default to printing bools as 0 or 1.
  The behaviour can be changed by setting or clearing the std::ios_base::boolalpha flag with setf
  or std::boolalpha and std::noboolalpha.
- The Boden implementation is locale independent by default (instead of the global locale it uses a variant
  of the classic locale by default). So formatting of integers etc. follows english rules by default.
  This is seen as the better default behaviour. Accidentally formatting data according to the user's locale
  can cause broken data to be generated that can not be properly parsed again. On the other hand, if a string
  is presented to the user with english formatting by mistake then that is usually not a serious problem.
  So defaulting to english is the safer alternative in general.

