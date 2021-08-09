#pragma once

#include "Scene.h"
#include "Cube.h"
#include "Mat.h"
#include "Pipeline.h"
#include "SpecularPhongPointEffect.h"
#include "SolidEffect.h"
#include "Sphere.h"
#include "MouseTracker.h"
#include "VertexLightTexturedEffect.h"
#include "RippleVertexSpecularPhongEffect.h"
#include "Plane.h"
#include "NormiePipe.h"

struct PointDiffuseParams
{
	static constexpr float linear_attenuation = 0.9f;
	static constexpr float quadradic_attenuation = 0.6f;
	static constexpr float constant_attenuation = 0.682f;
};

struct SpecularParams
{
	static constexpr float specular_power = 30.0f;
	static constexpr float specular_intensity = 0.6f;
};

class SpecularPhongPointScene : public Scene
{
	using SpecularPhongPointEffect = SpecularPhongPointEffect<PointDiffuseParams,SpecularParams>;
	using VertexLightTexturedEffect = VertexLightTexturedEffect<PointDiffuseParams>;
	using RippleVertexSpecularPhongEffect = RippleVertexSpecularPhongEffect<PointDiffuseParams,SpecularParams>;
public:
	struct Wall
	{
		const Surface* pTex;
		IndexedTriangleList<VertexLightTexturedEffect::Vertex> model;
		Mat4 world;
	};
public:
	typedef ::Pipeline<SpecularPhongPointEffect> Pipeline;
	typedef ::Pipeline<SolidEffect> LightIndicatorPipeline;
	typedef ::Pipeline<VertexLightTexturedEffect> WallPipeline;
	typedef ::Pipeline<RippleVertexSpecularPhongEffect> RipplePipeline;
	typedef Pipeline::Vertex Vertex;
public:
	SpecularPhongPointScene( Graphics& gfx )
		:
		pZb( std::make_shared<ZBuffer>( gfx.ScreenWidth,gfx.ScreenHeight ) ),
		pipeline( gfx,pZb ),
		liPipeline( gfx,pZb ),
		wPipeline( gfx,pZb ),
		rPipeline( gfx,pZb ),
		Scene( "phong point shader scene free mesh" )
	{
		// adjust suzanne model
		itlist.AdjustToTrueCenter();
		// set light sphere colors
		for( auto& v : lightIndicator.vertices )
		{
			v.color = Colors::White;
		}
		// load ceiling/walls/floor
		walls.push_back( {
			&tCeiling,
			Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>( 20,20,width,width,tScaleCeiling ),
			Mat4::RotationX( -PI / 2.0f ) * Mat4::Translation( 0.0f,height / 2.0f,0.0f )
		} );
		for( int i = 0; i < 4; i++ )
		{
			walls.push_back( {
				&tWall,
				Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>( 20,20,width,height,tScaleWall ),
				Mat4::Translation( 0.0f,0.0f,width / 2.0f ) * Mat4::RotationY( float( i ) * PI / 2.0f )
			} );
		}
		walls.push_back( {
			&tFloor,
			Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>( 20,20,width,width,tScaleFloor ),
			Mat4::RotationX( PI / 2.0 ) * Mat4::Translation( 0.0f,-height / 2.0f,0.0f )
		} );
	}
	virtual void Update( Keyboard& kbd,Mouse& mouse,float dt ) override
	{
		t += dt;

		if( kbd.KeyIsPressed( 'W' ) )
		{
			/* W is forward therefore add 1 to Z */

			/*
				mulitply this scalar value by the inverse inverse , therefore current camera rotation value

				then apply the changed in speed over pressing w each frame.
			*/
			cam_pos += Vec4{ 0.0f,0.0f,1.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'A' ) )
		{
			/* A left therefore negate x*/
			cam_pos += Vec4{ -1.0f,0.0f,0.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'S' ) )
		{
			cam_pos += Vec4{ 0.0f,0.0f,-1.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'D' ) )
		{
			cam_pos += Vec4{ 1.0f,0.0f,0.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'C' ) )
		{
			cam_pos += Vec4{ 0.0f,1.0f,0.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'Z' ) )
		{
			cam_pos += Vec4{ 0.0f,-1.0f,0.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'Q' ) )
		{
			cam_rot_inv = cam_rot_inv * Mat4::RotationZ( cam_roll_speed * dt );
		}
		if( kbd.KeyIsPressed( 'E' ) )
		{
			cam_rot_inv = cam_rot_inv * Mat4::RotationZ( -cam_roll_speed * dt );
		}

		/*
			check for any mouse events
		*/
		while( !mouse.IsEmpty() )
		{
			/*
				read the event from the mouse 
			*/
			const auto e = mouse.Read();

			/*
				Left press down 
			*/
			switch( e.GetType() )
			{
			case Mouse::Event::Type::LPress:

				/*
					engage at th current position 
				*/
				mt.Engage( e.GetPos() );
				break;
			/*
				let go of mouse therefore call release to disengage
			*/
			case Mouse::Event::Type::LRelease:
				mt.Release();
				break;

			/*
				if the mouse has been detected to have moved 
			*/
			case Mouse::Event::Type::Move:

				/*
					check to see if it has been clicked and held downwards
				*/
				if( mt.Engaged() )
				{
					/*
						calculate the change which returns an x y coordinate to show where its new postion
					*/
					const auto delta = mt.Move( e.GetPos() );

					/*
					* multiply cam_rot_inv to apply a Y and X rotation of the delta value times the change in pixels
					* for angle to obtain the angle that the mouse change actually equivalates to
					* thus enabling a click and drag value
					* its negative the values of x and y as to apply an inverse as that when you click and hold if u go right
					* you want the world to move left
					*/

					/*
						apply the rotation to the already applied rotation as to keep into account what is the orientation of the camera 
					*/

					/*
						this camera rotation is actually an inverse value to apply to the world 
					*/
					cam_rot_inv = cam_rot_inv
						* Mat4::RotationY( (float)-delta.x * htrack )
						* Mat4::RotationX( (float)-delta.y * vtrack );
				}
				break;
			}
		}

		theta_y = wrap_angle( t * rotspeed );
		l_pos.y = l_height_amplitude * sin( wrap_angle( (PI / (2.0f * l_height_amplitude)) * t ) );

		rPipeline.effect.vs.SetTime( t );
	}
	virtual void Draw() override
	{
		pipeline.BeginFrame();

		const auto proj = Mat4::ProjectionHFOV( hfov,aspect_ratio,0.2f,6.0f );

		/*
			translates the view in the opposite position to the camera position

			as the view is always is the inverse of where the camera moves to. 

			cam rot inv is applied, this this is the flipped orientation but is used to translate

			our scene with respect to the camera
		*/
		const auto view = Mat4::Translation( -cam_pos ) * cam_rot_inv;

		// render suzanne
		pipeline.effect.vs.BindWorldView(
			Mat4::RotationX( theta_x ) *
			Mat4::RotationY( theta_y ) *
			Mat4::RotationZ( theta_z ) *
			Mat4::Scaling( scale ) *
			Mat4::Translation( mod_pos ) *
			/*
				multiply this object by camera pos to obtain the position as it moves around inversely
			*/
			view
		);
		pipeline.effect.vs.BindProjection( proj );
		/*
			place our light into the same view as the other objects in the scene
			
			world > view space is just a translation from world space to obtain its value with respect to the camera 
		*/
		pipeline.effect.ps.SetLightPosition( l_pos * view );
		pipeline.effect.ps.SetAmbientLight( l_ambient );
		pipeline.effect.ps.SetDiffuseLight( l );
		pipeline.Draw( itlist );

		// draw light indicator with different pipeline
		// don't call beginframe on this pipeline b/c wanna keep z buffer contents
		// (don't like this assymetry but we'll live with it for now)
		liPipeline.effect.vs.BindWorldView( Mat4::Translation( l_pos ) * view );
		liPipeline.effect.vs.BindProjection( proj );
		liPipeline.Draw( lightIndicator);

		// draw walls (ceiling floor)
		wPipeline.effect.vs.SetLightPosition( l_pos * view );
		wPipeline.effect.vs.BindProjection( proj );
		wPipeline.effect.vs.SetAmbientLight( l_ambient );
		wPipeline.effect.vs.SetDiffuseLight( l );
		for( const auto& w : walls )
		{
			wPipeline.effect.vs.BindWorldView( w.world * view );
			wPipeline.effect.ps.BindTexture( *w.pTex );
			wPipeline.Draw( w.model );
		}

		// draw ripple plane
		rPipeline.effect.ps.BindTexture( tSauron );
		rPipeline.effect.ps.SetLightPosition( l_pos * view );
		rPipeline.effect.vs.BindWorldView( sauronWorld * view );
		rPipeline.effect.vs.BindProjection( proj );
		rPipeline.effect.ps.SetAmbientLight( l_ambient );
		rPipeline.effect.ps.SetDiffuseLight( l );
		rPipeline.Draw( sauron );
	}
private:
	float t = 0.0f;
	// scene params
	static constexpr float width = 4.0f;
	static constexpr float height = 1.75f;
	// pipelines
	std::shared_ptr<ZBuffer> pZb;
	Pipeline pipeline;
	LightIndicatorPipeline liPipeline;
	WallPipeline wPipeline;
	RipplePipeline rPipeline;
	// fov
	static constexpr float aspect_ratio = 1.33333f;
	static constexpr float hfov = 85.0f;
	static constexpr float vfov = hfov / aspect_ratio;
	// camera stuff
	MouseTracker mt;

