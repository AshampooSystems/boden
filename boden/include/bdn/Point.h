#ifndef BDN_Point_H_
#define BDN_Point_H_


namespace bdn
{
	

/** Represents a 2-D point (x, y)
	*/
struct Point
{
public:
	int x = 0;
	int y = 0;

	Point()
	{		
	}

	Point(int x, int y)
		: x(x)
		, y(y)
    {
	}


};


}


inline bool operator==(const bdn::Point& a, const bdn::Point& b)
{
	return (a.x==b.x
			&& a.y==b.y);
}

inline bool operator!=(const bdn::Point& a, const bdn::Point& b)
{
	return !operator==(a, b);
}


#endif

