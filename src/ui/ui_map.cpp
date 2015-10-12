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
Map::Map( int x, int y, int w, int h, Coordinate center, Sectors* sectors ) {
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
	scale = size / (1.5 * edge);

	cout << "north: " << north << endl;
	cout << "south: " << south << endl;
	cout << "east: " << east << endl;
	cout << "west: " << west << endl;
	cout << "scale is: " << scale << endl;

	if( MapFont == NULL ) {
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

	sectors = this->sectors->GetAllSectors();

	// Draw the backdrop
	Video::DrawRect( relx + GetX(), rely + GetY(), w, h, BLACK, alpha);

	Video::SetCropRect( relx + GetX(), rely + GetY(), w, h );

	// Draw sector connection lines
	for( iter = sectors->begin(); iter != sectors->end(); ++iter ) {
		Sector *sector = (Sector *)(*iter);
		list<string> neighbors = sector->GetNeighbors();
		list<string>::iterator neighborItr;
		Coordinate startPos;

		col = GREEN;
		startPos = WorldToScreen(Coordinate(sector->GetX(), sector->GetY()));

		for( neighborItr = neighbors.begin(); neighborItr != neighbors.end(); ++neighborItr ) {
			Sector *neighbor = (Sector *)this->sectors->Get( *neighborItr );
			assert(neighbor != NULL);

			Coordinate endPos;

			endPos = WorldToScreen(Coordinate(neighbor->GetX(), neighbor->GetY()));

			// Draw the connecting line
			Video::DrawLine( startPos.GetX(), startPos.GetY(), endPos.GetX(), endPos.GetY(), DARKGREY);
		}
	}

	// Draw the sectors
	for( iter = sectors->begin(); iter != sectors->end(); ++iter ) {
		Sector *sector = (Sector *)(*iter);

		col = BLUE;
		pos = WorldToScreen(Coordinate(sector->GetX(), sector->GetY()));

		field = sector->GetAlliance()->GetColor();

		// Draw a gradient for influence
		for(float i = 1; i < 10; i += 0.25f) {
			Video::DrawFilledCircle( pos, (10 * scale) / i, field, alpha * 0.05f );
		}
		Video::DrawCircle( pos, 3, 1, col, alpha );
	}

	// Do a second pass to draw planet Names on top
	for( iter = sectors->begin(); iter != sectors->end(); ++iter ) {
		Sector *sector = (Sector *)(*iter);
		pos = WorldToScreen(Coordinate(sector->GetX(), sector->GetY()));
		MapFont->Render( pos.GetX() + 5, pos.GetY(), sector->GetName().c_str() );
	}

	Video::UnsetCropRect();

	delete sectors;
	sectors = NULL;
}

/** \brief Convert click coordinates to World Coordinates
 */
Coordinate Map::ClickToWorld( Coordinate click ) {
	Coordinate world = click;

	world -= Coordinate( GetX(), GetY() ); // Offset by this Widget's origin
	world -= Coordinate( w / 2, h / 2 ); // Offset by the center of this widget
	world /= scale; // Descale the click
	world += center;

	return world;
}

/** \brief Convert world coordinates to relative click Coordinates
 */
Coordinate Map::WorldToClick( Coordinate world ) {
	Coordinate click = world;

	click -= center;
	click *= scale; // Descale the click
	click += Coordinate( w / 2, h / 2 ); // Offset by the center of this widget
	click += Coordinate( GetX(), GetY() ); // Offset by this Widget's origin

	return click;
}

/** \brief Convert world coordinates to screen Coordinates
 */
Coordinate Map::WorldToScreen( Coordinate world ) {
	Coordinate screen = world;

	screen -= center;
	screen *= scale; // Descale the screen
	screen += Coordinate( w / 2, h / 2 ); // Offset by the center of this widget
	screen += Coordinate( GetAbsX(), GetAbsY() ); // Offset by the absolute screen coordinate

	return screen;
}

bool Map::MouseLUp( int xi, int yi ) {
	Widget::MouseLUp( xi, yi );

	return false;
}

bool Map::MouseLDown( int xi, int yi ) {
	Widget::MouseLDown( xi, yi );

	return false;
}

/** \brief Pan the Map
 */
bool Map::MouseDrag( int xi, int yi ) {
	if( pannable ) {
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
	if( zoomable ) {
		scale *= MAP_ZOOM_RATIO;
	}

	Widget::MouseWUp( xi, yi );

	return true;
}

/** \brief Zoom the map out
 */
bool Map::MouseWDown( int xi, int yi )
{
	if( zoomable ) {
		scale /= MAP_ZOOM_RATIO;
	}

	Widget::MouseWDown( xi, yi );

	return true;
}

/** @} */

