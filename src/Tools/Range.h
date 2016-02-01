

#pragma once

template <class T = float> class Range
{
public:
	
	T min, max;
	
	/**
	 * Constructor.
	 * Defines an empty range.
	 */
	Range() {min = max = T();}
	
	/**
	 * Constructor.
	 * Defines an empty range.
	 * @param minmax A conjoined starting and ending point of the empty range.
	 */
	Range(T minmax) {min = max = minmax;}
	
	/**
	 * Constructor.
	 * @param min The minimum of the range.
	 * @param max The maximum of the range.
	 */
	Range(T min, T max)
	{Range::min = min; Range::max = max;}
	
	/**
	 * The function checks whether a certain value is in the range.
	 * Note that the function is able to handle circular range, i.e. max < min.
	 * @param t The value.
	 * @return Is the value inside the range?
	 */
	bool isInside(T t) const
	{return min <= max ? t >= min && t <= max : t >= min || t <= max;}
	
	bool operator==(const Range<T>& r) const {return min == r.min && max == r.max;}
};
