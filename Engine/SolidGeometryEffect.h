#pragma once

#include "Pipeline.h"
#include "DefaultVertexShader.h"

// solid color attribute taken from table in gs and not interpolated
class SolidGeometryEffect
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
		Vertex( const Vec3& pos,const Vertex& src )
			:
			pos( pos )
		{}
		Vertex& operator+=( const Vertex& rhs )
		{
			pos += rhs.pos;
			return *this;
		}
		Vertex operator+( const Vertex& rhs ) const
		{
			return Vertex( *this ) += rhs;
		}
		Vertex& operator-=( const Vertex& rhs )
		{
			pos -= rhs.pos;
			return *this;
		}
		Vertex operator-( const Vertex& rhs ) const
		{
			return Vertex( *this ) -= rhs;
		}
		Vertex& operator*=( float rhs )
		{
			pos *= rhs;
			return *this;
		}
		Vertex operator*( float rhs ) const
		{
			return Vertex( *this ) *= rhs;
		}
		Vertex& operator/=( float rhs )
		{
			pos /= rhs;
			return *this;
		}
		Vertex operator/( float rhs ) const
		{
			return Vertex( *this ) /= rhs;
		}
	public:
		Vec3 pos;
	};
	// default vs rotates and translates vertices
	// does not touch attributes
	typedef DefaultVertexShader<Vertex> VertexShader;
	// gs colors vertices using their index from a table
	// every two triangles are colored from the same entry
	class GeometryShader
	{
	public:
		class Output
		{
		public:
			Output() = default;
			Output( const Vec3& pos )
				:
				pos( pos )
			{}
			Output( const Vec3& pos,const Output& src )
				:
				color( src.color ),
				pos( pos )
			{}
			Output( const Vec3& pos,const Color& color )
				:
				color( color ),
				pos( pos )
			{}
			Output& operator+=( const Output& rhs )
			{
				pos += rhs.pos;
				return *this;
			}
			Output operator+( const Output& rhs ) const
			{
				return Output( *this ) += rhs;
			}
			Output& operator-=( const Output& rhs )
			{
				pos -= rhs.pos;
				return *this;
			}
			Output operator-( const Output& rhs ) const
			{
				return Output( *this ) -= rhs;
			}
			Output& operator*=( float rhs )
			{
				pos *= rhs;
				return *this;
			}
			Output operator*( float rhs ) const
			{
				return Output( *this ) *= rhs;
			}
			Output& operator/=( float rhs )
			{
				pos /= rhs;
				return *this;
			}
			Output operator/( float rhs ) const
			{
				return Output( *this ) /= rhs;
			}
		public:
			Vec3 pos;
			Color color;
		};
	public:

		/*
			its useful as we dont store colour values anymore

			we just determine the values based on how the geometry shader assigns them
		*/
		Triangle<Output> operator()( const VertexShader::Output& in0,const VertexShader::Output& in1,const VertexShader::Output& in2,size_t triangle_index ) const
		{
			/* 
				so, geometry shader returns a triangle 
				
				we can return a color specified for each triangle assembled correctly based on the index

				into the colours list, preventing the need for duplicated vertices in the block vertex shader.
			*/

			/*
				returns the same Vec3 position but the color defined in the shader is the specified indexing side.

				/2 > as a face is 2 triangles, so this geoemtry shaders gives one face made of two triangles the same color

			*/
			return{ 
				{ in0.pos,triangle_colors[triangle_index/2] },
				{ in1.pos,triangle_colors[triangle_index/2] },
				{ in2.pos,triangle_colors[triangle_index/2] }
			};
		};
		/* bind a color vector which is indexed into in the geometry shade to create the cube solid colors. */
		void BindColors( std::vector<Color> colors )
		{
			triangle_colors = std::move( colors );
		}
	private:
		/* a vector of colors for the color of each triangle */
		std::vector<Color> triangle_colors;
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
			return in.color;
		}
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};