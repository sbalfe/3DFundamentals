#pragma once

#include <vector>
#include <array>
#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"
#include <string>

class Plane
{
public:
	template<class V>

	/* obtain the number of plain divisions, this is the number of tesselations essentially
		
		how much the plain becomes split by.
	*/
	static IndexedTriangleList<V> GetPlain( int divisions = 7,float size = 1.0f )
	{

		/*
			so if u split up into 7's

			vertice count will be an extra 1 for the sides remember

			starting from x = 0 / y = 0, 1/7 > 1 always , there is an initial one to add
		*/
		const int nVerticesSide = divisions + 1;

		
		std::vector<V> vertices( sq( nVerticesSide +1 ) );
		// give V a ctor for pos only %%%improvements
		{
			// side = 0.5 
			const float side = size / 2.0f;

			// div size = 1/7 
			const float divisionSize = size / float( divisions );

			// bl = (-0.5, -0.5, 0) 
			const Vec3 bottomLeft = { -side,-side,0.0f };

			// go from y = 0 to 7 and x = 0 to 7 , i is the index of the vertices
			// so there is 0-86 i in total.
			for( int y = 0,i = 0; y < nVerticesSide; y++ )
			{
				// y = y * 7, 0, 1/7, 2/7 ..... 1
				const float y_pos = float( y ) * divisionSize;
#
		
				for( int x = 0; x < nVerticesSide; x++,i++ )
				{
					/* calculate the actual vec 3 coordinates starting from the inital bottom left
					
						example 

						x = 3

						y = 3/7

						dS = 1/7

						vertices[i].pos = (-0.5, -0.5, 0 )+ (3 * 1/7, 3/7, 0)

						not really hard it goes through the plane and creates vertices inbetween it based 
						on a division size.
					*/

					OutputDebugStringA(std::to_string(i).c_str());
					vertices[i].pos = bottomLeft + Vec3{ float( x ) * divisionSize,y_pos,0.0f };
				}
			}
		}
		
		std::vector<size_t> indices;

		// 6 indices required to define a single plane
		// multiply by sqrt of division to obtian how many indices we requior
		// 7 * 6 = 46 indices down from 64 vertices required
		// ea
		indices.reserve( sq( divisions ) * 6 );
		{
			/* create lambda function here */
			const auto vxy2i = [nVerticesSide]( size_t x,size_t y )
			{

				/* 
					0, 1, 8, 
					
				*/
				return y * nVerticesSide + x;
			};

			/* y = 0 to 6, x = 0 to 6*/
			for( size_t y = 0; y < divisions; y++ )
			{
				for( size_t x = 0; x < divisions; x++ )
				{
					/* 
						create an array of size 4 to store each vertex of the tesselated squaree
							
					*/

					/*
						0, 1, 8, 9:
						9, 10, 17, 18

						..... creates the vertices to index each of the new vertex in

						imagine a rectangle

						divide into number of divisions squared = 49 in our case

						we have stored each vertex in the array

						thsi calcualtes each corner indice array to push back into the which a sample is shown

						each square in tesselation is of course 2 triangles 

						therefore for example the first indices become

						0, 1, 8, 9: pushes 0 / 8 / 1 / 1 / 8 /9 , where the push is index into the 2 triangles
					*/
					const std::array<size_t,4> indexArray =
					{ vxy2i( x,y ),vxy2i( x + 1,y ),vxy2i( x,y + 1 ),vxy2i( x + 1,y + 1 ) };
					indices.push_back( indexArray[0] );
					indices.push_back( indexArray[2] );
					indices.push_back( indexArray[1] );
					indices.push_back( indexArray[1] );
					indices.push_back( indexArray[2] );
					indices.push_back( indexArray[3] );
				}
			}
		}

		return{ std::move( vertices ),std::move( indices ) };
	}
	template<class V>
	static IndexedTriangleList<V> GetSkinned( int divisions = 7,float size = 1.0f )
	{
		/* so this fetches the list of vertices with each indice depending on plain divisions and size */
		auto itlist = GetPlain<V>( divisions,size );
		{
			const int nVerticesSide = divisions + 1;
			const float tDivisionSize = 1.0f / float( divisions );
			const Vec2 tBottomLeft = { 0.0f,1.0f };

			/* y = 0 to 7, x = 0 to 7, i = 0 to 63, same as vertices*/
			for( int y = 0,i = 0; y < nVerticesSide; y++ )
			{
				/* y of texture = -y * 1/7 
					-0 * 1/7
					-1 * 1/7
					-2/7
					-3/7
					.... -1
				*/
				const float y_t = -float( y ) * tDivisionSize;
				for( int x = 0; x < nVerticesSide; x++,i++ )
				{
					/*
						set the texture coordinate of each vertice to correspond to this vlaue

						startinf from the bottom left which is 0 ,1 
						it goes up from the bottom which translate to going down the v axis
						in negative increments which is up as v goes down and u goes right in the texturee space

						different to the original plain which is -0.5, -0.5 as its centered at 0,0 
					*/
					itlist.vertices[i].t = tBottomLeft + Vec2{ float( x ) * tDivisionSize,y_t };
				}
			}

		}

		return itlist;
	}
};