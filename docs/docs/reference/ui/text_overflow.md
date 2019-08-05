path: tree/master/framework/ui/include/bdn/ui
source: TextOverflow.h

# TextOverflow

Represents the way text views (e.g. [Label](label.md)) handle cases where the text does not fit inside the view's geometry.

## Definition

```c++
namespace bdn::ui {
    enum class TextOverflow
    {
        EllipsisHead,
        EllipsisTail,
        EllipsisMiddle,
        Clip,
        ClipWord
    };
}
```

## Enum Values

* **TextOverflow::EllipsisHead**

	The text is shortened such that the end fits in the container.
	The overflow is replaced with an ellipsis glyph.

* **TextOverflow::EllipsisTail**

	The text is shortened such that the beginning fits in the container.
	The overflow is replaced with an ellipsis glyph.

* **TextOverflow::EllipsisMiddle**

	The text is shortened such that the beginning and end fits in the container.
	The overflow is replaced with an ellipsis glyph.

* **TextOverflow::Clip**

	The text is shortened such that the beginning fits in the container. 
	The overflow is clipped.	

* **TextOverflow::ClipWord**

	The text is shortened such that the beginning fits in the container.
	The text is clipped at the start of the first word that does not fully fit in the container.
