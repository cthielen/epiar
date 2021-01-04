/**\file			hud.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, July 23, 2006
 * \date			Modified: Saturday, June 4, 2016
 * \brief			Handles the Heads-Up-Display
 * \details
 */

#include "includes.h"
#include "common.h"
#include "engine/hud.h"
#include "engine/scenario.h"
#include "graphics/video.h"
#include "graphics/font.h"
#include "sprites/player.h"
#include "sprites/spritemanager.h"
#include "ui/ui_navmap.h"
#include "utilities/log.h"
#include "utilities/timer.h"
#include "engine/camera.h"

/* Length of the hull integrity bar (pixels) + 6px (the left+right side imgs) */
#define HULL_INTEGRITY_BAR  65

/* Location on screen of hull integrity bar (x,y) coord is top-left */
#define HULL_INTEGRITY_X     5
#define HULL_INTEGRITY_Y     5

#define HUD_MESSAGE_BOTTOM_SPACING 10

/* Center of radar in px coords. Derrived from hud_radarnav.png */
#define RADAR_MIDDLE_X      61
#define RADAR_MIDDLE_Y      61

/* Width/height of radar. Derrived from hud_radarnav.png */
#define RADAR_WIDTH        122
#define RADAR_HEIGHT       122

/* Number of additional, fading reticle boxes to draw while zooming in on a newly-selected target */
#define RETICLE_BLUR       9

/* Extra line height for HUD alerts/messages (in addition to the font-defined line height) */
#define ALERT_LINE_HEIGHT_EXTRA 4

list<AlertMessage> Hud::AlertMessages;
StatusBar* Hud::Bars[MAX_STATUS_BARS] = {};
int Hud::targetID = -1;
int Hud::timeTargeted = 0;
Font *Hud::AlertFont = NULL;
Color Hud::AlertColor = WHITE;
Sound *Hud::AlertBeep = NULL;

int Radar::visibility = 4096.0f;
//bool Radar::largeMode = false;

Font *StatusBar::font = NULL;

/**\class AlertMessage
 * \brief Alert/Info messages
 */

/**\var AlertMessage::message
 * \brief The message
 */

/**\var AlertMessage::start
 * \brief The start time (For expiration)
 */

/**\brief Adds an alert to the queue.
 * \param message std:string containing the message
 * \param start Starting time (for expiration)
 */
AlertMessage::AlertMessage( string message, Uint32 start )
{
	this->message = message;
	this->start = start;
}

/**\fn AlertMessage::operator ==(const AlertMessage& other)
 * \brief Compares the start time of two AlertMessage
 */

/**\brief Checks to see if message expired.
 * \param msg Pointer to an AlertMessage object
 * \return true if expired
 */
bool MessageExpired(const AlertMessage& msg){
	return (Timer::GetTicks() - msg.start > OPTION(Uint32,"options/timing/alert-drop"));
}

/**\class StatusBar
 * \brief Status bar
 */

StatusBar::StatusBar(string _title, int _width, StatusBarPosition _pos, string _updater)
	:width(_width)
	,pos(_pos)
	,ratio(0)
{
	strncpy(title, _title.c_str(), sizeof(title) - 1);
	title[sizeof(title)-1] = '\0';

	memset( name, '\0', sizeof(name) );
	lua_updater = _updater;
	LogMsg (DEBUG, "Creating a new StatusBar '%s' : Name(%s) / Ratio( %f)\n", title, name, ratio);
	assert(pos >= 0);
	assert(pos <= 3);

	if( font == NULL ) {
		font = Font::Get( SKIN("Skin/HUD/StatusBar/Font"), convertTo<int>(SKIN("Skin/HUD/StatusBar/Size")) );
		font->SetColor( SKIN("Skin/HUD/StatusBar/Color") );
	}
}

/**\brief Assignment operator for class StatusBar.
 * \return Pointer to StatusBar
 */
