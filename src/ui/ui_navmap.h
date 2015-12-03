/**\file			ui_navmap.h
 * \author			Matt Zweig
 * \date			Created:  Saturday, May 28, 2011
 * \date			Modified: Monday, October 19, 2015
 * \brief			
 * \details
 */

#ifndef __H_UI_NAVMAP
#define __H_UI_NAVMAP

#include "engine/scenario.h"
#include "engine/sectors.h"
#include "graphics/image.h"
#include "graphics/video.h"
#include "ui/ui.h"
#include "utilities/coordinate.h"

class NavMap: public Widget {
	public:
		NavMap( int x, int y, int w, int h, Coordinate center, Scenario *scenario );
		~NavMap();

		void Draw( int relx = 0, int rely = 0 );

		void SetAlpha( float newAlpha ) { alpha = newAlpha; }
		void SetCenter( Coordinate newCenter ) { center = newCenter; }
		void SetScale( float newScale ) { scale = newScale; }

		void SetZoomable( bool canZoom ) { zoomable = canZoom; }
		void SetPannable( bool canPan ) { pannable = canPan; }

		Coordinate GetCenter() { return center; }
		float GetScale() { return scale; }

		Coordinate ClickToWorld( Coordinate click );
		Coordinate WorldToClick( Coordinate world );
		Coordinate WorldToScreen( Coordinate world );

		string GetType( void ) { return string("Map"); }
		virtual int GetMask( void ) { return WIDGET_MAP; }

	protected:
		virtual bool MouseLUp( int xi, int yi );
		virtual bool MouseLDown( int xi, int yi );
		virtual bool MouseWUp( int xi, int yi );
		virtual bool MouseWDown( int xi, int yi );
		virtual bool MouseDrag( int xi, int yi );

	private:
		bool SectorNearClick(Sector *sector, Coordinate click);

		float alpha;
		float scale;
		Coordinate center;

		bool zoomable;
		bool pannable;

		Scenario* scenario;

		Sector* selectedSector;

		static Font* NavMapFont;
};

#endif // __H_UI_NAVMAP

