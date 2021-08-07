#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

// flat shading with vertex normals
class SpecularPhongPointEffect
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
				n( src.n ),
				worldPos( src.worldPos ),
				pos( pos )
			{}
			Output( const Vec3& pos,const Vec3& n,const Vec3& worldPos )
				:
				n( n ),
				pos( pos ),
				worldPos( worldPos )
			{}
			Output& operator+=( const Output& rhs )
			{
				pos += rhs.pos;
				n += rhs.n;
				worldPos += rhs.worldPos;
				return *this;
			}
			Output operator+( const Output& rhs ) const
			{
				return Output( *this ) += rhs;
			}
			Output& operator-=( const Output& rhs )
			{
				pos -= rhs.pos;
				n -= rhs.n;
				worldPos -= rhs.worldPos;
				return *this;
			}
			Output operator-( const Output& rhs ) const
			{
				return Output( *this ) -= rhs;
			}
			Output& operator*=( float rhs )
			{
				pos *= rhs;
				n *= rhs;
				worldPos *= rhs;
				return *this;
			}
			Output operator*( float rhs ) const
			{
				return Output( *this ) *= rhs;
			}
			Output& operator/=( float rhs )
			{
				pos /= rhs;
				n /= rhs;
				worldPos /= rhs;
				return *this;
			}
			Output operator/( float rhs ) const
			{
				return Output( *this ) /= rhs;
			}
		public:
			Vec3 pos;
			Vec3 n;
			Vec3 worldPos;
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
			const auto pos = v.pos * rotation + translation;
			return{ pos,v.n * rotation,pos };
		}
	private:
		Mat3 rotation;
		Vec3 translation;
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
			// re-normalize interpolated surface normal, rather than calculate it each time within the function below

			/*
				makes length of surf norm become 1
			*/
			const auto surf_norm = in.n.GetNormalized();

			// vertex to light data

			/*
				obtains the direction of world pos from light pos 

				imagine this as the arrow from teh vertex to light position

				world position is the original coordinates in 3D space not just 2D screen coordinates for dynamic frag shader shading 
			*/

			/* this normalizes the light position and world position to obtain*/
			const auto v_to_l = light_pos - in.worldPos;
			const auto dist = v_to_l.Len();
			const auto dir = v_to_l / dist;

			// calculate attenuation
			const auto attenuation = 1.0f /
				(constant_attenuation + linear_attenuation * dist + quadradic_attenuation * sq( dist ));

			// calculate intensity based on angle of incidence and attenuation
			const auto d = light_diffuse * attenuation * std::max( 0.0f,surf_norm * dir );

			// reflected light vector

			/*
				take dot of light position onto our world pos vertex 

				this dot is the projected value onto the surface normal plane
				unit vector therefore just becomes length of projected plane (length of w)

				multiply this by the normalized vector itself to obtain the w vector used in in the reflection angle calculation
			*/
			const auto w = surf_norm * (v_to_l * surf_norm);

			/*
				see ipad for derivation, chilli tutorial 16


			*/
			const auto r = w * 2.0f - v_to_l;

			// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function

			/*
				USE WORLD POS as its based on the eyes of us, i.e camera > the camera doesnt shift the world pos

				use the normalized value of the point we render as our view is always at the current 0,0,0 with no camera at least
			*/

			/*
				specular is a result of the light diffuse value i.e light color

				being multiplied by the specular intensity to the power of std::max(0.0f, phi (angle between reflected vector and current world space frag)

				flip r as the light is going into vertex , wheras the normalized world position points outwards

				apply a specular intensity and specular power to alter how powerful this effect is, how sharp the cut off is, creates   more metallic
				the higher it is

				specular intensity is how powerful this brightness actually is.


			*/

			/*
				specular power view on desmos as (cosx)^specular_power where cosx is the dot product

				as you can see the drop off at values further away from 1 on low power fades and scatters more for a more matte look

				wheras higher values drop off sooner at sharper angles leaving a more precise specular point good for metallic looks.
			*/
			const auto s = light_diffuse * specular_intensity * std::pow( std::max( 0.0f,-r.GetNormalized() * in.worldPos.GetNormalized() ),specular_power );
			
		// add diffuse+ambient, filter by material color, saturate and scale
			return Color( material_color.GetHadamard( d + light_ambient + s ).Saturate() * 255.0f );
		}
		void SetDiffuseLight( const Vec3& c )
		{
			light_diffuse = c;
		}
		void SetAmbientLight( const Vec3& c )
		{
			light_ambient = c;
		}
		void SetLightPosition( const Vec3& pos_in )
		{
			light_pos = pos_in;
		}
	private:
		Vec3 light_pos = { 0.0f,0.0f,0.5f };
		Vec3 light_diffuse = { 1.0f,1.0f,1.0f };
		Vec3 light_ambient = { 0.1f,0.1f,0.1f };
		Vec3 material_color = { 0.8f,0.85f,1.0f };
		// diffuse
		float linear_attenuation = 1.0f;
		float quadradic_attenuation = 2.619f;
		float constant_attenuation = 0.382f;
		// specular
		float specular_power = 200.0f;
		float specular_intensity = 0.6f;
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};