StatusBar& StatusBar::operator=( StatusBar& object ){
	strcpy( title, object.title );
	strcpy( name, object.name );

	ratio = object.ratio;
	lua_updater = object.lua_updater;

	return * this;
}

/**\brief Draws the StatusBar
 * \param x x-coordinate
 * \param y y-coordinate
 *
 * \bug This can segfault with an empty title.
 */
void StatusBar::Draw(int x, int y) {
	int widthRemaining = this->width;

	Image *BackgroundLeft = Image::Get( "data/skin/hud_bar_left.png" );
	Image *BackgroundMiddle = Image::Get( "data/skin/hud_bar_middle.png" );
	Image *BackgroundRight= Image::Get( "data/skin/hud_bar_right.png" );

	if(pos == SBP_UPPER_RIGHT || pos == SBP_LOWER_RIGHT) {
		x = Video::GetWidth() - BackgroundLeft->GetWidth() - width - BackgroundRight->GetWidth();
	}

	// Draw the Border
	BackgroundLeft->Draw(x,y);
	x += BackgroundLeft->GetWidth();
	BackgroundMiddle->DrawTiled(x, y, width, BackgroundMiddle->GetHeight());
	BackgroundRight->Draw(x + width, y);


	// Draw the Title
	int wTitle = font->RenderTight( x, y + BackgroundMiddle->GetHalfHeight(), title, Font::LEFT, Font::MIDDLE );
	widthRemaining -= wTitle;
	x += wTitle + 5;

	// Draw Name
	int wName = font->RenderTight( x, y + BackgroundMiddle->GetHalfHeight(), name, Font::LEFT, Font::MIDDLE );
	widthRemaining -= wName;
	x += wName;

	// Draw the Bar
	if ( (int)(ratio*widthRemaining) > 0 ) {
		Image *BarLeft = Image::Get( "data/skin/hud_hullstr_leftbar.png" );
		Image *BarMiddle = Image::Get( "data/skin/hud_hullstr_bar.png" );
		Image *BarRight = Image::Get( "data/skin/hud_hullstr_rightbar.png" );

		int bar_y = y + BackgroundLeft->GetHalfHeight() - BarLeft->GetHalfHeight();
		BarLeft->Draw( x, bar_y );
		x += BarLeft->GetWidth();
		int bar_w = widthRemaining - BarLeft->GetWidth() - BarRight->GetWidth();
		BarMiddle->DrawTiled( x, bar_y,static_cast<int>(bar_w*ratio), BarMiddle->GetHeight() );
		BarRight->Draw( x + static_cast<int>(bar_w*ratio), bar_y );
	}
}

void StatusBar::Update( lua_State* L ) {
	int returnvals, retpos = -1;

	// Run the StatusBar Updater
	returnvals = Lua::Run( lua_updater, true );

	// Get the new StatusBar Status
	if (returnvals == 0) {
		SetName( "" );
		SetRatio( 0.0f );
	} else if (lua_isnumber(L, retpos) && ( lua_tonumber(L,retpos)>=0.0 && lua_tonumber(L,retpos)<=1.0) )  {
		SetName( "" );
		SetRatio( TO_FLOAT(lua_tonumber(L, retpos)) );
	} else if (lua_isstring(L, retpos)) {
		SetRatio( 0.0f );
		SetName( string(lua_tostring(L, retpos)) );
	} else {
		LogMsg(ERR,"Error running '%s': %s", lua_updater.c_str(), lua_tostring(L, retpos));
	}
	lua_pop(L,returnvals);
}

/**\fn StatusBar::StatusBar(string _title, int _width, StatusBarPosition _pos, string _name, float _ratio) : title(_title), width(_width), pos(_pos), name(_name), ratio(_ratio)
 * \brief Empty constructor.
 */

/**\fn StatusBar::SetName(string n)
 * \brief Sets the name of the StatusBar
 */
void StatusBar::SetName( string n )
{
	strncpy(name, n.c_str(), sizeof(name) - 1);
	name[sizeof(name) - 1] = '\0';
}

