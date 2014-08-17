/**\file			ui_map.cpp
 * \author			Matt Zweig
 * \date			Created:  Saturday, May 28, 2011
 * \date			Modified: Sunday, August 17, 2014
 * \brief			Nav Map Widget
 * \details
 */

#include "includes.h"
#include "ui/ui_navmap.h"
#include "sprites/planets.h"
#include "sprites/effects.h"
#include "utilities/timer.h"

/** \addtogroup UI
 * @{
 */

/**\class NavMap
 * \brief Widget for displaying Sprites
 *
 */


#define MAP_ZOOM_RATIO 1.1f ///< The rate at which the Map Zooms in and out.

Font *NavMap::MapFont = NULL;

/** \brief Map Constructor
 *
 */
NavMap::NavMap( int x, int y, int w, int h, Coordinate center, SpriteManager* sprites )
{
	this->x = x;
	this->y = y;

	this->w = w;
	this->h = h;
	this->center = center;
	this->sprites = sprites;

	spriteTypes = ( DRAW_ORDER_PLAYER   |
	                DRAW_ORDER_PLANET );

	// Show sprites only if this option is set.
	if( OPTION(int,"options/development/ships-worldmap") ) {
		spriteTypes |= DRAW_ORDER_SHIP;
	}

	alpha = 1;

	float size = (h < w) ? h : w; // Max of Height and Width

	// Initially strech the Map so that it covers all QuadTrees
	float north, south, east, west, edge;
	sprites->GetBoundaries(&north, &south, &east, &west);
	// edge is the maximum distance from zero of north, south, east, and west
	edge = (north > -south) ? north : -south;
	edge = (edge > -west) ? edge : -west;
	edge = (edge > east) ? edge : east;
	scale = (size) / (0.5 * (edge + QUADRANTSIZE));

	if( MapFont == NULL )
	{
		MapFont = new Font( SKIN("Skin/HUD/Map/Font") );
		MapFont->SetColor( Color( SKIN("Skin/HUD/Map/Color") ) );
		MapFont->SetSize( convertTo<int>( SKIN("Skin/HUD/Map/Size") ) );
	}

	zoomable = true;
	pannable = true;
}

/** \brief Map Destructor
 *
 */
NavMap::~NavMap()
{
	sprites = NULL;
}

/** \brief Draw Map
 *
 */
void NavMap::Draw( int relx, int rely )
{
	list<Sprite*> *spriteList;
	list<Sprite*>::iterator iter;

	// These variables are used for almost every sprite symbol
	Coordinate pos, pos2;
	Color col, field;

	// The Backdrop
	Video::DrawRect( relx + GetX(), rely + GetY(), w, h, BLACK, alpha);

	Video::SetCropRect( relx + GetX(), rely + GetY(), w, h );

	// TODO: Quadrant lines should be be drawn correctly.

	// The Quadrant Lines
	Coordinate i, top, bottom;
	bottom = ClickToWorld( Coordinate(GetX(),GetY()) );
	bottom = Coordinate( TO_INT(bottom.GetX() / (2*QUADRANTSIZE)),
	                     TO_INT(bottom.GetY() / (2*QUADRANTSIZE)) );
	bottom *= 2*QUADRANTSIZE;
	bottom -= Coordinate(QUADRANTSIZE, QUADRANTSIZE);

	top = ClickToWorld( Coordinate(GetX()+w, GetY()+h) );
	top = Coordinate( TO_INT(top.GetX() / (2*QUADRANTSIZE)),
	                  TO_INT(top.GetY() / (2*QUADRANTSIZE)) );
	top *= 2*QUADRANTSIZE;
	top += Coordinate(QUADRANTSIZE, QUADRANTSIZE);

	for( i = bottom;
	     i.GetX() <= top.GetX() ||
	     i.GetY() <= top.GetY() ;
	     i += Coordinate(2*QUADRANTSIZE, 2*QUADRANTSIZE) )
	{
		Coordinate point = WorldToScreen( i );
		Video::DrawLine( relx + GetX()     , point.GetY(),
						 relx + GetX() + w , point.GetY(), 0.07, 0.07, 0.07 , alpha );
		Video::DrawLine( point.GetX(), rely + GetY(),
						 point.GetX(), rely + GetY() + h , 0.07, 0.07, 0.07, alpha );
	}

	// Draw the Sprites
	spriteList = sprites->GetSprites( spriteTypes );
	for( iter = spriteList->begin(); iter != spriteList->end(); ++iter )
	{
		col = (*iter)->GetRadarColor();
		pos = WorldToScreen( (*iter)->GetWorldPosition() );

		switch( (*iter)->GetDrawOrder() ) {
			case DRAW_ORDER_PLAYER:
				Video::DrawFilledCircle( pos, (Timer::GetRealTicks() % 2500) / 50, col, 0.6f * (alpha - (float)(Timer::GetRealTicks() % 2500) / 2500.0f) );
				Video::DrawFilledCircle( pos, 2, col, alpha );
				break;

			case DRAW_ORDER_SHIP:
			case DRAW_ORDER_PROJECTILE:
			case DRAW_ORDER_EFFECT:
				Video::DrawFilledCircle( pos, 2, col, alpha );
				break;

			case DRAW_ORDER_PLANET:
				field = ((Planet*)(*iter))->GetAlliance()->GetColor();
				// Draw a gradient for influence
				for(float i = 1; i < 10; i += 0.25f) {
					Video::DrawFilledCircle( pos, (((Planet*)(*iter))->GetInfluence() * scale) / i, field, alpha * 0.05f );
				}
				Video::DrawCircle( pos, 3, 1, col, alpha );
				break;

			default:
				LogMsg(WARN,"Unknown Sprite type (0x%04X) being drawn in the Map.", (*iter)->GetDrawOrder() );
		}
	}

	// Do a second pass to draw planet Names on top
	for( iter = spriteList->begin(); iter != spriteList->end(); ++iter )
	{
		if( (*iter)->GetDrawOrder() == DRAW_ORDER_PLANET )
		{
			pos = WorldToScreen( (*iter)->GetWorldPosition() );
			MapFont->Render( pos.GetX()+5, pos.GetY(), ((Planet*)(*iter))->GetName().c_str() );
		}
	}

	// TODO: Draw Radar Visibility

	Video::UnsetCropRect();

	delete spriteList;
	spriteList = NULL;
}

