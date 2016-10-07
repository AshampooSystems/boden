#ifndef BDN_PixelAligner_H_
#define BDN_PixelAligner_H_

#include <cmath>

namespace bdn
{


/** Aligns coordinates in the DIP coordinate space to pixel boundaries.
*/
class PixelAligner : public Base
{
public:
    /** \param dipsToPixelsFactor The factor to convert from DIP units to pixels.
            For example, if 100 DIPs correspond to 200 pixels, then the factor would be 2.
            The factor does NOT have to be an integer.
    */
    explicit PixelAligner(double dipsToPixelsFactor)
	{		
        if(dipsToPixelsFactor==0)
            throw InvalidArgumentError("PixelAligner constructor called with zero dipsToPixelsFactor.");

        _dipsToPixelsFactor = dipsToPixelsFactor;
	}


    /** Aligns the specified rectangle to pixel boundaries. positionRoundType indicates how the position
        is rounded, sizeRoundType indicates how the size is rounded.*/
    Rect alignRect(const Rect& rect, RoundType positionRoundType, RoundType sizeRoundType) const
    {
        return Rect( stableScaledRound(positionRoundType, rect.x, _dipsToPixelsFactor),
                     stableScaledRound(positionRoundType, rect.y, _dipsToPixelsFactor),
                     stableScaledRound(sizeRoundType, rect.width, _dipsToPixelsFactor),
                     stableScaledRound(sizeRoundType, rect.height, _dipsToPixelsFactor) );
    }


private:
    double _dipsToPixelsFactor;
};
	

}

#endif
