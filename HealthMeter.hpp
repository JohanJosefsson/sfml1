#ifndef HEALTH_METER_H
#define HEALTH_METER_H


#include <SFML/Graphics.hpp>
//#include "saucergame.hpp"
#include <iostream>

#include <cstdint>
#include <cassert>

class HealthMeter : public sf::Drawable
{
public:
	enum Color { RED, GREEN };
	HealthMeter(enum Color color, int x, int y, int w, int h):
	  m_x(x),m_y(y),m_w(w),m_h(h),
	  m_vertices_needle(sf::Quads, 4),
	  m_vertices_frame(sf::LinesStrip, 5),
	  m_percentage(100)
	  {

		  uint8_t r, g, b, a;
		  sf::Color framecolor;

		  switch(color)
		  {
		  case RED:
			  r = 255;
			  g = 75;
			  b = 75;
			  a = 200;
			  framecolor = sf::Color::Red;
			  break;
		  case GREEN:
			  r = 75;
			  g = 255;
			  b = 75;
			  a = 200;
			  framecolor = sf::Color::Green;

			  break;
		  default:
			  assert(0);
		  }




		  for(unsigned i = 0; i < m_vertices_needle.getVertexCount();i++)
		  {
			  m_vertices_needle[i].color.r = static_cast<sf::Uint8>(r);
			  m_vertices_needle[i].color.g= static_cast<sf::Uint8>(g);
			  m_vertices_needle[i].color.b= static_cast<sf::Uint8>(b);
			m_vertices_needle[i].color.a = static_cast<sf::Uint8>(a);
		  }

		  m_vertices_needle[0].position = sf::Vector2f(x,y);
		  m_vertices_needle[1].position = sf::Vector2f(x+w,y);
		  m_vertices_needle[2].position = sf::Vector2f(x+w,y+h);
		  m_vertices_needle[3].position = sf::Vector2f(x,y+h);




		  for(unsigned i = 0; i < m_vertices_frame.getVertexCount();i++)
		  {
			  m_vertices_frame[i].color = framecolor;//sf::Color::Red;
		  }


		   
		  
		  m_vertices_frame[0].position = sf::Vector2f(x-1,y-1);
		  m_vertices_frame[1].position = sf::Vector2f(x+w,y);
		  m_vertices_frame[2].position = sf::Vector2f(x+w,y+h);
		  m_vertices_frame[3].position = sf::Vector2f(x,y+h);
		  m_vertices_frame[4].position = sf::Vector2f(x,y);

#if 0
		  m_vertices_frame[0].position = sf::Vector2f(x,y);
		  m_vertices_frame[1].position = sf::Vector2f(x+w,y);

		  m_vertices_frame[2].position = sf::Vector2f(x+w,y);
		  m_vertices_frame[3].position = sf::Vector2f(x+w,y+h);

		  m_vertices_frame[4].position = sf::Vector2f(x+w,y+h);
		  m_vertices_frame[5].position = sf::Vector2f(x,y+h);

		  m_vertices_frame[6].position = sf::Vector2f(x,y+h);
		  m_vertices_frame[7].position = sf::Vector2f(x,y);
#endif

	  }


public:
	void setHealth(int percentage)
	{
		if(percentage != m_percentage)
		{
//		std::cout << percentage << std::endl;
		}
		assert(percentage <= 100);
		if(percentage < 0)percentage = 0;

		int perc_w = m_w * percentage / 100.0;



//		  m_vertices_needle[1].position = sf::Vector2f(x+w,y);
//		  m_vertices_needle[2].position = sf::Vector2f(x+w,y+h);

		assert(perc_w <= m_w);

				  m_vertices_needle[1].position = sf::Vector2f(m_x+perc_w,m_y);
		  m_vertices_needle[2].position = sf::Vector2f(m_x+perc_w,m_y+m_h);

	}


	private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {

        // draw the vertex array
		target.draw(m_vertices_needle);
        target.draw(m_vertices_frame);
    }

    sf::VertexArray m_vertices_frame;
	sf::VertexArray m_vertices_needle;
	int m_x;
	int m_y;
	int m_w;
	int m_h;

	int m_percentage;
};

#endif /* HEALTH_METER_H */
