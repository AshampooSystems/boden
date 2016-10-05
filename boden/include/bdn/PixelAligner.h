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


    /** Rounds the specified size to a full pixel size. The rounding type is indicated by the roundType
        parameter.
        
        The rounding is stable. I.e. passing the rounding result as the input size to stableRoundSize
        again produces the same result.
        See stableScaledRoundUp() for more information about this.
        */
    Size roundSize(RoundType roundType, const Size& size) const
    {
        return Size( stableScaledRound(roundType, size.width, _dipsToPixelsFactor),
                     stableScaledRound(roundType, size.height, _dipsToPixelsFactor) );
    }


    /** Aligns the specified position to a pixel boundary and returns the aligned position.
    
        The position is rounded to the boundary according to the roundType parameter.    
    */
    Point alignPosition(const Point& pos, RoundType roundType) const
    {
        return Point(stableScaledRound(roundType, pos.x, _dipsToPixelsFactor),
                     stableScaledRound(roundType, pos.y, _dipsToPixelsFactor) );
    }



    /** Aligns the specified rectangle to pixel boundaries. The position is always rounded
        to the nearest pixel. The size is rounded according to the sizeRoundType parameter.*/
    Rect alignRect(const Rect& rect, RoundType sizeRoundType) const
    {
        return Rect( alignPosition( RoundType::nearest, rect.getPosition()),
                     roundSize( sizeRoundType, rect.getSize() ) );
    }


private:
    double _dipsToPixelsFactor;
};
	

}

#endif
