#pragma once

#include <bdn/TextView.h>
#include <bdn/test/TestViewCore.h>
#include <bdn/TextView.h>

namespace bdn
{
    namespace test
    {

        /** Helper for tests that verify ITextViewCore implementations.*/
        class TestTextViewCore : public TestViewCore<TextView>
        {

          protected:
            std::shared_ptr<View> createView() override
            {
                std::shared_ptr<TextView> textView = std::make_shared<TextView>();

                textView->text = ("hello world");

                return textView;
            }

            void setView(std::shared_ptr<View> view) override
            {
                TestViewCore::setView(view);

                _textView = std::dynamic_pointer_cast<TextView>(view);
            }

            /** Returns true if the text view implementation will never return a
               preferred width that exceeds the available width, even if the
               text cannot be wrapped to make it smaller than the available
               width.

                Note that the preferred behaviour is for the text view to report
               a bigger preferred size if the text cannot be wrapped to fit the
               available width. I.e. the preferred behaviour is the one that
               corresponds to clipsPreferredWidthToAvailableWidth returning
               false.

                The default implementation returns false.
                */
            virtual bool clipsPreferredWidthToAvailableWidth() const { return false; }

            /** Returns true if the text view implementation will wrap at
               character boundaries if a single word does not fit in the
               available space. The preferred behaviour is to NOT wrap at
               character boundaries and instead clip the word.

                The default implementation returns false.
                */
            virtual bool wrapsAtCharacterBoundariesIfWordDoesNotFit() const { return false; }

            /** Returns true if the text view implementation uses all the
               available width when the text is wrapped (i.e. if the
               availableWidth is smaller than the text view's unconstrained
               width).

                The preferred behaviour would be for the text view to use the
               width that is actually needed for the wrapped	text (which
               usually ends up being slightly smaller than availableWidth). But
               some implementations cannot do that and will always return
               exactly availableWidth if the availableWidth is smaller than the
               unconstrained width.

                The default implementation returns false.
                */
            virtual bool usesAllAvailableWidthWhenWrapped() const { return false; }

            void runInitTests() override
            {
                TestViewCore::runInitTests();

                SECTION("text")
                {
                    _textView->text = ("helloworld");
                    initCore();
                    verifyCoreText();
                }
            }

