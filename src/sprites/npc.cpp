/**\file			npc.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Thursday, December 3, 2015
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "menu.h"
#include "sprites/npc.h"
#include "sprites/player.h"
#include "sprites/spritemanager.h"
#include "utilities/lua.h"
#include "engine/scenario_lua.h"

/** \addtogroup Sprites
 * @{
 */

/**\class AI
 * \brief AI controls the non-player Ships.
 *
 * */

/** \brief AI Constructor
 */

NPC::NPC(string _name, string machine) :
	name(_name),
	allegiance(NULL),
	stateMachine(machine),
	state("default")
{
	this->isPlayerFlag = false;
	target = 0;
	merciful = 0;
}

/** \brief Run the Lua Statemachine to act and possibly change state.
 */

void NPC::Decide( lua_State *L ) {
	string newstate;
	// Decide
	const int initialStackTop = lua_gettop(L);

	// Get the current state machine
	lua_getglobal(L, stateMachine.c_str() );
	int machineIndex = lua_gettop(L);
	if( ! lua_istable(L, machineIndex) )
	{
		LogMsg(ERR, "There is no State Machine named '%s'!", stateMachine.c_str() );
		return; // This ship will just sit idle...
	}

	// Get the current state
	lua_pushstring(L, state.c_str() );
	lua_gettable(L, machineIndex);
	if( ! lua_isfunction(L,lua_gettop(L)) )
	{
		LogMsg(WARN, "The State Machine '%s' has no state '%s'.", stateMachine.c_str(), state.c_str() );
		lua_getglobal(L, stateMachine.c_str() );
		lua_pushstring(L, "default" );
		lua_gettable(L,machineIndex);
		if( !lua_isfunction(L,lua_gettop(L)) )
		{
			LogMsg(ERR, "The State Machine '%s' has no default state.", stateMachine.c_str() );
			lua_settop(L, initialStackTop);
			return; // This ship will just sit idle...
		}
	}

	// Push Current AI Variables
	lua_pushinteger( L, this->GetID() );
	lua_pushnumber( L, this->GetWorldPosition().GetX() );
	lua_pushnumber( L, this->GetWorldPosition().GetY() );
	lua_pushnumber( L, this->GetAngle() );
	lua_pushnumber( L, this->GetMomentum().GetMagnitude() ); // Speed
	lua_pushnumber( L, this->GetMomentum().GetAngle() ); // Vector

	// Run the current AI state
	//printf("Call:"); Lua::stackDump(L); // DEBUG
	if( lua_pcall(L, 6, 1, 0) != 0)
	{
		LogMsg(ERR,"Failed to run %s(%s): %s\n", stateMachine.c_str(), state.c_str(), lua_tostring(L, -1));
		lua_settop(L, initialStackTop);
		return;
	}
	//printf("Return:"); Lua::stackDump(L); // DEBUG

	if( lua_isstring( L, lua_gettop(L) ) )
	{
		newstate = (string)luaL_checkstring(L, lua_gettop(L));

		// Verify that this new state exists
		lua_pushstring(L, newstate.c_str() );
		lua_gettable(L,machineIndex);
		if( lua_isfunction(L, lua_gettop(L) ))
		{
			state = newstate;
		} else {
			LogMsg(ERR, "The State Machine '%s' has no state '%s'. Could not transition from '%s'. Resetting StateMachine.", stateMachine.c_str(), newstate.c_str(), state.c_str() );
			state = "default"; // Reset the state
		}
		//printf("Changing State:"); Lua::stackDump(L); // DEBUG
	}

	//printf("Complete:");Lua::stackDump(L); // DEBUG
	lua_settop(L,initialStackTop);
}

/**\brief Updates the AI controlled ship by first calling the Lua function
 * and then calling Ship::Update()
 */
void NPC::Update( lua_State *L ) {
	//Update enemies
	int t;

	if(enemies.size() > 0) {
		t = ChooseTarget( L );
		if(t != -1) {
			target = t;
			RegisterTarget( L, t );
		}
	}
	if( !this->IsDisabled() ) {
		this->Decide( L );
	}

	// Now act like a normal ship
	this->Ship::Update( L );
}

/**\brief The last function call to the ship before it get's deleted
 *
 * At this point, the ship still exists. It has not been removed from the Universe
 *
 * \note We don't want to make this a destructor call because we want the rest
 * of the system to still treat the ship normally.
 */
