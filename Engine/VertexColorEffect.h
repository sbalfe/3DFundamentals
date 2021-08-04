#pragma once

#include "Pipeline.h"

// color gradient effect between vertices
class VertexColorEffect
{
public:
	// the vertex type that will be input into the pipeline
	class Vertex
	{
	public:
		Vertex() = default;
		Vertex( const Vec3& pos )
			:
			pos( pos )
		{}
		/* creates a copy constructor for the vertices out part of the pipeline
			which extracts the color out of it to obtain the color effect for it
			
		*/
		Vertex( const Vec3& pos,const Vertex& src )
			:
			color( src.color ),
			pos( pos )
		{}
		
		Vertex( const Vec3& pos,const Vec3& color )
			:
			color( color ),
			pos( pos )
		{}
		Vertex& operator+=( const Vertex& rhs )
		{
			pos += rhs.pos;
			color += rhs.color;
			return *this;
		}
		Vertex operator+( const Vertex& rhs ) const
		{
			return Vertex( *this ) += rhs;
		}
		Vertex& operator-=( const Vertex& rhs )
		{
			pos -= rhs.pos;
			color -= rhs.color;
			return *this;
		}
		Vertex operator-( const Vertex& rhs ) const
		{
			return Vertex( *this ) -= rhs;
		}
		Vertex& operator*=( float rhs )
		{
			pos *= rhs;
			color *= rhs;
			return *this;
		}
		Vertex operator*( float rhs ) const
		{
			return Vertex( *this ) *= rhs;
		}
		Vertex& operator/=( float rhs )
		{
			pos /= rhs;
			color /= rhs;
			return *this;
		}
		Vertex operator/( float rhs ) const
		{
			return Vertex( *this ) /= rhs;
		}
	public:
		/* the member of color must be interpolated for this vertex color effect to work.*/
		Vec3 pos;

		/* stores R,G,B Vec3 255*/
		Vec3 color;
	};
	// invoked for each pixel of a triangle
	// takes an input of attributes that are the
	// result of interpolating vertex attributes
	// and outputs a color
	class PixelShader
	{
	public:
		template<class Input>
		Color operator()( const Input& in ) const
		{
			return Color( in.color );
		}
	};
public:
	PixelShader ps;
};