/** \brief Convert click coordinates to World Coordinates
 */
Coordinate NavMap::ClickToWorld( Coordinate click ) {
	Coordinate world = click;
	world -= Coordinate( GetX(), GetY() ); // Offset by this Widget's origin
	world -= Coordinate( w/2, h/2 ); // Offset by the center of this widget
	world /= scale; // Descale the click
	world += center;
	return world;
}

/** \brief Convert world coordinates to relative click Coordinates
 */
Coordinate NavMap::WorldToClick( Coordinate world ) {
	Coordinate click = world;
	click -= center;
	click *= scale; // Descale the click
	click += Coordinate( w/2, h/2 ); // Offset by the center of this widget
	click += Coordinate( GetX(), GetY() ); // Offset by this Widget's origin
	return click;
}

/** \brief Convert world coordinates to screen Coordinates
 */
Coordinate NavMap::WorldToScreen( Coordinate world ) {
	Coordinate screen = world;
	screen -= center;
	screen *= scale; // Descale the screen
	screen += Coordinate( w/2, h/2 ); // Offset by the center of this widget
	screen += Coordinate( GetAbsX(), GetAbsY() ); // Offset by the absolute screen coordinate
	return screen;
}

bool NavMap::MouseLUp( int xi, int yi )
{
	Widget::MouseLUp( xi, yi );
	return false;
}

bool NavMap::MouseLDown( int xi, int yi )
{
	Widget::MouseLDown( xi, yi );
	return false;
}

/** \brief Pan the Map
 */
bool NavMap::MouseDrag( int xi, int yi )
{
	if( pannable )
	{
		center -= Coordinate( (xi-x) - dragX , ((yi-y) - dragY) ) / scale;
		dragX = xi-x;
		dragY = yi-y;
	}
	Widget::MouseDrag( xi, yi );
	return true;
}

/** \brief Zoom the map in
 */
bool NavMap::MouseWUp( int xi, int yi ) {
	//cout << "MouseWUp" << ClickToWorld( Coordinate(xi,yi) ) <<endl;
	if( zoomable )
	{
		scale *= MAP_ZOOM_RATIO;
	}
	Widget::MouseWUp( xi, yi );
	return true;
}

/** \brief Zoom the map out
 */
bool NavMap::MouseWDown( int xi, int yi ) {
	//cout << "MouseWDown" << ClickToWorld( Coordinate(xi,yi) ) <<endl;
	if( zoomable )
	{
		scale /= MAP_ZOOM_RATIO;
	}
	Widget::MouseWDown( xi, yi );
	return true;
}

/** @} */