void NPC::Killed( lua_State *L ) {
	LogMsg( WARN, "AI %s has been killed\n", GetName().c_str() );
	SpriteManager *sprites = Scenario_Lua::GetScenario(L)->GetSpriteManager();

	Sprite* killer = sprites->GetSpriteByID( target );
	if(killer != NULL) {
		if( killer->GetDrawOrder() == DRAW_ORDER_PLAYER ) {
			((Player*)killer)->UpdateFavor( this->GetAlliance()->GetName(), -1 );
		}
	}
}


/**\brief Draw the AI Ship, and possibly debugging information.
 *
 * When the "options/development/debug-ai" flag is set, this will display the
 * current stateMachine and state below the Ship Spite.
 *
 * \see OPTION
 */
void NPC::Draw(){
	this->Ship::Draw();
	//if( OPTION(int,"options/development/debug-ai") ) {
	//	Coordinate position = this->GetWorldPosition();
	//	SansSerif->SetColor( WHITE );
	//	SansSerif->Render(position.GetScreenX(),position.GetScreenY()+GetImage()->GetHalfHeight(),stateMachine);
	//	SansSerif->Render(position.GetScreenX(),position.GetScreenY()+GetImage()->GetHalfHeight()+20,state);
	//}
}


/**\brief chooses who the AI should target given the list of the AI's enemies
 *
 */
int NPC::ChooseTarget( lua_State *L ){
	SpriteManager *sprites = Scenario_Lua::GetScenario(L)->GetSpriteManager();
	list<Sprite*> *nearbySprites = sprites->GetSpritesNear(this->GetWorldPosition(), COMBAT_RANGE, DRAW_ORDER_SHIP);
	
	nearbySprites->sort(CompareAI);
	list<Sprite*>::iterator it;
	list<enemy>::iterator enemyIt = enemies.begin();

	for(enemyIt = enemies.begin(); enemyIt != enemies.end(); ) {
		if(sprites->GetSpriteByID( enemyIt->id) == NULL) {
			enemyIt = enemies.erase(enemyIt);
		}
		else {
			enemyIt++;
		}
	}
	
	it = nearbySprites->begin();
	enemyIt = enemies.begin();
	int max = 0, currTarget =- 1;
	int threat = 0;
		
	for(it = nearbySprites->begin(); it != nearbySprites->end() && enemyIt != enemies.end() ; it++) {
		if( (*it)->GetID()== this->GetID() ) {
			continue;
		}

		if( (*it)->GetDrawOrder() == DRAW_ORDER_SHIP) {
			if( enemyIt->id < ((NPC*) (*it))->GetTarget() ){
				while ( enemyIt!=enemies.end() && enemyIt->id < ( (NPC*) (*it) )->GetTarget() ) {
					if ( !InRange( sprites->GetSpriteByID(enemyIt->id)->GetWorldPosition() , this->GetWorldPosition() ) ) {
						enemyIt=enemies.erase(enemyIt);
						threat=0;
						continue;
					}
					int cost= CalcCost( threat + ( (Ship*)sprites->GetSpriteByID(enemyIt->id))->GetTotalCost() , enemyIt->damage); //damage might need to be scaled so that the damage can be adquately compared to the treat level
					if(currTarget==-1 || max<cost){
						currTarget=enemyIt->id;
						max=cost;
					}
					threat = 0;
					enemyIt++;
				}
				it--;

				continue;
			}
			if( enemyIt->id == ((NPC*) (*it))->GetTarget() )
				threat-= ( (Ship*)(*it) )->GetTotalCost();
		} else {
			LogMsg( ERR, "Error Sprite %d is not an AI\n", (*it)->GetID() );
		}
	
	}

	while (enemyIt != enemies.end()) {
		if ( !InRange( sprites->GetSpriteByID(enemyIt->id)->GetWorldPosition() , this->GetWorldPosition() ) ) {
			enemyIt = enemies.erase(enemyIt);
			threat = 0;
			continue;
		}

		int cost= CalcCost( threat + ( (Ship*)sprites->GetSpriteByID(enemyIt->id))->GetTotalCost() , enemyIt->damage); //damage might need to be scaled so that the damage can be adquately compared to the treat level
		threat = 0;
		if( currTarget == -1 || max < cost) {
			max = cost;
			currTarget = enemyIt->id;
		}

		enemyIt++;
	}

	return currTarget;
	
}

