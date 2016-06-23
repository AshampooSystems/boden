#ifndef BDN_TextView_H_
#define BDN_TextView_H_


namespace bdn
{
	


/** Displays text, but does not offer the user to enter any.
	See TextEditView if you need text entering capability.

	The text view will automatically wrap text into multiple lines if it does not
	fit into the view otherwise.
	
	With default settings, the preferred size of the text view will be a size with which
	it does not have to wrap any text. For long texts this is usually not optimal. For that
	reason it is recommended that you set a preferredSizeHint() to let the text view know
	what its preferred width should be (roughly). When such a hint is set then the text view
	will calculate a preferred size that comes as close to that hint as possible.
	*/
class TextView : public View
{
public:

	/** Can be used to give the text view a hint as to what the preferred width
		or height should be.

		If width or height are -1 then it means "unspecified" and the text view will
		automatically calculate an optimal value for the missing component.

		It is recommended that you set a hint for the preferred width of a text view
		to give the view a hint at what width it should start to automatically wrap its
		text into multiple lines. It is usually best to set the height component of the
		preferred size hint to -1.

		It is recommended that you specify any size hint in UiLength::sem units, to ensure
		that they scale properly in all circumstances and on all devices.
		*/
	Property<UiSize>& preferredSizeHint()
	{		
	}


	const ReadProperty<UiSize>& preferredSizeHint() const
	{		
	}
	
};


}


#endif