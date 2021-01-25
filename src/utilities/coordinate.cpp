/**\file			coordinate.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Sunday, November 22, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "engine/camera.h"
#include "utilities/coordinate.h"
#include "utilities/trig.h"

/**\class Coordinate
 * \brief Coordinates. */

Coordinate::Coordinate() {
	m_x = m_y = 0;
}

Coordinate::Coordinate( double x, double y ) {
	m_x = x;
	m_y = y;
}

Coordinate::~Coordinate () {

}

double  Coordinate::GetX () const {
	return ( m_x );
}

void  Coordinate::SetX ( double x ) {
	m_x = x;
}

double  Coordinate::GetY () const {
	return ( m_y );
}

void  Coordinate::SetY ( double y ) {
	m_y = y;
}

// Returns a SDL Rectangle using width/height of 0
SDL_Rect  Coordinate::getRect () {
	SDL_Rect rect;
	rect.x = (int) m_x;
	rect.y = (int) m_y;
	rect.w = rect.h = 0;
	return ( rect );
}

// Returns SDL Rectangle with given width and height
SDL_Rect  Coordinate::getRectWithDim ( int w, int h ) {
	SDL_Rect rect;
	rect.x = (int) m_x;
	rect.y = (int) m_y;
	rect.w = w;
	rect.h = h;
	return ( rect );
}

float Coordinate::GetAngle() {
	// 0 is right, 90 is up
	Trig *trig = Trig::Instance();
	return static_cast<float>(trig->RadToDeg( static_cast<float>(atan2( static_cast<float>(m_y)
					,static_cast<float>( m_x ))) ));
}

float Coordinate::GetMagnitude() {
	return static_cast<float>(sqrt( m_y*m_y + m_x*m_x ));
}

ostream& operator<<(ostream &out, const Coordinate &c) {
	out << "(" << c.m_x << ", " << c.m_y << ")";
	return out;
}

// Ensures coordinates are within boundaries, if set
void Coordinate::EnforceBoundaries( double top, double right, double bottom, double left ) {
	if( top != 0. ) {
		if( m_y > 0. ) {
			if( m_y > top ) m_y = top;
		}
	}
	if( right != 0. ) {
		if( m_x > 0. ) {
			if( m_x > right ) m_x = right;
		}
	}
	if( bottom != 0. ) {
		if( m_y < 0. ) {
			if( (-1 * m_y) > bottom ) m_y = -1 * bottom;
		}
	}
	if( left != 0. ) {
		if( m_x < 0. ) {
			if( (-1 * m_x) > left ) m_x = -1 * left;
		}
	}	
}

bool Coordinate::ViolatesBoundary( double top, double right, double bottom, double left ) {
	if( m_x < left ) return( true );
	if( m_x > right ) return( true );
	if( m_y < top ) return( true );
	if( m_y > bottom ) return( true );
	
	return( false );
}

void Coordinate::EnforceMagnitude(double radius) {
	// While the magnitude exceeds maximum, reduce X and Y by the same factor.
	// This allows the momentum angle to continue to absorb changes from the
	// accel angle even at max velocity.
	while(GetMagnitudeSquared() > radius*radius){
		SetX(GetX() * 0.99);
		SetY(GetY() * 0.99);
	}
}

Coordinate Coordinate::RotateBy( float angle ) {
	int newangle = TO_INT( normalizeAngle(angle + GetAngle()) );
	RotateTo( newangle );
	return *this;
}

Coordinate Coordinate::RotateTo( float newangle ) {
	Trig *trig = Trig::Instance();
	int angle = TO_INT( newangle );
	float radius = GetMagnitude();
	m_x = trig->GetCos( angle )*radius;
	m_y = -trig->GetSin( angle )*radius;
	return *this;
}

float randf()
{
	return (float)rand()/(float)RAND_MAX;
}

float gaussian()
{
	// from http://www.taygeta.com/random/gaussian.html
	// Algorithm by Dr. Everett (Skip) Carter, Jr.

	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * randf() - 1.0;
			x2 = 2.0 * randf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return y1;
}

Coordinate GaussianCoordinate()
{
	return Coordinate( gaussian(), gaussian() );
}
