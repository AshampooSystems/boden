#ifndef BDN_ITitleCore_H_
#define BDN_ITitleCore_H_

namespace bdn
{

/** Interface for core object that have a title.*/
class ITitleCore : BDN_IMPLEMENTS IBase
{
public:
	
	virtual void	setTitle(const String& title)=0;
	virtual String	getTitle() const=0;

	
	/** Convenience implementation for a property delegate that sets the title.*/
	class TitleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<String>::IDelegate
	{
	public:
		TitleDelegate(ITitleCore* pCoreWeak)
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
		ITitleCore* _pCoreWeak;
	};


};


}

#endif