/**\brief determines the potenital of an enemy as a target
 *
 */
int NPC::CalcCost( int threat, int damage) {
	return( threat * (damage + 1) ); //damage might need to be scaled so that the damage can be adquately compared to the threat level
}

/**\brief sets the AI's target
 *
 */
void NPC::SetTarget(int t) {
	AddEnemy(t, 0);
	target = t;
}

/**\brief Adds an enemy to the AI's list of enemies
 * \todo Remove the SpriteManager Instance access.
 */
void NPC::AddEnemy(int spriteID, int damage) {
	Sprite *spr = Menu::GetCurrentScenario()->GetSpriteManager()->GetSpriteByID(spriteID);

	if(!spr) {
		this->RemoveEnemy(spriteID);
		return;
	}

	int drawOrder = spr->GetDrawOrder();
	if( !(drawOrder == DRAW_ORDER_SHIP || drawOrder == DRAW_ORDER_PLAYER) ) {
		LogMsg(WARN, "Sprite ID %d is not a ship or player", spriteID);
		return;
	}

	// Don't forgive players that attack
	if( drawOrder == DRAW_ORDER_PLAYER ) {
		merciful = 0;
	}

	enemy newE;
	newE.id = spriteID;
	newE.damage = damage;

	// Search the enemies list for this sprite, combine damage taken if found.
	list<enemy>::iterator it = lower_bound(enemies.begin(), enemies.end(), newE, NPC::EnemyComp);
	if( (*it).id == spriteID ) {
		(*it).damage += damage;
	} else {
		enemies.insert(it,newE);
	}

	bool success = false;
	for(it = enemies.begin(); it != enemies.end(); it++) {
		if(it->id == spriteID) {
			success = true;
		}
	}

	assert(success);
}

/**\brief Remove an enemy from the AI's list of enemies
 */
void NPC::RemoveEnemy(int spriteID) {
	enemy newE;
	newE.id=spriteID;
	list<enemy>::iterator it = lower_bound(enemies.begin(), enemies.end(), newE, NPC::EnemyComp);

	if( (*it).id == spriteID) {
		enemies.erase(it);
	}

	for(it = enemies.begin(); it != enemies.end(); it++) {
		assert(spriteID != it->id);
	}
}

/**\brief sets the AI to hunt the current target using the lua function setHuntHostile
*
*/
void NPC::RegisterTarget( lua_State *L, int t ) {
	//Lua::Call( "setHuntHostile", "ii", this->GetID(), t );
}

/**\brief checks if a potential target is within targeting range
 *
 */
bool NPC::InRange(Coordinate a, Coordinate b) {
	//printf("InRange check\n");
	int x=a.GetX() - b.GetX();
	int y=a.GetY() - b.GetY();
	//printf("finished InRange check\n");
	return(x*x + y*y <=COMBAT_RANGE_SQUARED);
}


/**\brief sorts a list of AI's by their current target
 *
 */

bool NPC::CompareAI(Sprite* a, Sprite* b) {
	//printf("Start Comp AI\n");
	if(a->GetDrawOrder()==DRAW_ORDER_SHIP && b->GetDrawOrder()==DRAW_ORDER_SHIP)
		return(  ( (NPC*)a )->GetTarget() < ( (NPC*)b )->GetTarget() );
	else{
		if(a->GetDrawOrder()!=DRAW_ORDER_SHIP)
			LogMsg( ERR, "Error Sprite %d is not a ship\n", (a)->GetID() );
		if(b->GetDrawOrder()!=DRAW_ORDER_SHIP)
			LogMsg( ERR, "Error Sprite %d is not a ship\n", (b)->GetID() );
		return false;
	}
	//printf("finished Comp AI\n");
}
/**\fn AI::SetStateMachine(string _machine)
 * \brief Sets the state machine.
 */

/**\fn AI::SetState(string _state)
 * \brief Sets the current state.
 */

/**\fn AI::SetAlliance(Alliance* alliance)
 * \brief Sets the current alliance.
 */

/**\fn AI::GetStateMachine
 * \brief Retrieves the state machine.
 */

/**\fn AI::GetState
 * \brief Retrieves the current state.
 */

/**\fn AI::GetAlliance
 * \brief Retrieves the current alliance.
 * \warning Alliance may be NULL.
 */

/** @} */
