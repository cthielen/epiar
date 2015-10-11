/**\file			ui_map.cpp
 * \author			Matt Zweig
 * \date			Created:  Saturday, May 28, 2011
 * \date			Modified: Sunday, October 11, 2015
 * \brief			Map Widget
 * \details
 */

#include "includes.h"
#include "ui/ui_map.h"
#include "sprites/planets.h"
#include "sprites/effects.h"
#include "utilities/timer.h"

/** \addtogroup UI
 * @{
 */

/**\class Map
 * \brief Widget for displaying Sprites
 *
 */


#define MAP_ZOOM_RATIO 1.1f ///< The rate at which the Map Zooms in and out.

Font *Map::MapFont = NULL;

/** \brief Map Constructor
 *
 */
Map::Map( int x, int y, int w, int h, Coordinate center, Sectors* sectors )
{
	this->x = x;
	this->y = y;

	this->w = w;
	this->h = h;
	this->center = center;
	this->sectors = sectors;

	alpha = 1;

	float size = (h < w) ? h : w; // Max of Height and Width

	// Initially strech the Map so that it covers all QuadTrees
	float north, south, east, west, edge;
	sectors->GetBoundaries(&north, &south, &east, &west);

	// edge is the maximum distance from zero of north, south, east, and west
	edge = (north > -south) ? north : -south;
	edge = (edge > -west) ? edge : -west;
	edge = (edge > east) ? edge : east;
	scale = (size) / (0.5 * edge);

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
Map::~Map()
{
	sectors = NULL;
}

/** \brief Draw Map
 *
 */
void Map::Draw( int relx, int rely ) {
	list<Sector*>::iterator iter;
	list<Sector*>* sectors = NULL;

	// These variables are used for almost every sprite symbol
	Coordinate pos, pos2;
	Color col, field;

	// Draw the backdrop
	Video::DrawRect( relx + GetX(), rely + GetY(), w, h, BLACK, alpha);

	Video::SetCropRect( relx + GetX(), rely + GetY(), w, h );

	// Draw the sectors
	for( iter = sectors->begin(); iter != sectors->end(); ++iter )
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
					Video::DrawFilledCircle( pos, (10 * scale) / i, field, alpha * 0.05f );
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
Coordinate Map::ClickToWorld( Coordinate click )
{
	Coordinate world = click;
	world -= Coordinate( GetX(), GetY() ); // Offset by this Widget's origin
	world -= Coordinate( w/2, h/2 ); // Offset by the center of this widget
	world /= scale; // Descale the click
	world += center;
	return world;
}

/** \brief Convert world coordinates to relative click Coordinates
 */
Coordinate Map::WorldToClick( Coordinate world )
{
	Coordinate click = world;
	click -= center;
	click *= scale; // Descale the click
	click += Coordinate( w/2, h/2 ); // Offset by the center of this widget
	click += Coordinate( GetX(), GetY() ); // Offset by this Widget's origin
	return click;
}

/** \brief Convert world coordinates to screen Coordinates
 */
Coordinate Map::WorldToScreen( Coordinate world )
{
	Coordinate screen = world;
	screen -= center;
	screen *= scale; // Descale the screen
	screen += Coordinate( w/2, h/2 ); // Offset by the center of this widget
	screen += Coordinate( GetAbsX(), GetAbsY() ); // Offset by the absolute screen coordinate
	return screen;
}

bool Map::MouseLUp( int xi, int yi )
{
	Widget::MouseLUp( xi, yi );
	return false;
}

bool Map::MouseLDown( int xi, int yi )
{
	Widget::MouseLDown( xi, yi );
	return false;
}

/** \brief Pan the Map
 */
bool Map::MouseDrag( int xi, int yi )
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
bool Map::MouseWUp( int xi, int yi )
{
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
bool Map::MouseWDown( int xi, int yi )
{
	//cout << "MouseWDown" << ClickToWorld( Coordinate(xi,yi) ) <<endl;
	if( zoomable )
	{
		scale /= MAP_ZOOM_RATIO;
	}
	Widget::MouseWDown( xi, yi );
	return true;
}

/** @} */

