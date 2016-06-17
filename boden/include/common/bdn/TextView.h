#ifndef BDN_TextView_H_
#define BDN_TextView_H_


namespace bdn
{
	


/** Displays text, but does not offer the user to enter any.
	See TextEditView if you need text entering capability.*/
class TextView : public View
{
public:

	/** Controls the maximum preferred width that the text view reports to its parent.
		Usually the preferred width will simply be the width of the text in the view.		
		But if the texts get too long then one usually wants the text to wrap to multiple
		lines, rather than making the view wider. This is the setting that controls
		at which point this happens.

		When the preferred width of the view exceeds the length specified here then
		the view will prefer to increase its height, rather than its width.

		It is recommended that you specify this length in UiLength::sem units, to ensure
		that they scale properly in all circumstances and on all devices.
		*/
	Property<UiLength>& maxPreferredWidth()
	{		
	}
	
};


}


#endif