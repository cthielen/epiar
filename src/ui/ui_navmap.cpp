/**\file			ui_navmap.cpp
 * \author			Matt Zweig
 * \date			Created:  Saturday, May 28, 2011
 * \date			Modified: Sunday, April 10, 2016
 * \brief			Map Widget
 * \details
 */

#include "engine/navigation.h"
#include "includes.h"
#include "input/input.h"
#include "ui/ui_navmap.h"
#include "sprites/planets.h"
#include "sprites/effects.h"
#include "utilities/timer.h"

#define SECTOR_CLICK_SELECTION_RADIUS 35.0

/** \addtogroup UI
 * @{
 */

/**\class NavMap
 * \brief Widget for displaying Sprites
 *
 */

#define MAP_ZOOM_RATIO 1.1f ///< The rate at which the Map Zooms in and out.

Font* NavMap::NavMapFont = NULL;

/** \brief Map Constructor
 *
 */
NavMap::NavMap( int x, int y, int w, int h, Coordinate center, Scenario* scenario ) {
	this->x = x;
	this->y = y;

	this->w = w;
	this->h = h;
	this->center = center;
	this->scenario = scenario;

	this->selectedSector = NULL;

	alpha = 1;

	float size = (h < w) ? h : w; // Max of Height and Width

	// Set Map boundaries to cover all sectors
	float north, south, east, west, edge;
	Sectors* sectors = this->scenario->GetSectors();
	if(sectors != NULL) {
		sectors->GetBoundaries(&north, &south, &east, &west);

		// edge is the maximum distance from zero of north, south, east, and west
		edge = (north > -south) ? north : -south;
		edge = (edge > -west) ? edge : -west;
		edge = (edge > east) ? edge : east;
		scale = size / (1.5 * edge);
	}

	if( NavMapFont == NULL ) {
		NavMapFont = new Font( SKIN("Skin/HUD/Map/Font"), convertTo<int>( SKIN("Skin/HUD/Map/Size") ));
		NavMapFont->SetColor( Color( SKIN("Skin/HUD/Map/Color") ) );
	}

	zoomable = true;
	pannable = true;

	// Set up the 'Clear Route' button
	clearRouteButton = new Button( 10, this->GetH() - 35, 85, 25, "Clear Route", NavMap::ClearRouteButtonCallback );
	Container::AddChild( clearRouteButton );
}

/** \brief Map Destructor
 *
 */
NavMap::~NavMap() {
	scenario = NULL;
	delete NavMapFont;
	NavMapFont = NULL;
	clearRouteButton = NULL; // Let the Container Destructor delete the button
}

/** \brief Draw Map
 *
 */
void NavMap::Draw( int relx, int rely ) {
	list<Sector*>::iterator iter;
	list<Sector*>* sectors = NULL;
	Sector* currentSector = this->scenario->GetCurrentSector();

	// These variables are used for almost every sprite symbol
	Coordinate pos;
	Color col, field;

	Sectors* sectorsHandle = this->scenario->GetSectors();
	if(sectorsHandle == NULL) return;

	sectors = sectorsHandle->GetAllSectors();

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
		startPos = WorldToMap(Coordinate(sector->GetX(), sector->GetY()));

		for( neighborItr = neighbors.begin(); neighborItr != neighbors.end(); ++neighborItr ) {
			Sector *neighbor = (Sector *)sectorsHandle->Get( *neighborItr );
			assert(neighbor != NULL);

			Coordinate endPos;

			endPos = WorldToMap(Coordinate(neighbor->GetX(), neighbor->GetY()));

			// If this connecting line is a navigation route, draw in a different color
			if(Navigation::IsEnroute(sector, neighbor)) {
				// Draw the connecting line
				Video::DrawLine( startPos.GetX(), startPos.GetY(), endPos.GetX(), endPos.GetY(), GREEN);
				Video::DrawLine( startPos.GetX(), startPos.GetY() + 1, endPos.GetX(), endPos.GetY() + 1, GREEN);
			} else {
				// Draw the connecting line
				Video::DrawLine( startPos.GetX(), startPos.GetY(), endPos.GetX(), endPos.GetY(), DARKGREY);
			}
		}
	}

	// Draw the sectors
	for( iter = sectors->begin(); iter != sectors->end(); ++iter ) {
		Sector *sector = (Sector *)(*iter);

		// If sector has no planets, draw as WHITE instead of BLUE
		if(sector->HasPlanets()) {
			col = BLUE;
		} else {
			col = WHITE;
		}
		pos = WorldToMap(Coordinate(sector->GetX(), sector->GetY()));

		field = sector->GetAlliance()->GetColor();

		Video::DrawFilledCircle( pos, (55 * scale) / 3, BLACK, alpha );
		if(sector == currentSector) {
			Video::DrawFilledCircle( pos, (55 * scale) / 3, LIGHTBLUE, alpha );
		}
		Video::DrawCircle( pos, (55 * scale) / 3, 1, col, alpha );

		if(sector == selectedSector) {
			Video::DrawTarget( pos.GetX(), pos.GetY(), (50 * scale), (50 * scale), 3, 0.8, 0.8, 0.8 );
		}
	}

	// Do a second pass to draw planet Names on top
	for( iter = sectors->begin(); iter != sectors->end(); ++iter ) {
		Sector *sector = (Sector *)(*iter);
		pos = WorldToMap(Coordinate(sector->GetX(), sector->GetY()));
		NavMapFont->Render( pos.GetX() + 5, pos.GetY(), sector->GetName().c_str() );
	}

	Video::UnsetCropRect();

	delete sectors;
	sectors = NULL;

	Container::Draw(relx, rely);
}

