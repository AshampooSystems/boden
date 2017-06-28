#ifndef BDN_View_H_
#define BDN_View_H_

namespace bdn
{
	class View;
    class IViewCore;    // included below
    class IUiProvider;      // included below
}

#include <bdn/UiMargin.h>
#include <bdn/UiSize.h>
#include <bdn/Rect.h>
#include <bdn/Nullable.h>
#include <bdn/RequireNewAlloc.h>
#include <bdn/DefaultProperty.h>
#include <bdn/mainThread.h>
#include <bdn/round.h>


namespace bdn
{


/** Views are the building blocks of the visible user interface.
    A view presents data or provides some user interface functionality.
    For example, buttons, text fields etc are all view objects.

	View objects must be allocated with newObj or new.
   */
class View : public RequireNewAlloc<Base, View>
{
public:
	View();
	~View();

	// delete copy constructor
	View(const View& o) = delete;



	/** Returns the core object of this view.

		The core can be null if the view is not currently connected (directly or indirectly) to a top level window.
		It can also be null for short periods of time when a reinitialization was necessary.

		The core provides the actual implementation of the view. It is provided by the
		IUiProvider object that the view uses. The IUiProvider is inherited from the parent view
		and can be explicitly set when creating a top level window.
		*/
	P<IViewCore> getViewCore() const
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pCore;		
	}

	
	/** Returns the property which controls wether the view is
        visible or not.
        
        Note that a view with visible=true might still not show on
        the screen if one of its parents is invisible. In other words:
        this visible property only raefers to the view itself, not
        the parent hierarchy.

		It is safe to access this from any thread.
		*/
	virtual Property<bool>& visible()
	{
		// no need for mutex locking. Properties are thread-safe.
		return _visible;
	}

	virtual const ReadProperty<bool>& visible() const
	{
		// no need for mutex locking. Properties are thread-safe.
		return _visible;
	}


	/** The size of the empty space that should be left around the view.
	
		The margin is NOT part of the view itself. It is merely something that the
		layout takes into account.
        
		The default margin is 0.
	*/
	virtual Property<UiMargin>& margin()
	{
		return _margin;
	}

	virtual const ReadProperty<UiMargin>& margin() const
	{
		return _margin;
	}


	/** The size space around the content inside this view.

		The padding is part of the view and thus it influences the size of
		the view (in contrast to the margin(), which is NOT part of the view).
                
        On some platforms some UI elements may have a built-in minimum for the padding.
        If you specify a smaller padding then the minimum padding will be silently
        used instead.
        
        The padding can also be "null", which means that the implementation
        should use the "normal" or "most commonly used" padding for the UI element on the platform.
        
        The default padding is "null".
	*/
	virtual Property< Nullable<UiMargin> >& padding()
	{
		return _padding;
	}

	virtual const ReadProperty< Nullable<UiMargin> >& padding() const
	{
		return _padding;
	}


	/** The position of the view inside its parent coordinate system in DIP units (see UiLength::Unit::dip).

        The position property is read-only. The position of a view can be modified with adjustAndSetBounds,
        which is usually called automatically during the parent view's layout process.

		The default position for a newly constructed view is always position 0,0.
        The position is usually initialized automatically by the parent view's layout routine.

        IMPORTANT:

        On some platforms top level windows (see #Window class) may report a zero position at all times, even though
        the window is not at the top left corner of the screen.
	*/
	virtual const ReadProperty<Point>& position() const
	{
		return _position;
	}



    /** The size of the view DIP units (see UiLength::Unit::dip).

        The size property is read-only. The size of a view can be modified with adjustAndSetBounds,
        which is usually called automatically during the parent view's layout process.
	
		The default size for a newly constructed view is always 0x0.
		The size is usually set automatically by the parent view's layout routine.        
	*/
	virtual const ReadProperty<Size>& size() const
	{
		return _size;
	}



    /** Sets the view's position and size, after adjusting the specified values
        to ones that are compatible with the underlying view implementation. The bounds are specified in DIP units
        and refer to the parent view's coordinate system.

        IMPORTANT: This function must only be called from the main thread.

        See adjustBounds() for more information about the adjustments that are made.
        
        Note that the adjustments are made with a "nearest valid" policy. I.e. the position and size are set
        to the closest valid value. This can mean that the view ends up being bigger or smaller than requested.
        If you need more control over which way the adjustments are made then you should pre-adjust the bounds
        with adjustBounds().

        The function returns the adjusted bounds that are actually used.

        IMPORTANT:

        The position and/or size of top level #Window objects are restricted on some platforms. Sometimes
        it is not possible to change the top level Window bounds at all (in that case the bounds will be
        "adjusted" to the current bounds value).

        On some platforms top level windows may also report a zero position at all times, even though
        the window is not at the top left corner of the screen.
        */
    virtual Rect adjustAndSetBounds(const Rect& requestedBounds);



