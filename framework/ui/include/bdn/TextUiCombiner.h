#ifndef BDN_TextUiCombiner_H_
#define BDN_TextUiCombiner_H_

#include <bdn/ITextUi.h>

namespace bdn
{

    /** Combines two ITextUi implementations.

        Written text is forwarded to all sub UIs.

        Read operations are forwarded only to the primary sub UI.
    */
    class TextUiCombiner : public Base, BDN_IMPLEMENTS ITextUi
    {
      public:
        TextUiCombiner()
        {
            _outputSink = newObj<Sink>();
            _statusOrProblemSink = newObj<Sink>();
        }

        /** Initializes the test UI combiner with two text UI objects.

            You can also combine more than two UI objects (see other
           constructor).

            The first sub UI is the primary UI (the one that read operations
            use). The other sub UIs are only used when data is written.

            It is also valid to create a TextUiCombiner with one or zero sub
           UIs. When zero sub UIs are specified then read operations are dummy
           operations that never provide any data.
         */
        TextUiCombiner(ITextUi *primary, ITextUi *secondary)
        {
            _uiList.add(primary);
            _uiList.add(secondary);

            _outputSink = newObj<Sink>();
            _statusOrProblemSink = newObj<Sink>();

            for (auto &ui : _uiList) {
                _outputSink->addSubSink(ui->output());
                _statusOrProblemSink->addSubSink(ui->statusOrProblem());
            }
        }

        /** Initializes the Text UI combiner with a sequence of sub ITextUi
           objects.

            Any kind of sequence or collection object that stores pointers to
           ITextUi objects either plain pointers or smart pointers) can be
           passed to this constructor.

            The first sub UI is the primary UI (the one that read operations
            use). The other sub UIs are only used when data is written.

            It is also valid to create a TextUiCombiner with one or zero sub
           UIs. When zero sub UIs are specified then read operations are dummy
           operations that never provide any data.
         */
        template <class SEQUENCE_TYPE> TextUiCombiner(SEQUENCE_TYPE &&subUis) : _uiList(subUis.begin(), subUis.end())
        {
            _outputSink = newObj<Sink>();
            _statusOrProblemSink = newObj<Sink>();

            for (auto &ui : subUis) {
                _outputSink->addSubSink(ui->output());
                _statusOrProblemSink->addSubSink(ui->statusOrProblem());
            }
        }

        P<IAsyncOp<String>> readLine() override
        {
            if (_uiList.isEmpty())
                return newObj<DummyReadOp>();
            else
                return _uiList.front()->readLine();
        }

        P<ITextSink> statusOrProblem() override { return _statusOrProblemSink; }

        P<ITextSink> output() override { return _outputSink; }

      private:
        class DummyReadOp : public Base, BDN_IMPLEMENTS IAsyncOp<String>
        {
          public:
            DummyReadOp() { _doneNotifier = newObj<OneShotStateNotifier<P<IAsyncOp>>>(); }

            String getResult() const
            {
                if (_aborted)
                    throw AbortedError("Dummy read operation was aborted.");
                else
                    throw UnfinishedError("Dummy read operation will never finish.");
            }

            void signalStop()
            {
                _aborted = true;

                // XXX this creates a circular reference!!
                _doneNotifier->postNotification(this);
            }

            bool isDone() const { return _aborted; }

            IAsyncNotifier<P<IAsyncOp>> &onDone() const { return *_doneNotifier; }

          private:
            std::atomic<bool> _aborted{false};
            P<OneShotStateNotifier<P<IAsyncOp>>> _doneNotifier;
        };

        class Sink : public Base, BDN_IMPLEMENTS ITextSink
        {
          public:
            Sink() {}

            void addSubSink(ITextSink *sink) { _sinkList.add(sink); }

            void write(const String &s) override
            {
                for (auto &sink : _sinkList)
                    sink->write(s);
            }

            void writeLine(const String &s) override
            {
                for (auto &sink : _sinkList)
                    sink->writeLine(s);
            }

          private:
            List<P<ITextSink>> _sinkList;
        };

        List<P<ITextUi>> _uiList;
        P<Sink> _outputSink;
        P<Sink> _statusOrProblemSink;
    };
}

#endif