/**\fn StatusBar::GetName
 * \brief Returns the name of the StatusBar
 */

/**\fn StatusBar::SetRatio(float _ratio)
 * \brief Sets ratio (Ratio of the middle to the whole)
 */

/**\fn StatusBar::GetRatio
 * \brief Returns the ratio
 * \sa StatusBar::SetRatio(float _ratio)
 */

/**\fn StatusBar::GetPosition
 * \brief Returns the StatusBarPosition
 */

//Protect members
/**\var StatusBar::title
 * \brief Title of the StatusBar
 */

/**\var StatusBar::width
 * \brief Width of the StatusBar
 */

/**\var StatusBar::pos
 * \brief StatusBarPosition of the StatusBar
 */

/**\var StatusBar::name
 * \brief Name of the StatusBar
 */

/**\var StatusBar::ratio
 * \brief Ratio of the StatusBar
 * \sa StatusBar::SetRatio(float _ratio)
 */

/**\class Hud
 * \brief Heads-Up-Display. */

void Hud::Init( void ) {
	AlertFont = new Font( SKIN("Skin/HUD/Alert/Font"), convertTo<int>( SKIN("Skin/HUD/Alert/Size") ) );
	AlertColor = Color( SKIN("Skin/HUD/Alert/Color") );
	AlertBeep = Sound::Get( "data/audio/interface/hud_beep.ogg" );
}

void Hud::Close( void ) {
	if(AlertFont != NULL) delete AlertFont;

	AlertFont = NULL;
}

/**\brief Updates the HUD
 */
void Hud::Update( lua_State *L ) {
	int j;
	list<AlertMessage>::iterator i;

	i = AlertMessages.begin();
	while( i != AlertMessages.end() ) {
		bool messageExpired = MessageExpired(*i);

		if(messageExpired) {
			AlertMessages.erase(i++);
		} else {
			++i;
		}
	}

	for( j = 0; j < MAX_STATUS_BARS; j++) {
		if( Bars[j] != NULL ) {
			Bars[j]->Update( L );
		}
	}
}

/**\brief Draws the Hud
 */
void Hud::Draw( int flags, float fps, Camera* camera, SpriteManager* sprites ) {
	if(flags & HUD_Target)     Hud::DrawTarget( sprites );
	if(flags & HUD_Shield)     Hud::DrawShieldIntegrity();
	if(flags & HUD_Radar)      Hud::DrawRadarNav( camera, sprites );
	if(flags & HUD_Messages)   Hud::DrawMessages();
	if(flags & HUD_FPS)        Hud::DrawFPS(fps, sprites);
	if(flags & HUD_StatusBars) Hud::DrawStatusBars();
}

/**\brief Handles Hud related User Input
 * \param events User entered Keyboard and mouse clicks
 */
void Hud::HandleInput( list<InputEvent> & events, Camera* camera, SpriteManager* sprites ) {
	list<InputEvent>::iterator i;

	for(i = events.begin(); i != events.end() ; ++i ) {
		// Mouse Clicks
		if( i->type == MOUSE && i->mstate==MOUSELDOWN) {
			if( (i->mx > Video::GetWidth() - 129)
			 && (i->my < Image::Get( "data/skin/hud_radarnav.png" )->GetHeight() + 5) )
			{
				//Radar::StartLargeMode(camera, sprites);
			}
			else
			{
				Coordinate screenPos(i->mx, i->my), worldPos;
				camera->TranslateScreenToWorld( screenPos, worldPos );
				// Target any clicked Sprite
				list<Sprite*> *impacts = sprites->GetSpritesNear( worldPos, 5 );
				if( impacts->size() > 0) {
					Target( (*(impacts->begin()))->GetID());
				}
				delete impacts;
			}
		}
	}
}


/**\brief Draw HUD messages (eg Welcome to Epiar).
 */
