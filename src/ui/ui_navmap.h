/**\file			ui_navmap.h
 * \author			Matt Zweig
 * \date			Created:  Saturday, May 28, 2011
 * \date			Modified: Sunday, August 17, 2014
 * \brief			
 * \details
 */

#ifndef __H_UI_MAP
#define __H_UI_MAP

#include "graphics/image.h"
#include "graphics/video.h"
#include "ui/ui.h"
#include "sprites/spritemanager.h"
#include "utilities/coordinate.h"

class NavMap: public Widget {
	public:
		NavMap( int x, int y, int w, int h, Coordinate center, SpriteManager* sprites );
		~NavMap();

		void Draw( int relx = 0, int rely = 0 );

		void SetAlpha( float newAlpha ) { alpha = newAlpha; }
		void SetCenter( Coordinate newCenter ) { center = newCenter; }
		void SetScale( float newScale ) { scale = newScale; }
		void SetFilter( int spriteFlags ) { spriteTypes = spriteFlags; }

		void SetZoomable( bool canZoom ) { zoomable = canZoom; }
		void SetPannable( bool canPan ) { pannable = canPan; }

		Coordinate GetCenter() { return center; }
		float GetScale() { return scale; }

		Coordinate ClickToWorld( Coordinate click );
		Coordinate WorldToClick( Coordinate world );
		Coordinate WorldToScreen( Coordinate world );

		string GetType( void ) { return string("NavMap"); }
		virtual int GetMask( void ) { return WIDGET_NAVMAP; }
	protected:
		virtual bool MouseLUp( int xi, int yi );
		virtual bool MouseLDown( int xi, int yi );
		virtual bool MouseWUp( int xi, int yi );
		virtual bool MouseWDown( int xi, int yi );
		virtual bool MouseDrag( int xi, int yi );

	private:
		int spriteTypes;
		float alpha;
		float scale;
		Coordinate center;

		bool zoomable;
		bool pannable;

		SpriteManager* sprites;

		static Font *MapFont;
};

#endif // __H_UI_MAP