/** \brief Convert click coordinates to World Coordinates
 */
Coordinate NavMap::ClickToWorld( Coordinate click ) {
	Coordinate world = click;

	world -= Coordinate( GetX(), GetY() ); // Offset by this Widget's origin
	world -= Coordinate( w / 2, h / 2 ); // Offset by the center of this widget
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
	click += Coordinate( w / 2, h / 2 ); // Offset by the center of this widget
	click += Coordinate( GetX(), GetY() ); // Offset by this Widget's origin

	return click;
}

/** \brief Convert world coordinates to map Coordinates, taking current
 *         sector as the center.
 */
Coordinate NavMap::WorldToMap( Coordinate world ) {
	Coordinate map = world;

	map -= center;
	map *= scale; // Descale the map
	map += Coordinate( w / 2, h / 2 ); // Offset by the center of this widget
	map += Coordinate( GetAbsX(), GetAbsY() ); // Offset by the absolute screen coordinate

	return map;
}

bool NavMap::MouseLUp( int x, int y ) {
	Widget::MouseLUp( x, y );

	Coordinate click(x, y);

	// Determine if they clicked on a sector
	list<Sector*>* sectors = NULL;
	list<Sector*>::iterator iter;
	Sectors* sectorsHandle = this->scenario->GetSectors();
	assert(sectorsHandle != NULL);

	sectors = sectorsHandle->GetAllSectors();

	for( iter = sectors->begin(); iter != sectors->end(); ++iter ) {
		Sector *sector = (Sector *)(*iter);
		
		if(SectorNearClick(sector, click)) {
			selectedSector = sector;

			// Is shift held down? If so, they are trying to plot a course ...
			if(Input::keyIsHeld(SDLK_LSHIFT)) {
				Navigation::AddSector(selectedSector->GetName());
			}

			return true;
		}
	}

	return Container::MouseLUp( x, y );
}

bool NavMap::MouseLDown( int x, int y ) {
	Widget::MouseLDown( x, y );

	return Container::MouseLDown( x, y );
}

/** \brief Pan the Map
 */
bool NavMap::MouseDrag( int xi, int yi ) {
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
bool NavMap::MouseWUp( int xi, int yi ) {
	if( zoomable ) {
		scale *= MAP_ZOOM_RATIO;
	}

	Widget::MouseWUp( xi, yi );

	return true;
}

/** \brief Zoom the map out
 */
bool NavMap::MouseWDown( int xi, int yi )
{
	if( zoomable ) {
		scale /= MAP_ZOOM_RATIO;
	}

	Widget::MouseWDown( xi, yi );

	return true;
}

bool NavMap::SectorNearClick(Sector *sector, Coordinate click) {
	assert(sector != NULL);

	Coordinate sectorScreen = WorldToClick(Coordinate(sector->GetX(), sector->GetY()));

	float distance = (sectorScreen - click).GetMagnitude();

	if(distance < (SECTOR_CLICK_SELECTION_RADIUS * scale)) return true;	

	return false;
}

void NavMap::ClearRouteButtonCallback(void) {
	Navigation::ClearRoute();
}

/** @} */

