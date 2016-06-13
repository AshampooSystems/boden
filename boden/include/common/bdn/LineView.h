#ifndef BDN_LineView_H_
#define BDN_LineView_H_

#include <bdn/View.h>

namespace bdn
{
	

/** A container view that arranges its children one after the other
	in a straight line (either horizontally or vertically).
	
	The default orientation is vertical. I.e. child views are
	displayed one below the other.
	*/
class LineView : public View
{
public:
	LineView()
	{		
	}

	Property<Orientation>& orientation()
	{
		return *_pOrientation;
	}

	/** Static function that sets the extra space factor of a child view.
		
		When more space is available then needed then the extra space is
		assigned to the children according to this factor.

		The value of this factor is only meaningful when compared to the values of the
		other items in the line view. If an item's factor is twice as big as that of another
		item then it will get twice as much free space as that other item.

		So if you want the free space to be equally distributed among all items then
		you need to simply set the extraSpaceFactor to the same value for all items. It does
		not matter which value that is, as long as it is > 0 and the same for all items.

		The default is 0 (i.e. the child view does grow when extra space is available).

		If all child items have an extra space factor of 0 then the extra space is not distributed
		to the children.
		There will simply be a big empty space after the last child item in the line view.
	*/
	static void setExtraSpaceFactor(View* pView, double factor)
	{
		pView->setAttribute("LineView.extraSpaceFactor", factor);
	}


	/** Defines the parameters for one item in the line view.
	
		The parameter functions are designed to be chainable.

		Example:

		\code

		pLineView->addItem( pChildView, LineView::Params()
											.expand()
											.align( 0.5 )
											.extraSpaceFactor( 7 ) );

		\endcode	
	*/
	class Params
	{
	public:
		Params()
		{			
			_expand = false;
			_align = Alignment::Start;
			_extraSpaceFactor = 0;
		}
		

		/** Sets the extra space factor of the child view.
		
			When more space is available then needed then the extra space is
			assigned to the children according to this factor.

			The value of this factor is only meaningful when compared to the values of the
			other items in the line view. If an item's factor is twice as big as that of another
			item then it will get twice as much free space as that other item.

			So if you want the free space to be equally distributed among all items then
			you need to simply set the extraSpaceFactor to the same value for all items. It does
			not matter which value that is, as long as it is > 0 and the same for all items.

			The default is 0 (i.e. the child view does grow when extra space is available).

			If all child items have an extra space factor of 0 then the extra space is not distributed
			to the children.
			There will simply be a big empty space after the last child item in the line view.

			Returns a reference to the Params object.
		*/
		Params& extraSpaceFactor(double factor)
		{
			_extraSpaceFactor = factor;			
			return *this;
		}


		/** Controls wether or not the child view is expanded in the direction perpendicular to the line.
			
			In a horizontal line view this controls wether the child view is expanded to the container view's
			height.

			In a vertical line view this controls it is expanded to the container's width.

			Returns a reference to the Params object.
		*/
		Params& expand(bool expand=true)
		{
			_expand = expand;
			return *this;
		}


		/** Controls the alignment of the child view in the direction perpendicular to the line.
		
			In a horizontal line view this controls the vertical alignment. 0 means top, 0.5 means middle, 1 means bottom.
			
			In a vertical line view it controls the horizontal alignment.  0 means left, 0.5 means middle, 1 means right.

			Other values between 0 and 1 are also possible if a non-centered alignment is desired. For example, with 0.33
			in a vertical line, there will be twice as much free space to the right of the child view as to its left.

			Returns a reference to the Params object.
			*/
		Params& align(double align)
		{
			_align = align;
			return *this;
		}
		
	};

	void addChildView(View* pChildView, const Params& params)
	{
		_childViewParamMap[pChildView] = newObj< PropertyWithMainThreadDelegate<Params> >( newObj<ParamsDelegate>(pChildView), params);

		_childViews.push_back(pChildView);

		addedChild(pChildView);
	}


	void insertChildView(View* pInsertBefore, View* pChildView, const Params& params)
	{
		// note: if the specified insertbefore view is not found then
		// we get 'end' as the iterator. That is ok.
		auto insertPosIt = (pInsertBefore==nullptr) ? _childViews.begin() : findChildView(pInsertBefore);
		_childViews.insert(insertPosIt, pChildView);

		addedChild(pChildView);
	}


	/** Returns a property with the parameters of the specified child view.
		This can be used to read or change the parameters.*/
	Property<Params>& childViewParams(View* pChildView)
	{
		auto it = _childViewParamMap.find(pChildView);
		if(it==_childViewParamMap.end())
			throw InvalidArgumentError("The specified child view was not found.");

		return *(*it);	
	}


	ReadProperty<Params>& childViewParams(View* pChildView) const
	{
		return const_cast<LineView*>(this)->childViewParams(pChildView);
	}
};


}

#endif