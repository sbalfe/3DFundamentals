#pragma once
#include "Vec2.h"

class MouseTracker
{
public:
	/* the user clicks */
	void Engage( const Vei2& pos )
	{
		/*
			records the base position of where the click started 
		*/
		base = pos;

		/*
			while engaged it tracks mouse position to alter rotation
		*/
		engaged = true;
	}
	void Release()
	{
		/* release the mouse click */
		engaged = false;
	}

	Vei2 Move( const Vei2& pos )
	{
		/*
			delta determines offset from original base , via the current pos the cursor is at
		*/
		const auto delta = pos - base;
		base = pos; /* new base is of course now pos 
					*/
		return delta;
	}
	bool Engaged() const
	{
		return engaged;
	}
private:
	bool engaged = false;
	Vei2 base;
};