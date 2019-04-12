# Base

Common base class for all Boden classes. Deriving classes can call `shared_from_this()` to obtain a `shared_ptr<Base>` instance pointing to `this`.

## Relationships

Inherits from `std::enable_shared_from_this<Base>`.