void Hud::DrawMessages() {
	int j;
	int now = Timer::GetTicks();
	list<AlertMessage>::reverse_iterator i;
	Uint32 age;
	Uint32 alertFade = OPTION(Uint32,"options/timing/alert-fade");
	Uint32 alertDrop = OPTION(Uint32,"options/timing/alert-drop");

	for( i = AlertMessages.rbegin(), j=1; (i != AlertMessages.rend()) && (j <= MAX_ALERTS); ++i,++j ){
		age = now - (*i).start;
		if(age > alertFade) {
			AlertFont->SetColor( AlertColor, 1.f - float((age - alertFade)) / float(alertDrop - alertFade) );
		} else {
			AlertFont->SetColor( AlertColor, 1.f);
		}

		AlertFont->Render( 15, Video::GetHeight() - (j * (AlertFont->LineHeight() + ALERT_LINE_HEIGHT_EXTRA)) - HUD_MESSAGE_BOTTOM_SPACING, (*i).message);
	}
}

/**\brief Draw the current framerate (calculated in scenario.cpp).
 */
void Hud::DrawFPS( float fps, SpriteManager* sprites ) {
	char frameRate[18] = {0};

	BitType->SetColor( WHITE );
	snprintf(frameRate, sizeof(frameRate) - 1, "%.2f fps", fps );
	BitType->Render( Video::GetWidth() - 100, Video::GetHeight() - 15, frameRate );

	snprintf(frameRate, sizeof(frameRate) - 1, "%d Sprites", sprites->GetNumSprites());
	BitType->Render( Video::GetWidth() - 100, Video::GetHeight() - 30, frameRate );
}

/**\brief Draws the status bar.
 */
void Hud::DrawStatusBars() {
	// Initialize the starting Coordinates
	int barHeight = Image::Get( "data/skin/hud_bar_left.png" )->GetHeight() + 5;

	Coordinate startCoords[4];
	startCoords[SBP_UPPER_LEFT]  = Coordinate(5, Image::Get( "data/skin/hud_shieldintegrity.png" )->GetHeight() + 9);
	startCoords[SBP_UPPER_RIGHT] = Coordinate(5, Radar::GetHeight() + 9);
	startCoords[SBP_LOWER_LEFT]  = Coordinate(5, Video::GetHeight()-barHeight);
	startCoords[SBP_LOWER_RIGHT] = Coordinate(5, Video::GetHeight()-barHeight);
	Coordinate offsetCoords[4]= {
		Coordinate(0,barHeight), Coordinate(0,barHeight),
		Coordinate(0,-barHeight), Coordinate(0,-barHeight)};

	int i;
	StatusBar* bar;
	for( i= 0; i < MAX_STATUS_BARS; ++i ){
		bar = Bars[i];
		if( bar == NULL ) continue;
		int pos = bar->GetPosition();
		assert(pos>=0);
		assert(pos<=4);
		int x = static_cast<int>(startCoords[pos].GetX());
		int y = static_cast<int>(startCoords[pos].GetY());
		bar->Draw(x,y);
		startCoords[pos] += offsetCoords[pos];
	}
}

/**\brief Draw the shield bar.
 */
void Hud::DrawShieldIntegrity() {
	Image::Get( "data/skin/hud_shieldintegrity.png" )->Draw( 35, 5 );
}

/**\brief Draw the radar.
 */
void Hud::DrawRadarNav( Camera* camera, SpriteManager* sprites ) {
	Image::Get( "data/skin/hud_radarnav.png" )->Draw( Video::GetWidth() - 129, 5 );

	Video::SetCropRect( Video::GetWidth() - 125, 9, RADAR_WIDTH - 8, RADAR_HEIGHT - 8 );

	Radar::Draw( camera, sprites );

	Video::UnsetCropRect();
}

/**\brief Draws the target.
 */