    /** Adjusts the specified bounds to values that are compatible with the underlying view implementation
        and returns the result. The bounds are specified in DIP units and refer to the parent view's coordinate system.

        IMPORTANT: This function must only be called from the main thread.

        Not all positions and sizes are necessarily valid for all view implementations. For example,
        the backend might need to round the abstract DIP coordinates to the nearest physical pixel boundary.

        The function adjusts the specified bounds according to its implementation constraints and returns the
        valid values. The positionRoundType and sizeRoundType control in which direction adjustments are made
        (adjusting up, down or to the nearest valid value).     

        IMPORTANT:

        The position and/or size of top level #Window objects are restricted on some platforms. Sometimes
        it is not possible to change the top level Window bounds at all (in that case the bounds will be
        "adjusted" to the current bounds value).

        On some platforms top level windows may also report a zero position at all times, even though
        the window is not at the top left corner of the screen.
    */
    virtual Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const;





	enum class HorizontalAlignment
	{			
		left,
		center,
		right,
		/** Expand the view to the full available width.*/
		expand
	};

	enum class VerticalAlignment
	{
		top,
		bottom,
		middle,		
		/** Expand the view to the full available height.*/
		expand
	};


	/** Controls how the view is arranged vertically if
		there is additional vertical free space. Parent view containers can ignore this setting
		if it does not make sense in their context.		

		The default alignment is View::VerticalAlignment::top.
		*/
	virtual Property<VerticalAlignment>& verticalAlignment()
	{
		return _verticalAlignment;
	}

	virtual const ReadProperty<VerticalAlignment>& verticalAlignment() const
	{
		return _verticalAlignment;
	}


	/** Controls how the view is arranged horizontally if
		there is additional horizontal free space. Parent view containers can ignore this setting
		if it does not make sense in their context.		

		The default alignment is View::HorizontalAlignment::left.
		*/
	virtual Property<HorizontalAlignment>& horizontalAlignment()
	{
		return _horizontalAlignment;
	}

	virtual const ReadProperty<HorizontalAlignment>& horizontalAlignment() const
	{
		return _horizontalAlignment;
	}


	/*

	virtual Property<double>& extraSpaceWeight()
	{
		return _extraSpaceWeight;
	}

	virtual const ReadProperty<double>& extraSpaceWeight() const
	{
		return _extraSpaceWeight;
	}

	*/
	
	/** Returns the UI provider used by this view. This can be nullptr if no UI provider
		is currently associated with the view object. This can happen, for example, when the
		view object is not yet connected to a top level window (either directly or indirectly).
		
		Note that there can sometimes be a short delay after a window has been added to a new parent
		until its UI provider becomes available in the child view.
		*/
	P<IUiProvider> getUiProvider()
	{
		// the UI provider depends on the hierarchy
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pUiProvider;
	}


	/** Returns the type name of the view core. This is a somewhat arbitrary name that is used
		in the internal implementation. It is NOT necessarily the same as the name of the
		C++ class of the view or view core (although it is often similar).
		*/
	virtual String getCoreTypeName() const=0;
	

