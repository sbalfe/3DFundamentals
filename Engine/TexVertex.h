#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include <iostream>
class TexVertex
{
public:
	TexVertex( const Vec3& pos,const Vec2& tc )
		:
		pos( pos ),
		tc( tc )
	{
		
	}
	/* interpolation between 2 texture vertices */
	TexVertex InterpolateTo( const TexVertex& dest,float alpha ) const
	{
		return{
			pos.InterpolateTo( dest.pos,alpha ),
			tc.InterpolateTo( dest.tc,alpha )
		};
	}
	TexVertex& operator+=( const TexVertex& rhs )
	{
		pos += rhs.pos;
		tc += rhs.tc;
		return *this;
	}
	TexVertex operator+( const TexVertex& rhs ) const
	{
		return TexVertex( *this ) += rhs;
	}
	TexVertex& operator-=( const TexVertex& rhs )
	{
		pos -= rhs.pos;
		tc -= rhs.tc;
		return *this;
	}
	TexVertex operator-( const TexVertex& rhs ) const
	{
		return TexVertex( *this ) -= rhs;
	}
	TexVertex& operator*=( float rhs )
	{
		pos *= rhs;
		tc *= rhs;
		return *this;
	}
	TexVertex operator*( float rhs ) const
	{
		return TexVertex( *this ) *= rhs;
	}
	TexVertex& operator/=( float rhs )
	{
		pos /= rhs;
		tc /= rhs;
		return *this;
	}
	TexVertex operator/( float rhs ) const
	{
		return TexVertex( *this ) /= rhs;
	}
public:
	/* our textures now must include the texture coordinate */
	Vec3 pos;
	Vec2 tc;
};