void Hud::DrawTarget( SpriteManager* sprites ) {
	Sprite* target = sprites->GetSpriteByID( targetID );

	if(target != NULL) {
		int edge = (target->GetImage())->GetWidth() / 6;
		Coordinate targetScreenPosition = target->GetScreenPosition();
		if(edge > 25) edge = 25;
		int x = targetScreenPosition.GetX();
		int y = targetScreenPosition.GetY();
		int r = target->GetRadarSize();
		Color c = target->GetRadarColor();

		if( (Timer::GetTicks() - timeTargeted) < OPTION(Uint32, "options/timing/target-zoom")) {
			r += Video::GetHalfHeight() - Video::GetHalfHeight()*(Timer::GetTicks()-timeTargeted)/OPTION(Uint32,"options/timing/target-zoom");
			for( int i = 0; i < RETICLE_BLUR; i++ ) {
				c = c * .9f;
				edge += 3;
				r *= 1.1f;
				Video::DrawTarget(x,y,r,r,edge,c.r,c.g,c.b);
			}
		} else {
			Video::DrawTarget(x,y,r,r,edge,c.r,c.g,c.b);
		}
	}
}

/**\brief Adds a new AlertMessage.
 * \param message C string of message.
 * \param ... Arguments that are formated into the message.
 */
void Hud::Alert( bool audible, const char *message, ... ) {
	va_list args;
	char msgBuffer[ 2048 ] = {0};

	// Format the Message
	va_start( args, message );
	vsnprintf( msgBuffer, 2047, message, args );
	va_end( args );

	// Store this Message as a new Alert.
	// Use the current time so that it can fade away gracefully.
	AlertMessages.push_back( AlertMessage( msgBuffer, Timer::GetTicks() ) );

	if(audible && AlertBeep) { AlertBeep->Play(); }
}


/**\brief Changes the Hud target
 * \param id Unique Sprite id number
 */
void Hud::Target(int id) {
	targetID = id;
	timeTargeted = Timer::GetTicks();
}

/**\brief Adds a new StatusBar.
 * \param bar Pointer to a new StatusBar
 */
void Hud::AddStatus( StatusBar* bar) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( Bars[i]== NULL )
		{
			Bars[i] = bar;
			break;
		}
	}
}

/**\brief Deletes a StatusBar.
 */
bool Hud::DeleteStatus( StatusBar* bar ) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( Bars[i]==bar )
		{
			Bars[i] = NULL;
			delete bar;
			return true;
		}
	}
	return false;
}

/**\brief Deletes a StatusBar by Name.
 */
bool Hud::DeleteStatus( string deleteTitle ) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( (Bars[i]!=NULL) && (Bars[i]->GetTitle()==deleteTitle) )
		{
			delete Bars[i];
			Bars[i] = NULL;
			return true;
		}
	}
	LogMsg(ERR, "Could not find the StatusBar named '%s'", deleteTitle.c_str() );
	return false;
}

/**\brief Deletes a StatusBar if it exists
 */
void Hud::DeleteStatusIfExists( string deleteTitle ) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( (Bars[i]!=NULL) && (Bars[i]->GetTitle()==deleteTitle) )
		{
			LogMsg(INFO, "Deleting StatusBar '%s'", deleteTitle.c_str() );
			delete Bars[i];
			Bars[i] = NULL;
			return;
		}
	}
}

/**\brief Is there a matching status bar?
 */
bool Hud::HasStatusMatching( string matchPattern ) {
	int i;
	for(i = 0; i< MAX_STATUS_BARS; i++)
	{
		if( (Bars[i]!=NULL) && (  strstr(Bars[i]->GetTitle().c_str(), matchPattern.c_str() ) != NULL ) )
		{
			return true;
		}
	}
	return false;
}

/**\brief Register Lua functions for HUD related updates.
 */