	/** Returns the view's parent view. This can be null if the view was not yet
		added to a parent, or if the view is a top level window.*/
	virtual P<View> getParentView()
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pParentViewWeak;
	}

	
	/** Stores a list with all the child views in the target list object.*/
	virtual void getChildViews(std::list< P<View> >& childViews) const
	{
		// no child views by default. So nothing to do.
	}


	/** Finds the child view that "precedes" the specified one.
		Returns nullptr if any of the following conditions are true:
		
		- the specified view is not a child of this view
		- the specified view is the first child of this view
		- this view does not define an order among its children

		*/
	virtual P<View> findPreviousChildView(View* pChildView)
	{
		// no child views by default
		return nullptr;
	}
	

	/** Should only be called by view container implementors when they add or remove a child.
		Users of View objects should NOT call this.		

		Tells the view object that it has a new parent.
		pParentView can be nullptr if the view was removed from a parent
		and does not currently have one.

		Note that any modifications to the view hierarchy should only be done while
		the mutex returned by getHierarchyAndCoreMutex() is locked.
		*/
	void _setParentView(View* pParentView);



	/** Should only be called by view container implementations.
		Users of View objects should NOT call this.
		
		This must be called when another view container "steals" a view that
		was formerly a child of this view.		

		Note that any modifications to the view hierarchy should only be done while
		the mutex returned by getHierarchyAndCoreMutex() is locked.
		*/
	virtual void _childViewStolen(View* pChildView)
	{
        // do nothing by default
	}


	/** Requests that the view updates its sizing information (preferred size, etc.).
		The measuring does not happen immediately in this function - it is performed asynchronously.
		
		Note that it is usually NOT necessary to call this as a user of a view object. The view object
		will automatically schedule re-measuring when its parameters and properties change.
		*/
	void needSizingInfoUpdate();


	/** Requests that the view updates the layout of its child view and contents.
		
		The layout operation does not happen immediately in this function - it is performed asynchronously.
		
		Note that it is usually NOT necessary to call this as a user of a view object. The view object
		will automatically schedule re-layout operations when its layout parameters or child views change.
		*/
	void needLayout();


    /** An optional hint for the viewa s to how to calculate its preferred size. This can be set by the App to
        influence the automatic sizing of the view.
    
        This does *not* set a hard limit like preferredSizeMinimum() or preferredSizeMaximum(). The view is
        free to ignore this hint, if it does not make sense in the context of the view.

        But for some views the hint is used to influence the calculation of the preferred size. For example,
        text views should use the hint width as a guideline as to where to automatically wrap their text.
        
        Width and/or height of the hint can be set to Size::componentNone() to indicate that there is no hint for that
        component (i.e. the view should choose the preferred size completely on its own).

        The default value is Size::none(), i.e. there is no size hint.

        IMPORTANT: this property only influences the preferred size that the view requests during layout
        (see calcPreferredSize()).
        Its parent view may decide to make it bigger than this because of other layout considerations.        
    */
    virtual Property<Size>& preferredSizeHint()
    {
        return _preferredSizeHint;
    }

    virtual const ReadProperty<Size>& preferredSizeHint() const
    {
        return _preferredSizeHint;
    }


    /** An optional lower limit for the preferred size of the view (in DIP units). This can be used by the application
        to influence the layout of the view and enforce special sizing.

        Width and/or height of the constraint can be set to Size::componentNone() to indicate that the corresponding
        component should not have a lower limit.

        The default value is Size::none(), i.e. there is no minimum for either width or height.

        The view will automatically apply this constraint when calculating its preferred size. It is a hard limit,
        so the view will never report a preferred size below this minimum.        

        IMPORTANT: this property only influences the preferred size that the view requests during layout
        (see calcPreferredSize()).
        Its parent view may decide to make it bigger than this because of other layout considerations.        
    */
    virtual Property<Size>& preferredSizeMinimum()
    {
        return _preferredSizeMinimum;
    }

    virtual const ReadProperty<Size>& preferredSizeMinimum() const
    {
        return _preferredSizeMinimum;
    }



    /** An optional upper limit for the preferred size of the view (in DIP units). This can be used by the application
        to influence the layout of the view and enforce special sizing.

        Width and/or height of the constraint can be set to Size::componentNone() to indicate that the corresponding
        component should not have an upper limit.

        The default value is Size::none(), i.e. there is no maximum for either width or height.

        The view will automatically apply this constraint when calculating its preferred size. It is a hard limit,
        so the view will never report a preferred size that exceeds this maximum. 

        IMPORTANT: this property only influences the preferred size that the view requests during layout
        (see calcPreferredSize()).
        Its parent view may decide to make it bigger than this because of other layout considerations.        
    */
    virtual Property<Size>& preferredSizeMaximum()
    {
        return _preferredSizeMaximum;
    }

    virtual const ReadProperty<Size>& preferredSizeMaximum() const
    {
        return _preferredSizeMaximum;
    }




	struct SizingInfo
	{
        /** The preferred size of the view (in DIP units), assuming that unlimited space is available.
        
            Note that this size is not yet adjusted to meet the constraints of the physical display
            that the view is rendered to. For example, it is NOT rounded to full pixel boundaries.
            As a result, the size might be adjusted when it is assigned to the view with
            adjustAndSetSize(). You can also call adjustSize() ahead of time to adjust it manually,
            if you need to.
            */
		Size preferredSize;

		bool operator==(const SizingInfo& o) const
		{
			return preferredSize == o.preferredSize;
		}

		bool operator!=(const SizingInfo& o) const
		{
			return !operator==(o);
		}
	};


	/** Returns the sizing information of the view (to use during layout).
	*/
	const ReadProperty<SizingInfo>& sizingInfo() const
	{
		return _sizingInfo;
	}



    /** Converts a UiLength object to DIPs.
        DIP stands for "device independent pixel", a special unit (see UiLength::Unit::dip).
		
        This uses view-specific internal data, so the result can be different
		for different view objects.
		The result can differ when this function is called again at a later time with the same view object
		(if the view's parameters or the operating systems settings have changed).

		IMPORTANT: This function must only be called from the main thread.
		*/
	double uiLengthToDips( const UiLength& length) const;



	/** Converts a UiMargin object to a DIP based margin object.
        DIP stands for "device independent pixel", a special unit (see UiLength::Unit::dip)
		
        This uses view-specific internal data, so the result can be different
		for different view objects.
		The result can differ when this function is called again at a later time with the same view object
		(if the view's parameters or the operating systems settings have changed).

		IMPORTANT: This function must only be called from the main thread.
		*/
	Margin uiMarginToDipMargin( const UiMargin& uiMargin) const;



    /** Asks the view to calculate its preferred size in DIPs (see UiLength::Unit::dip),
        based on it current contents	and properties.

        There are several constraints for the preferred size:

        availableSpace
        --------------
        
		The availableSpace function parameter is used to indicate the maximum amount of available
		space for the view (also in DIPs). If availableSpace is Size::none() (i.e. width and height equal Size::componentNone())
        then that means that the available space should be considered to be unlimited.
		I.e. the function should return the view's optimal size.

		When one of the availableSpace components (width or height) is not Size::componentNone() then it means that the available space is limited
		in that dimension. The function should return the preferred size of the view within those constraints,
		trying to not exceed the limited size component.
		
		For example, many views displaying text can handle a limited available width by wrapping the text into
		multiple lines (and thus increasing their height).

		If the view cannot reduce its size to fit into the available space then it is valid for the function
		to return a size that exceeds the available space. However, the layout manager is free to
		size the view to something smaller than the returned preferred size.

        preferredSizeHint(), preferredSizeMinimum() and preferredSizeMaximum()
        ---------------------------------------------

        calcPreferredSize must also take the View::preferredSizeHint(), View::preferredSizeMinimum() and View::preferredSizeMaximum()
        properties into account and constrains the result accordingly.
        
        preferredSizeHint() can be used to provide an advisory hint to the view as to what the preferred width and/or height should
        roughly be. The view is free to ignore this, however. Text views often use this to select the place where they wrap their
        text into multiple lines, for example.

        preferredSizeMinimum() and preferredSizeMaximum() are optional hard limits. calcPreferredSize should never
        return a size that violates these limits, if they are set. Even if that means that the view's content does not fit into
        the view.
     
        If there is a conflict between the minimum and maximum and/or hint values then the values should
        be prioritized in this ascending order: hint, minimum, maximum.
        So the maximum value has the highest priority and the returned value should never exceed
        it. For example, if a minimum is set that exceeds the maximum
        then the maximum should "win" and the preferred size should not exceed the maximum.
         

        Important Notes
        ---------------

        IMPORTANT: It is perfectly ok (even recommended) for the view to return a preferred size
        that is not adjusted for the properties of the current display / monitor yet. I.e. it may not be rounded
        to full physical pixels yet. The size will be adapted to the display properties in adjustAndSetBounds().        

        IMPORTANT: This function must only called be called from the main thread.
		*/		
    virtual Size calcPreferredSize( const Size& availableSpace = Size::none() ) const;



    

	/**	Updates the layout of the view's its contents and child views.

        IMPORTANT: this function should never be called directly by the app. The
        platform's layout system takes care of calling this function at optimized times.
        Call needLayout() instead when you need to manually trigger a re-layout of the view.*/
    void _doLayout()
    {
        layout();
    }


    /**	Internal function that calls updateSizingInfo. Should only be called by the view implementation.*/
    void _doUpdateSizingInfo()
    {
        updateSizingInfo();
    }

    

    
	/** Returns the global mutex object that is used to synchronize changes in the
		UI hierarchy (parent-child relationships) and replacement of view core objects.

        Why a single global mutex?
        ---------------------------

		The reason why we use a single global mutex for hierarchy changes is that otherwise deadlocks
		could occur. We need to lock the old parent, the child and the new parent.
		If multiple changes with the same objects are done in different threads
		then it could potentially happen that the same two objects are locked in inverse order
		in two threads, creating a deadlock.
		For example, consider this UI hierarchy:

		A
		  B
			C
		D

		Lets say we want to move B to D and C to D at the same time. Since B is the child-to-be-moved
		for one operation and the old parent for another, the locking order could easily be inverse
		and thus a deadlock could occur.

		To avoid all this we use a single mutex for all hierarchy modifications. The impact on parallel
		performance should be negligible, since the operations are short (just setting a parent pointer or
		adding to a child list). Also, it should be a rare case when the hierarchy is modified from two
		threads at the same time.

		The same mutex is used to guard changes to the view cores. The reason is that hierarchy changes
		sometimes cause creation, destruction or replacement of view cores. And these changes can also
		propagate down the UI hierarchy (if a parent core is destroyed then all child cores must also
		be destroyed). Because of this, the hierarchy mutex must be locked whenever a core is updated
		(so that it does not change during the update operation). And if we had multiple mutexes
		for cores and the hierarchy, then such operations would again be very sensitive to locking order
		and could create potential deadlocks.
		*/
	static Mutex& getHierarchyAndCoreMutex();


    