            void testPreferredSize(int availableHeight)
            {
                // note that the available height should have NO influence on
                // the result, since the text cannot be made smaller in height
                // (only in width by line-breaking)
                std::shared_ptr<TestTextViewCore> self =
                    std::dynamic_pointer_cast<TestTextViewCore>(shared_from_this());

                SECTION("wider text causes wider preferred size")
                {
                    _textView->text = ("");

                    CONTINUE_SECTION_WHEN_IDLE(self, availableHeight)
                    {
                        Size prefSizeBefore =
                            self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                        self->_textView->text = ("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self, prefSizeBefore, availableHeight)
                        {
                            Size prefSize =
                                self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                            // width must increase with a bigger text
                            REQUIRE(prefSize.width > prefSizeBefore.width);

                            // note that the height might or might not increase.
                            // But it cannot be smaller.
                            REQUIRE(prefSize.height >= prefSizeBefore.height);

                            // when we go back to the same text as before then
                            // the preferred size should also be the same again
                            self->_textView->text = ("");

                            CONTINUE_SECTION_WHEN_IDLE(self, prefSizeBefore, availableHeight)
                            {
                                REQUIRE(self->_textView->calcPreferredSize(
                                            Size(Size::componentNone(), availableHeight)) == prefSizeBefore);
                            };
                        };
                    };
                }

                SECTION("linebreaks cause multiline")
                {
                    _textView->text = ("");

                    CONTINUE_SECTION_WHEN_IDLE(self, availableHeight)
                    {
                        Size emptyTextPreferredSize =
                            self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                        self->_textView->text = ("hello");

                        CONTINUE_SECTION_WHEN_IDLE(self, emptyTextPreferredSize, availableHeight)
                        {
                            Size prefSizeBefore =
                                self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                            // we put a little less text in the first line. On
                            // some systems the text view will allocate space
                            // for the linebreak at the end of the first line,
                            // which would cause our width test below to fail.
                            // So to avoid that we make the first line
                            // considerably shorter, thus causing the whole
                            // width of the text view to be measured according
                            // to the second line (which should have exactly the
                            // same width as the single line above).
                            self->_textView->text = ("he\nhello");

                            CONTINUE_SECTION_WHEN_IDLE(self, prefSizeBefore, availableHeight, emptyTextPreferredSize)
                            {
                                Size prefSize =
                                    self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                                // should have same width as before (since both
                                // lines have the same texts).
                                REQUIRE(prefSize.width == prefSizeBefore.width);

                                // should have roughly twice the height as
                                // before. Note that the exact height can vary
                                // because the line spacing can be different.

                                REQUIRE(prefSize.height >= prefSizeBefore.height * 1.8);
                                REQUIRE(prefSize.height < prefSizeBefore.height * 3);

                                // when we go back to empty text then we should
                                // get the original size
                                self->_textView->text = ("");

                                CONTINUE_SECTION_WHEN_IDLE(self, availableHeight, emptyTextPreferredSize)
                                {
                                    REQUIRE(self->_textView->calcPreferredSize(Size(
                                                Size::componentNone(), availableHeight)) == emptyTextPreferredSize);
                                };
                            };
                        };
                    };
                }

                SECTION("CRLF same as LF")
                {
                    _textView->text = ("hello world\nbla");

                    CONTINUE_SECTION_WHEN_IDLE(self, availableHeight)
                    {
                        Size sizeLF = self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                        self->_textView->text = ("hello world\r\nbla");

                        CONTINUE_SECTION_WHEN_IDLE(self, sizeLF, availableHeight)
                        {
                            Size sizeCRLF =
                                self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                            REQUIRE(sizeLF == sizeCRLF);
                        };
                    };
                }

                SECTION("availableWidth has no effect if bigger than "
                        "unconstrained width")
                {
                    _textView->text = ("hello world");

                    CONTINUE_SECTION_WHEN_IDLE(self, availableHeight)
                    {
                        Size unconstrainedSize =
                            self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                        REQUIRE(self->_textView->calcPreferredSize(
                                    Size(unconstrainedSize.width + 10, availableHeight)) == unconstrainedSize);
                    };
                }

                SECTION("availableWidth has no effect if equal to "
                        "unconstrained width")
                {
                    _textView->text = ("hello world");

                    CONTINUE_SECTION_WHEN_IDLE(self, availableHeight)
                    {
                        Size unconstrainedSize =
                            self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                        REQUIRE(self->_textView->calcPreferredSize(Size(unconstrainedSize.width, availableHeight)) ==
                                unconstrainedSize);
                    };
                }

                SECTION("smaller availableWidth causes word wrap")
                {
                    _textView->text = ("hellohello worldworld\nblabb");

                    CONTINUE_SECTION_WHEN_IDLE(self, availableHeight)
                    {
                        Size wrappedAtSecondPositionSize =
                            self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                        self->_textView->text = ("hellohello\nworldworld blabb");

                        CONTINUE_SECTION_WHEN_IDLE(self, wrappedAtSecondPositionSize, availableHeight)
                        {
                            Size wrappedAtFirstPositionSize =
                                self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                            self->_textView->text = ("hellohello worldworld blabb");

                            CONTINUE_SECTION_WHEN_IDLE(self, wrappedAtSecondPositionSize, wrappedAtFirstPositionSize,
                                                       availableHeight)
                            {
                                Size fullSize =
                                    self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                                REQUIRE(fullSize.width > wrappedAtSecondPositionSize.width);
                                REQUIRE(fullSize.height < wrappedAtSecondPositionSize.height);

                                // note that there might be some rounding errors
                                // with the width. So we accept 1 pixel
                                // difference
                                REQUIRE_ALMOST_EQUAL(self->_textView->calcPreferredSize(
                                                         Size(wrappedAtSecondPositionSize.width + 1, availableHeight)),
                                                     wrappedAtSecondPositionSize, Size(1, 0));

                                REQUIRE_ALMOST_EQUAL(self->_textView->calcPreferredSize(
                                                         Size(wrappedAtSecondPositionSize.width, availableHeight)),
                                                     wrappedAtSecondPositionSize, Size(1, 0));

                                if (self->usesAllAvailableWidthWhenWrapped()) {
                                    // the implementation will return exactly
                                    // the available width when text is wrapped.
                                    // Possibly with a small rounding
                                    // difference.
                                    REQUIRE_ALMOST_EQUAL(
                                        self->_textView->calcPreferredSize(
                                            Size(wrappedAtSecondPositionSize.width - 1, availableHeight)),
                                        Size(wrappedAtSecondPositionSize.width - 1, wrappedAtFirstPositionSize.height),
                                        Size(0.5, 0.5));
                                } else {
                                    REQUIRE_ALMOST_EQUAL(self->_textView->calcPreferredSize(Size(
                                                             wrappedAtSecondPositionSize.width - 1, availableHeight)),
                                                         wrappedAtFirstPositionSize, Size(1, 0));
                                }

                                REQUIRE_ALMOST_EQUAL(
                                    self->_textView
                                        ->calcPreferredSize(Size(wrappedAtFirstPositionSize.width, availableHeight))
                                        .width,
                                    wrappedAtFirstPositionSize.width, 1);
                            };
                        };
                    };
                }

                SECTION("availableWidth below single word width")
                {
                    _textView->text = ("hello");

                    CONTINUE_SECTION_WHEN_IDLE(self, availableHeight)
                    {
                        Size unrestrictedSize =
                            self->_textView->calcPreferredSize(Size(Size::componentNone(), availableHeight));

                        // specifying an available width that is smaller than
                        // the word should not reduce the preferred size.
                        if (self->clipsPreferredWidthToAvailableWidth()) {
                            // This implementation will restrict the preferred
                            // width to the available width. This is not optimal
                            // behaviour, but with some implementations it
                            // cannot be avoided. so we accept it.
                            REQUIRE_ALMOST_EQUAL(
                                self->_textView->calcPreferredSize(Size(unrestrictedSize.width - 1, availableHeight)),
                                Size(unrestrictedSize.width - 1, unrestrictedSize.height),
                                Size(1, 1)); // the implementation might round
                                             // to the nearest real pixel (which
                                             // we assume is < 1 DIP)
                        } else if (self->wrapsAtCharacterBoundariesIfWordDoesNotFit()) {
                            // the implementation will wrap at character
                            // boundaries.
                            Size size =
                                self->_textView->calcPreferredSize(Size(unrestrictedSize.width - 1, availableHeight));

                            // width should be <= the specified width
                            REQUIRE(size.width <= unrestrictedSize.width - 1);

                            // should have gotten higher since wrapping occurred
                            REQUIRE(size.height > unrestrictedSize.height);
                        } else
                            REQUIRE(self->_textView->calcPreferredSize(
                                        Size(unrestrictedSize.width - 1, availableHeight)) == unrestrictedSize);
                    };
                }
            }

            void runPostInitTests() override
            {
                std::shared_ptr<TestTextViewCore> self =
                    std::dynamic_pointer_cast<TestTextViewCore>(shared_from_this());

                TestViewCore::runPostInitTests();

                SECTION("text")
                {
                    SECTION("value")
                    {
                        _textView->text = ("helloworld");

                        CONTINUE_SECTION_WHEN_IDLE(self) { self->verifyCoreText(); };
                    }
                }

                SECTION("preferredSize")
                {
                    SECTION("availableHeight=-1")
                    testPreferredSize(-1);
                    SECTION("availableHeight=0")
                    testPreferredSize(0);
                    SECTION("availableHeight=1")
                    testPreferredSize(1);
                    SECTION("availableHeight=1000")
                    testPreferredSize(1000);
                }
            }

            /** Verifies that the text view core's text has the expected value
                (the text set in the outer TextView object's TextView::text()
               property.*/
            virtual void verifyCoreText() = 0;

            std::shared_ptr<TextView> _textView;
        };
    }
}
