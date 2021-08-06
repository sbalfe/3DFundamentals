#pragma once

#include "Vec3.h"
#include "Mat3.h"
#include "IndexedTriangleList.h"


/*

	the axis of these calculatiosn are z pointing upwards
	
*/
class Sphere
{
public:
	template<class V>
	static IndexedTriangleList<V> GetPlain( float radius = 1.0f,int latDiv = 12,int longDiv = 24 )
	{

		/* base vertex is just pointing up in the z direction based on sphere radius size */
		const Vec3 base = { 0.0f,0.0f,radius };

		/* 
			latitude is just half circle therefore PI
			obtain the change in each angle by usins this formula
		*/

		const float lattitudeAngle = PI / latDiv;

		/* longitude is the circumference of circle therefore 2pi / longitudue divisions */
		const float longitudeAngle = 2.0f * PI / longDiv;


		std::vector<V> vertices;

		/* skip north pole by starting at 1 */
		for( int iLat = 1; iLat < latDiv; iLat++ )
		{
			/* 
				we rotate the original base vector which points radius in z 
				
				about the x axis by the current latitude mult the change in angle over latitude changes
				
			*/
			const auto latBase = base * Mat3::RotationX( lattitudeAngle * iLat );
			for( int iLong = 0; iLong < longDiv; iLong++ )
			{
				/* place the vertex */
				vertices.emplace_back(latBase * Mat3::RotationZ(longitudeAngle * iLong));

				/* 
					multiply the current lattitude by a Z rotation of longitude angle 
					so rotates down from top Z and for every scaled down angle it does a full perimeter of the Z axis rotation
					placing a vertex at each point
				*/
			
			}
		}

		// add the cap vertices
		const auto iNorthPole = vertices.size();
		vertices.emplace_back(base); 
		const auto iSouthPole = vertices.size();
		vertices.emplace_back(-base); 
		
		const auto calcIdx = [latDiv,longDiv]( int iLat,int iLong )
			/* 
				the current latitude multiplied by how many longitutide divisions
				1 * 24 * numberOfLong

				same method as used to index a 1d array as if it were a 2d

				the index we want we first move down the 2d array by whatever y we are scanning
				then scan along y using the offset. 

				in this case replace x and y with lat and long respectively. 

				x * dimension_y + y
			*/
			{ return iLat * longDiv + iLong; };
		std::vector<size_t> indices;

		/* ignore 2 for the north and south pole */
		for( int iLat = 0; iLat < latDiv - 2; iLat++ )
		{
			/* ignore 1 for the latitude line */
			for( int iLong = 0; iLong < longDiv - 1; iLong++ )
			{
				/* calculates how far in the index position they are passed on lat and long division count*/
				indices.push_back( calcIdx( iLat,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong + 1 ) );
			}
			// wrap band

			/*	
				with the current latitude in place 
				
				this indexes the ignore latitude indices at the start and end as required 

				basically just filling in where the latitude was skipped to wrap entirelyt

				fixed at a certain latitude calculate the final longitude to get the top right vertex
				then ofc apply same concepts to obtain the indices for the 6 points of the square. 
			*/

			/* first calculate the lat on the final division line */
			indices.push_back( calcIdx( iLat,longDiv - 1 ) );
			indices.push_back( calcIdx( iLat + 1,longDiv - 1 ) );
			/* then calculate at the beginning long div line */
			indices.push_back( calcIdx( iLat,0 ) );
			indices.push_back( calcIdx( iLat,0 ) );
			indices.push_back( calcIdx( iLat + 1,longDiv - 1 ) );
			indices.push_back( calcIdx( iLat + 1,0 ) );			
		}

		// cap fans, recall that the number of divisions on longitude stays same
		// here we dont require lat as its just the north and south pole
		for( int iLong = 0; iLong < longDiv - 1; iLong++ )
		{
			// north, rememember all polygons on top map to the same vertex of north, same for south
			// so its 2nd first division , ignroing the pole
			indices.push_back( iNorthPole );
			indices.push_back( calcIdx( 0,iLong ) );
			indices.push_back( calcIdx( 0,iLong + 1 ) );
			// south, and its the 2nd last division , igorning th epole
			indices.push_back( calcIdx( latDiv - 2,iLong + 1 ) );
			indices.push_back( calcIdx( latDiv - 2,iLong ) );
			indices.push_back( iSouthPole );
		}
		// wrap triangles, fills out a single indice for where wrapping latituded should go
		/*
			so the vertices for north wrap

			are the north pole central vertex of course

			the index of lat 0 and the last longdiv

			the index of lat 0 and first longdiv.

			basically joins the longitude together with a wrap 
		*/
		// north
		indices.push_back( iNorthPole );
		indices.push_back( calcIdx( 0,longDiv - 1 ) );
		indices.push_back( calcIdx( 0,0 ) );
		// south
		indices.push_back( calcIdx( latDiv - 2,0 ) );
		indices.push_back( calcIdx( latDiv - 2,longDiv - 1 ) );
		indices.push_back( iSouthPole );


		return{ std::move( vertices ),std::move( indices ) };
	}
	template<class V>
	static IndexedTriangleList<V> GetPlainNormals( float radius = 1.0f,int latDiv = 12,int longDiv = 24 )
	{
		auto sphere = GetPlain<V>( radius,latDiv,longDiv );
		for( auto& v : sphere.vertices )
		{
			/* just a single vertex so the normal is just its normalized position vector */
			v.n = v.pos.GetNormalized();
		}
		return sphere;
	}
};