protected:
    
    /** Updates the layout of the view's its contents and child views.
    
        The view should NOT update its OWN size or position during this - 
		it has to work with the size and position it currently has and
		should ONLY update the size and position of its child views.

        IMPORTANT: This function should never be called manually. It should
        only be called by the platform's layout coordinator.

        This function must only be called from the main thread.

        Note to implementors
        --------------------

        Depending on the UI implementation backend, 
        the sizes and positions of child views may have some constraints. For example,
        with many implementations the sizes and positions must be rounded to full physical
        pixel boundaries. The layout() function should be aware of this and use
        adjustBounds() to calculate a bounds rect that meets these constraints.
        
        When calling adjustBounds in this context, it is recommended to use RoundType::up for rounding child view positions.
        That ensures that small margins that are less than 1 pixel in size are rounded up to 1 pixel, rather than
        disappearing completely.
        
        Child view sizes should usually be rounded with RoundType::up when enough space is available
        and RoundType::down when not enough space is available.

        The rounding policies noted above are merely guidelines: layout implementations are free to
        ignore them if there are other considerations that cause other rounding types to be better for
        for the particular case.

		*/
	virtual void layout()=0;


	/** Verifies that the current thread is the main thread.
		Throws a ProgrammingError if that is not the case.
		The methodName parameter should be the name of the method that was called
		that should have been called from the main thread.*/
	void verifyInMainThread(const String& methodName) const;


	/** Tells the view to update its sizing info.
	
		IMPORTANT: This must only be called from the main thread.
	*/
	virtual void updateSizingInfo();


    /** This is called when the sizing information of a child view has changed.
        Usually this will prompt this view (the parent view) to also schedule an update to
        its own sizing information and an update to its layout.*/
    virtual void childSizingInfoChanged(View* pChild);



	// allow the coordinator to call the sizing info and layout functions.
	friend class LayoutCoordinator;




    enum class PropertyInfluence_
    {
        /** The property has no influence on the view size or layout.*/
        none=0,

        /** The property influences the view's preferredSize (and as such it can also influence the 
            parent layout)*/
        preferredSize = 1,

        /** The property influences how the view lays out its own children*/
        childLayout = 2,


        /** The property influences the size of the view's parent, but not the view's own preferred size.
            An example of a property with this influence would be the view's margin.
            */
        parentPreferredSize = 4,

        /** The property influences how the view is arranged within the parent, but it does not
            influence the view's own PREFERRED size. Note that the property may influence the actual size
            that the parent assigns to the view, based on the arrangement values.*/
        parentLayout = 8,
    };

	template<typename ValueType, class CoreInterfaceType, void (CoreInterfaceType::*CoreFunc)(const ValueType &), int propertyInfluences>
	void initProperty( Property<ValueType>& prop )
	{	
		prop.onChange() += weakMethod(this, &View::propertyChanged<ValueType, CoreInterfaceType, CoreFunc, propertyInfluences>);
	}


    void handlePropertyInfluences(int propertyInfluences)
    {
        if( (propertyInfluences & (int)PropertyInfluence_::preferredSize)!=0 )
        {
            // update the sizing information. If that changes then the parent
            // layout will automatically be updated.
            needSizingInfoUpdate();
        }    
        
        if( (propertyInfluences & (int)PropertyInfluence_::childLayout)!=0 )
        {
            // the layout of our children is influenced by this
            needLayout();
        }

        if( (propertyInfluences & (int)PropertyInfluence_::parentPreferredSize)!=0 )
        {
            P<View> pParent = getParentView();
            if(pParent!=nullptr)
                pParent->needSizingInfoUpdate();
        }

        if( (propertyInfluences & (int)PropertyInfluence_::parentLayout)!=0 )
        {
            P<View> pParent = getParentView();
            if(pParent!=nullptr)
                pParent->needLayout();
        }
    }

	template<typename ValueType, class CoreInterfaceType, void (CoreInterfaceType::*CoreFunc)(const ValueType&), int propertyInfluences >	
	void propertyChanged( P<const IValueAccessor<ValueType> > pValue )
	{
		// note that our object is guaranteed to be fully alive during this function call.
		// That is guaranteed because we subscribed using a weak method. And during the call
        // a weak method holds a strong reference.

        // get the core. Note that it is OK if the core object
		// is replaced directly after this during this call.
		// We will update an outdated core, but thats should have no effect.
		// And the new core will automatically get the up-to-date value from
		// the property.
		P<CoreInterfaceType>	pCore = cast<CoreInterfaceType>( _pCore );
		if(pCore!=nullptr)
		{			
            // note that we can call the core functions directly. Notifiers always call the
            // subscribed functions from the main thread. And they also guarantee that no
            // mutexes are locked during the call. So there are no restrictions on what we can call here.
			if(CoreFunc!=nullptr)
                (pCore->*CoreFunc)( pValue->get() );

            // after the core has been updated we need to handle the influences.
            // Note that we still need to handle the influences, even if the core is not
            // notified of the change (CoreFunc==null).

            // Also note that if multiple properties get changed then their change notifications
            // will already be in the queue at the point when we are called. That means that
            // layout updates that are triggered by these changes are automatically batched
            // together, since the layout update is posted to the end of the queue.
            handlePropertyInfluences(propertyInfluences);
		}        
	}






	/** (Re-)initializes the core object of the view. If a core object existed before then
		the old object is destroyed.

		The core object is immediately detached from the view (before reinitCore returns).
		
		If the view is part of a UI hierarchy that is connected to a top level window then a new
		core will be created shortly thereafter.

		If reinitCore is called from the main thread then a new core is immediately created and
		attached, before reinitCore returns.

		If reinitCore is called from some other thread then the core will be initially null
		when the function returns. A new core will be created asynchronously and will be set shortly
		thereafter.

		reinitCore also causes the reinitialization of the cores of all child views.
		*/
	void reinitCore();


	/** Determines the Ui provider to use with this view object.
		The default implementation returns the parent view's Ui provider,
		or null if the view does not have a parent or the parent does not
		have a ui provider.
		*/
	virtual P<IUiProvider> determineUiProvider()
	{
		return (_pParentViewWeak != nullptr) ? _pParentViewWeak->getUiProvider() : nullptr;
	}

private:
	/** Should not be called directly. Use reinitCore() instead.	
	*/
	void _deinitCore();


	/** Should not be called directly. Use reinitCore() instead.*/
	void _initCore();


protected:
	DefaultProperty<bool>                   _visible;
	DefaultProperty<UiMargin>               _margin;
	DefaultProperty< Nullable<UiMargin> >	_padding;

	DefaultProperty<Point>                  _position;
    DefaultProperty<Size>                   _size;

	DefaultProperty<HorizontalAlignment>	_horizontalAlignment;
	DefaultProperty<VerticalAlignment>		_verticalAlignment;

	P<IUiProvider>			                _pUiProvider;
		

private:
	View*					        _pParentViewWeak = nullptr;
	P<IViewCore>			        _pCore;
    
	DefaultProperty<SizingInfo>		_sizingInfo;

    DefaultProperty<Size>           _preferredSizeHint;
    DefaultProperty<Size>           _preferredSizeMinimum;
    DefaultProperty<Size>           _preferredSizeMaximum;
};

}


#include <bdn/IViewCore.h>
#include <bdn/IUiProvider.h>

#endif

