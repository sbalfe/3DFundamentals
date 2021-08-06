#pragma once

#include "Pipeline.h"
#include "DefaultVertexShader.h"
#include "DefaultGeometryShader.h"

// flat shading with vertex normals
class VertexFlatEffect
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
			n( src.n ),
			pos( pos )
		{}
		Vertex( const Vec3& pos,const Vec3& n )
			:
			n( n ),
			pos( pos )
		{}
		Vertex& operator+=( const Vertex& rhs )
		{
			pos += rhs.pos;
			n += rhs.n;
			return *this;
		}
		Vertex operator+( const Vertex& rhs ) const
		{
			return Vertex( *this ) += rhs;
		}
		Vertex& operator-=( const Vertex& rhs )
		{
			pos -= rhs.pos;
			n -= rhs.n;
			return *this;
		}
		Vertex operator-( const Vertex& rhs ) const
		{
			return Vertex( *this ) -= rhs;
		}
		Vertex& operator*=( float rhs )
		{
			pos *= rhs;
			n *= rhs;
			return *this;
		}
		Vertex operator*( float rhs ) const
		{
			return Vertex( *this ) *= rhs;
		}
		Vertex& operator/=( float rhs )
		{
			pos /= rhs;
			n /= rhs;
			return *this;
		}
		Vertex operator/( float rhs ) const
		{
			return Vertex( *this ) /= rhs;
		}
	public:
		Vec3 pos;
		Vec3 n;
	};
	// calculate color based on normal to light angle
	// no interpolation of color attribute
	class VertexShader
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
		void BindRotation( const Mat3& rotation_in )
		{
			rotation = rotation_in;
		}
		void BindTranslation( const Vec3& translation_in )
		{
			translation = translation_in;
		}
		Output operator()( const Vertex& v ) const
		{
			/* 
				calculate intensity based on angle of incidence
			
				diffuse is the color of the light

				to find d we just dot this with the larger of the values below
				which is the dot of the direction of light with the normal rotated, which must be negated
				To calculate the correct angle from the same way round 

				if there was no negation in the dir * normal then it would be the wrong angle technically

				take max as we dont want negative light as this causes issues overall, as its impossible in real world.
			*/
			const auto d = diffuse * std::max( 0.0f,-(v.n * rotation) * dir );
			// add diffuse+ambient, filter by material color, saturate and scale

			/*
				Hadamard product is just multiplying 2 vectors x y z in this case together

				product = (x^2, y^2, z^2) 

				d + ambient is the total light for this vertex now

				this essentially fines out what color the object is in this light by multiplying
				its releative reflectivities, where the higher r g b values are more strongly reflected

				saturation locks the value to 0 and 1, as this is the scale of light in total

				this is then scaled up to become proper color by 255 multiplication
			*/
			const auto c = color.GetHadamard( d + ambient ).Saturate() * 255.0f;

			/*
				stores position and colour in this specific vertex shader.
			*/
			return{ v.pos * rotation + translation,Color( c ) };
		}
		void SetDiffuseLight( const Vec3& c )
		{
			diffuse = { c.x,c.y,c.z };
		}
		void SetAmbientLight( const Vec3& c )
		{
			ambient = { c.x,c.y,c.z };
		}
		void SetLightDirection( const Vec3& dl )
		{
			assert( dl.LenSq() >= 0.001f );
			dir = dl.GetNormalized();
		}
		void SetMaterialColor( Color c )
		{
			color = Vec3( c );
		}
	private:
		Mat3 rotation;
		Vec3 translation;

		/* direction the light is pointing*/
		Vec3 dir = { 0.0f,0.0f,1.0f };

		/* create a 3 channel vector of light R/G/B, white light for this case*/
		Vec3 diffuse = { 1.0f,1.0f,1.0f };

		/* 
			ambient light is general global illumination to prevent complete darkness
			stray photons essentially, keeps darker sides visible.
		*/
		Vec3 ambient = { 0.1f,0.1f,0.1f };

		/* 
			color of the object
			how much of each r / g / b is our object going to actually reflect 

			the colour it reflects more, it appears to be more that basically
		*/

		Vec3 color = { 0.8f,0.85f,1.0f };
	};
	// default gs passes vertices through and outputs triangle
	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
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