void Hud::RegisterHud(lua_State *L) {
	Lua::RegisterGlobal("UPPER_LEFT", SBP_UPPER_LEFT);
	Lua::RegisterGlobal("UPPER_RIGHT", SBP_UPPER_RIGHT);
	Lua::RegisterGlobal("LOWER_LEFT", SBP_LOWER_LEFT);
	Lua::RegisterGlobal("LOWER_RIGHT", SBP_LOWER_RIGHT);

	static const luaL_Reg hudFunctions[] = {
		{"setVisibity", &Hud::setVisibity},
		{"newStatus", &Hud::newStatus},
		{"closeStatus", &Hud::closeStatus},
		{"closeStatusIfExists", &Hud::closeStatusIfExists},
		{"HudHasStatusMatching", &Hud::HudHasStatusMatching},
		{"newAlert", &Hud::newAlert},
		{"getTarget", &Hud::getTarget},
		{"setTarget", &Hud::setTarget},
		{NULL, NULL}
	};

	luaL_openlib(L, EPIAR_HUD, hudFunctions, 0);

	lua_pop(L,1);
}

/**\brief Set's the visibility of the target (Lua callable)
 */
int Hud::setVisibity(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (visibility)", n);
	int visibility = (int)(luaL_checkint(L,1));
	Radar::SetVisibility(visibility);
	return 0;
}

/**\brief Creates a new Alert (Lua callable).
 */
int Hud::newAlert(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments

	if(n != 2) {
		return luaL_error(L, "Got %d argument(s) expected 2 (audible, message)", n);
	}

	bool audible = (bool)luaL_checkint(L, 1);
	const char* msg = luaL_checkstring(L, 2);

	Alert(audible, msg);

	return 0;
}

/**\brief Creates a new Status (Lua callable).
 */
int Hud::newStatus(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if( n < 3 ) {
		return luaL_error(L, "Got %d argument(s) expected 4 (title, width, postition )", n);
	}

	// Create the Status Bar
	string title = (string)luaL_checkstring(L, 1);
	int width = (int)(luaL_checkint(L, 2));
	StatusBarPosition pos = (StatusBarPosition)(luaL_checkint(L, 3));

	if(pos < 0 || pos > 3) {
		return luaL_error(L, "Invalid Position %d. Valid Options are: UPPER_LEFT=0, UPPER_RIGHT=1, LOWER_LEFT=2, LOWER_RIGHT=3", pos);
	}

	string updater = (string)luaL_checkstring(L, 4);
	StatusBar *bar = new StatusBar(title, width, pos, updater);

	// Add the Bar to the Hud
	AddStatus(bar);

	return 0;
}

/**\brief Closes the status (Lua callable).
 */
int Hud::closeStatus(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1) {
		return luaL_error(L, "Got %d argument(s) expected 1 (Title)", n);
	}

	string deleteTitle = luaL_checkstring(L,1);
	bool success = DeleteStatus( deleteTitle );

	if(success == false) {
		return luaL_error(L, "closeStatus couldn't find the StatusBar titled '%s'.", deleteTitle.c_str() );
	}

	return 0;
}

/**\brief Closes the status if it exists (Lua callable).
 */
int Hud::closeStatusIfExists(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (Title)", n);
	string deleteTitle = luaL_checkstring(L,1);
	DeleteStatusIfExists( deleteTitle );
	return 0;
}

/**\brief Lua-callable for HasStatusMatching
 */
int Hud::HudHasStatusMatching(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (Pattern)", n);
	string matchPattern = luaL_checkstring(L,1);
	bool yes = HasStatusMatching( matchPattern );
	lua_pushinteger(L, (yes?1:0) );
	return 0;
}


/**\brief Returns the target (Lua callable).
 */
int Hud::getTarget(lua_State *L) {
	lua_pushinteger(L, targetID );
	return 1;
}

/**\brief Sets the target (Lua callable).
 */
int Hud::setTarget(lua_State *L) {
	int n = lua_gettop(L);  // Number of arguments
	if (n != 1)
		return luaL_error(L, "Got %d arguments expected 1 (ID)", n);
	Target( luaL_checkint(L,1) );
	return 0;
}

/**\class Radar
 * \brief Hud Element that displays nearby objects. */

/**\brief Empty constructor.
 */
Radar::Radar( void ) {
}

/**\brief Sets the visibility.
 * \param visibility true or false
 */
