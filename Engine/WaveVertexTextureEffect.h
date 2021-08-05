#pragma once

#include "Pipeline.h"
#include <iostream>
class WaveVertexTextureEffect
{
public:
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
			t( src.t ),
			pos( pos )
		{}
		Vertex( const Vec3& pos,const Vec2& t )
			:
			t( t ),
			pos( pos )
		{}
		Vertex& operator+=( const Vertex& rhs )
		{
			pos += rhs.pos;
			t += rhs.t;
			return *this;
		}
		Vertex operator+( const Vertex& rhs ) const
		{
			return Vertex( *this ) += rhs;
		}
		Vertex& operator-=( const Vertex& rhs )
		{
			pos -= rhs.pos;
			t -= rhs.t;
			return *this;
		}
		Vertex operator-( const Vertex& rhs ) const
		{
			return Vertex( *this ) -= rhs;
		}
		Vertex& operator*=( float rhs )
		{
			pos *= rhs;
			t *= rhs;
			return *this;
		}
		Vertex operator*( float rhs ) const
		{
			return Vertex( *this ) *= rhs;
		}
		Vertex& operator/=( float rhs )
		{
			pos /= rhs;
			t /= rhs;
			return *this;
		}
		Vertex operator/( float rhs ) const
		{
			return Vertex( *this ) /= rhs;
		}
	public:
		Vec3 pos;
		Vec2 t;
	};
	// perturbes vertices in y axis in sin wave based on
	// x position and time
	class VertexShader
	{
	public:
		typedef Vertex Output;
	public:
		void BindRotation( const Mat3& rotation_in )
		{
			rotation = rotation_in;
		}
		void BindTranslation( const Vec3& translation_in )
		{
			translation = translation_in;
		}
		Output operator()( const Vertex& in ) const
		{
			Vec3 pos = in.pos * rotation + translation;
			/* 
				imagine this as playing with a sin graph

				multiply the original sin by an amplitidue to value to increase the overall wave effecct

				freqWave * x means take find where the x position is and mu

				time * freqScroll is the speed = distance/time
				speed x time = distance > offset value for the sin function

				freqWave * pos.x , imagine the sin function sin(x) on desmos, can make it 3 times more frequent using sin(3x)
				add this onto the offset value based on speed to control to give the effect that 
				there is a ripple through the plain as the sine wave of a set frequency 
				moves over our vertices.

				its on the viewport so remember up x, right y and into screen z

				test with x y and z and it will make sense the wave

				the wave essentially pushes each point in its corrsesponding direction based on the sin function

			*/
			pos.y+= amplitude * std::sin( (time * freqScroll) + pos.x * freqWave );
			return{ pos,in.t };
		}
		void SetTime( float t )

		{
			time = t;
		
		}
	private:
		Mat3 rotation;
		Vec3 translation;
		float time = 0.0f;
		float freqWave = 10.0f;
		float freqScroll = 10.0f;
		float amplitude = 0.06f;
	};
	// texture clamped ps
	class PixelShader
	{
	public:
		template<class Input>
		Color operator()( const Input& in ) const
		{
			return pTex->GetPixel(
				(unsigned int)std::min( in.t.x * tex_width + 0.5f,tex_xclamp ),
				(unsigned int)std::min( in.t.y * tex_height + 0.5f,tex_yclamp )
			);
		}
		void BindTexture( const std::wstring& filename )
		{
			pTex = std::make_unique<Surface>( Surface::FromFile( filename ) );
			tex_width = float( pTex->GetWidth() );
			tex_height = float( pTex->GetHeight() );
			tex_xclamp = tex_width - 1.0f;
			tex_yclamp = tex_height - 1.0f;
		}
	private:
		std::unique_ptr<Surface> pTex;
		float tex_width;
		float tex_height;
		float tex_xclamp;
		float tex_yclamp;
	};
public:
	VertexShader vs;
	PixelShader ps;
};