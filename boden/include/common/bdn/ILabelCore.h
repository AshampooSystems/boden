#ifndef BDN_ILabelCore_H_
#define BDN_ILabelCore_H_

namespace bdn
{

/** Interface for core object that have a label.*/
class ILabelCore : BDN_IMPLEMENTS IBase
{
public:
	
	virtual void	setLabel(const String& label)=0;
	virtual String	getLabel() const=0;

	
	/** Convenience implementation for a property delegate that sets the label.*/
	class LabelDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<String>::IDelegate
	{
	public:
		LabelDelegate(ILabelCore* pCoreWeak)
		{
			_pCoreWeak = pCoreWeak;
		}

		void set(const String& val) override
		{
			_pCoreWeak->setLabel(val);
		}

		bool get() const override
		{
			return _pCoreWeak->getLabel();
		}

	protected:
		ILabelCore* _pCoreWeak;
	};


};


}

#endif