void Radar::SetVisibility( int visibility ) {
	Radar::visibility = visibility;
	//if( largeMode ) {
	//	Map* map = (Map*)UI::Search("/Map/");
	//	assert( map );
	//	if( map ) {
	//		map->SetScale( 300.0 / (2*visibility) );
	//	}
	//}
}

/*void Radar::StartLargeMode( Camera* camera, SpriteManager* sprites ) {
	largeMode = true;
	Map* map = new Map( Video::GetWidth() - 300, 0, 300, 300, camera->GetFocusCoordinate(), sprites );
	map->RegisterAction( Action_MouseLeave, new VoidAction( Radar::StopLargeMode ) );
	map->SetFilter(
		DRAW_ORDER_PLAYER   |
		DRAW_ORDER_PLANET   |
		DRAW_ORDER_SHIP );
	map->SetScale( 300.0 / (2*visibility) );
	UI::Add(map);
}

void Radar::StopLargeMode() {
	UI::Close( UI::Search( "/Map/" ) );
	largeMode = false;
}*/

/**\brief Draws the radar.
 */
void Radar::Draw( Camera* camera, SpriteManager* sprites ) {
	short int radar_mid_x = RADAR_MIDDLE_X + Video::GetWidth() - 129;
	short int radar_mid_y = RADAR_MIDDLE_Y + 5;
	int radarSize;
	Coordinate focus = camera->GetFocusCoordinate();

	/*if(largeMode) {
		if( visibility <= QUADRANTSIZE )
		{
			Map* map = (Map*)UI::Search( "/Map/" );
			assert(map); // large mode should only be on when there is a map.
			if(map) {
				map->SetCenter( focus );
				map->SetScale( 300.0 / (2*visibility) );
			}
		}
		return;
	}*/

	list<Sprite*> *spriteList = sprites->GetSpritesNear(camera->GetFocusCoordinate(), (float)visibility);
	for( list<Sprite*>::const_iterator iter = spriteList->begin(); iter != spriteList->end(); iter++)
	{
		Coordinate blip;
		Sprite *sprite = *iter;

		// Calculate the blip coordinate for this sprite
		Coordinate wpos = sprite->GetWorldPosition();
		WorldToBlip( focus, wpos, blip );

		// Use the OpenGL Crop Rectangle to ensure that the blip is on the radar

		/* Convert to screen coords */
		blip.SetX( blip.GetX() + radar_mid_x );
		blip.SetY( blip.GetY() + radar_mid_y );

		radarSize = int((sprite->GetRadarSize() / float(visibility)) * (RADAR_HEIGHT / 4.0));

		if( radarSize >= 1 ) {
			if(sprite->GetID() == Hud::GetTarget() && Timer::GetTicks() % 1000 < 100)
				Video::DrawCircle( blip, radarSize, 2, WHITE );
			else
				Video::DrawCircle( blip, radarSize, 1, sprite->GetRadarColor() );
		} else {
			if(sprite->GetID() == Hud::GetTarget() && Timer::GetTicks() % 1000 < 100)
				Video::DrawCircle( blip, 1, 2, WHITE );
			else
				Video::DrawPoint( blip, sprite->GetRadarColor() );
		}
	}

	delete spriteList;
}

/**\brief Gets the radar position based on world coordinate
 * \param w Pointer to world coordinate
 * \retval b Pointer to radar coordinate
 */
void Radar::WorldToBlip( Coordinate focus, Coordinate &w, Coordinate &b ) {
	b.SetX( ( ( w.GetX() - focus.GetX() ) / float(visibility) ) * ( RADAR_WIDTH / 2.0 ) );
	b.SetY( ( ( w.GetY() - focus.GetY() ) / float(visibility) ) * ( RADAR_HEIGHT / 2.0 ) );
}

int Radar::GetHeight() {
	//if( largeMode ) {
	//	return 300;
	//} else {
		return Image::Get( "data/skin/hud_radarnav.png" )->GetHeight();
	//}
}
