#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <string>

class Scene
{
public:
	Scene( const std::string& name )
		:
		name( name )
	{}
	virtual void Update( Keyboard& kbd,Mouse& mouse,float dt ) = 0;
	virtual void Draw() = 0; /* this is a pure virtual function, as this class is an abstract 
							 class, must override it and call it to add functioanlity*/
	virtual ~Scene() = default; /* uses the default destructor in this object , for readability */
	const std::string& GetName() const
	{
		return name;
	}
private:
	std::string name;
};