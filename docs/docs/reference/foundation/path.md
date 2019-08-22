path: tree/master/framework/foundation/include/bdn/
source: path.h

# path

Provides functions to retrieve platform-specific directory paths.

!!! note

	All functions that retrieve directory paths may return paths to directories that do not exist.
	After retrieving a path, make sure the directory exists.

## Declaration

```C++
namespace bdn::path
```

## Retrieving Directory Paths

* **std::string temporaryDirectoryPath()**

	Returns a path to the temporary (or caches) directory of the system.

* **std::string applicationSupportDirectoryPath()**

	Returns a path to the application support/application data directory for the current application.

* **std::string documentDirectoryPath()**

	Returns a path to a directory where user documents should be stored. This directory can either be application-specific (iOS, Android) or user-specific (e.g. macOS).