	/*
		calculate ratio  of horizontal and vertical FOV over screen height/width

		this calculates a tiny value in how many degrees to apply for every pixel 
	*/
	static constexpr float htrack = to_rad( hfov ) / (float)Graphics::ScreenWidth;
	static constexpr float vtrack = to_rad( vfov ) / (float)Graphics::ScreenHeight;
	static constexpr float cam_speed = 1.0f;
	static constexpr float cam_roll_speed = PI;


	/*
		camera position 
	*/
	Vec3 cam_pos = { 0.0f,0.0f,0.0f };

	/*
		holds the inverse camera value, inverse in regards to the world view 
	*/
	Mat4 cam_rot_inv = Mat4::Identity();
	// suzanne model stuff
	IndexedTriangleList<Vertex> itlist = IndexedTriangleList<SpecularPhongPointScene::Vertex>::LoadNormals( "models\\suzanne.obj" );
	/*
		model position, used for suzanne to apply model matrix. 
	*/
	Vec3 mod_pos = { 1.2f,-0.4f,1.2f };
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	float rotspeed = PI / 4.0f;
	float scale = 0.4;
	// light stuff
	IndexedTriangleList<SolidEffect::Vertex> lightIndicator = Sphere::GetPlain<SolidEffect::Vertex>( 0.05f );
	static constexpr float l_height_amplitude = 0.7f;
	static constexpr float l_height_period = 3.713f;
	Vec4 l_pos = { 0.0f,0.0f,0.0f,1.0f };
	Vec3 l = { 1.0f,1.0f,1.0f };
	Vec3 l_ambient = { 0.35f,0.35f,0.35f };
	// wall stuff
	static constexpr float tScaleCeiling = 0.5f;
	static constexpr float tScaleWall = 0.65f;
	static constexpr float tScaleFloor = 0.65f;
	Surface tCeiling = Surface::FromFile( L"Images\\ceiling.png" );
	Surface tWall = Surface::FromFile( L"Images\\stonewall.png" );
	Surface tFloor = Surface::FromFile( L"Images\\floor.png" );
	std::vector<Wall> walls;
	// ripple stuff
	static constexpr float sauronSize = 0.6f;
	Mat4 sauronWorld = Mat4::RotationX( PI / 2.0f ) * Mat4::Translation( 0.3f,-0.8,0.0f );
	Surface tSauron = Surface::FromFile( L"Images\\sauron-bhole-100x100.png" );
	IndexedTriangleList<RippleVertexSpecularPhongEffect::Vertex> sauron = Plane::GetSkinned<RippleVertexSpecularPhongEffect::Vertex>( 50,10,sauronSize,sauronSize,0.6f );
};
