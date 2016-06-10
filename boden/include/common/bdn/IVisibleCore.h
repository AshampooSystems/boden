#ifndef BDN_IVisibleCore_H_
#define BDN_IVisibleCore_H_

namespace bdn
{

/** Interface for core object that have a visibility property.*/
class IVisibleCore : BDN_IMPLEMENTS IBase
{
public:
	
	virtual void	setVisible(bool visible)=0;
	virtual bool	getVisible() const=0;

	
	/** Convenience implementation for a property delegate that sets the visibility.*/
	class VisibleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<bool>::IDelegate
	{
	public:
		VisibleDelegate(ITitleCore* pCoreWeak)
		{
			_pCoreWeak = pCoreWeak;
		}

		void set(const bool& val) override
		{
			_pCoreWeak->setVisible(val);
		}

		bool get() const override
		{
			return _pCoreWeak->getVisible();
		}

	protected:
		IVisibleCore* _pCoreWeak;
	};


};


}

#endif
