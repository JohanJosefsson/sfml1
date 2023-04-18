
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cassert>
#include <list>
#include "saucergame.hpp"
#include "monster.hpp"

#include "saucer.hpp"

#include "HealthMeter.hpp"

#include "secundo.hpp"

// https://github.com/LaurentGomila/SFML/wiki/Tutorial%3A-Create-and-Use-SetIcon
extern "C" const struct {
  unsigned  	 width;
  unsigned  	 height;
  unsigned  	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char 	 pixel_data[32 * 32 * 4 + 1];
} gimp_image;


int Area::getCenterX() const
{
	return(x + width / 2);
}
int Area::getCenterY() const
{
	return(y + height / 2);
}


Area::Area(float x, float y, int w, int h)
{
	this->x = x;
	this->y = y;
	width = w;
	height = h;
}

bool Area::isOverlapping(Area& that) const
{
	return(
		that.x + width > this->x &&
		that.x < this->x + this->width &&


		that.y + height > this->y &&
		that.y < this->y + this->height);
}

void Thing::collideWith(CollisionSignature c)
{
}

CollisionSignature Thing::getColSig() const
{
	return UNDEF;
}


bool Thing::toBeDeleted() const
{
	return false;
}


Thing::~Thing()
{
}



std::list<Thing *> ThingKeeper::L;

void ThingKeeper::enlist(Thing * p)
{
	L.push_back(p);

}

void ThingKeeper::remove()
{
//	std::cout << L.size() << std::endl;

	// http://en.wikipedia.org/wiki/Erase-remove_idiom
	L.erase(
		std::remove_if(
			L.begin(),
			L.end(),
			[](Thing * pThing)->bool
			{
				bool r = pThing->toBeDeleted();
				if(r) delete pThing;
				return r;
				//return pThing->toBeDeleted();
			}
		),
		L.end()
	);

		// BUG! Minnesläcka, det pekaren pekar på frias aldrig, vad? fixad?

#if 0
	//http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
	std::list<Thing *>::iterator i;
	i=L.begin();
	while(i != L.end())
	{
		if((*i)->toBeDeleted())
		{
			delete *i;
			L.erase(i++);
		} else  {
			i++;
		}
	}
#endif
}

void ThingKeeper::update()
{

	// http://stackoverflow.com/questions/11956374/for-loop-vs-stdfor-each-with-lambda

	std::for_each(L.begin(), L.end(), [](Thing * pThing){pThing->update();});

#if 0
	std::list<Thing *>::iterator i; 
	for(i=L.begin(); i != L.end(); ++i)
	{
		(*i)->update();
	}
#endif
}

void ThingKeeper::collideThings(Thing& t1, Thing& t2)
{
	t1.collideWith(t2.getColSig());
	t2.collideWith(t1.getColSig());
}


void ThingKeeper::collide()
{
	std::list<Thing *>::iterator i; 
	std::list<Thing *>::iterator j; 

	for(i=L.begin(); i != L.end(); ++i)
	{
		for(j = i; j != L.end(); ++j)
		{
			if(i != j && (*i)->getArea().isOverlapping((*j)->getArea()))
			{
				collideThings(**i, **j);
			}
		}
	}
}
void ThingKeeper::draw(sf::RenderTarget& window)
{
	std::list<Thing *>::iterator i;
	std::for_each(L.begin(), L.end(), [&](Thing * p){p->draw(window);});
#if 0
	for(i=L.begin(); i != L.end(); ++i)
	{
		(*i)->draw(window);
	}
#endif
}


void ThingKeeper::delete_all()
{
	std::for_each(L.begin(), L.end(), [](Thing * pThing){delete pThing;});
	L.clear();
}




 class PrimusLevel:public Level
 {
	 sf::Texture bkgTexture;
	 sf::Sprite bkgSprite;
 public:
	 PrimusLevel()
	 {
		 			//sf::Texture bkgTexture;
			bkgTexture.loadFromFile("../res/bkg.png");
			//sf::Sprite bkgSprite;
			bkgSprite.setTexture(bkgTexture);

			Saucer * pSaucer = new Saucer;
			ThingKeeper::enlist(pSaucer);

			Primus::Monster * pMonster = new Primus::Monster;
			ThingKeeper::enlist(pMonster);

			// TODO: 
			// rensa listan


	 }
	void draw(sf::RenderTarget& window)
	 {
		 window.draw(bkgSprite);
	}

	 ~PrimusLevel()
	 {
		 ThingKeeper::delete_all();
	 }


 };



 



 class IntroLevel:public Level
 {
	 sf::Font font;

 public:
	 IntroLevel()
	 {
		 if (!font.loadFromFile("C:\\Windows\\fonts\\arial.ttf"))
			 // if (!font.loadFromFile("../res/bkg.png"))
		 {
			 std::cout << "Shit pommes frites" << std::endl;
			 // error...
		 }

	 }

	 void draw(sf::RenderTarget& window)
	 {
		 sf::Text text;
		 // select the font
		 text.setFont(font); // font is a sf::Font
		 // set the string to display
		 text.setString("Press SPACE to start");
		 // set the character size
		 text.setCharacterSize(24); // in pixels, not points!
		 // set the color
		 text.setFillColor(sf::Color::Red);
		 //text.setColor(sf::Color::Red);
		 // set the text style
		 text.setStyle(sf::Text::Bold | sf::Text::Underlined);
		 text.setPosition(200,400);
		 sf::Text text1;
		 // select the font
		 text1.setFont(font); // font is a sf::Font
		 // set the string to display
		 text1.setString(
			 "The Old Man (Farsan) and the Lad (Gossen a.k.a. Pajken) designed and built a contraption\n"
			 "that would make it possible to travel in time. Or so their goal was. They wanted to be\n"
			 "able to turn back the time for maybe five or ten minutes when time is short in the\n"
			 "mornings.\n\n"
			 "Their experiment went terribly wrong though. Now they are stuck somewhere far away in\n"
			 "outer space, in a place so far away that the ordinary laws of nature do not reach there.\n\n"
			 "With their ingenuity, hard work and their bare hands, they have managed to build a space\n"
			 "craft that will hopefully one day bring them back to planet Earth.\n\n"
			 "Their first mission is however to fend off some space monsters that usually threaten space\n"
			 "mariners who fall off the edge of the universe. Now these monsters have entered the\n"
			 "inhabited part of the universe and must be chased away."
			 );

		 // set the character size
		 text1.setCharacterSize(16); // in pixels, not points!
		 // set the color
		 text1.setFillColor(sf::Color::Green);
		 // set the text style
		 text1.setStyle(sf::Text::Bold);
		 text1.setPosition(36,36);

		 // inside the main loop, between window.clear() and window.display()
		 window.draw(text);
		 window.draw(text1);

		 if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		 {
			 Levels::finish(Levels::PROCEED);
		 }
	 }
 };





 
 class AfterTextLevel:public Level
 {
	 sf::Font font;

 public:
	 AfterTextLevel()
	 {
		 if (!font.loadFromFile("C:\\Windows\\fonts\\arial.ttf"))
			 // if (!font.loadFromFile("../res/bkg.png"))
		 {
			 std::cout << "Shit pommes frites" << std::endl;
			 // error...
		 }

	 }

	 void draw(sf::RenderTarget& window)
	 {




		 sf::Text text;
		 // select the font
		 text.setFont(font); // font is a sf::Font
		 // set the string to display
		 text.setString("Press SPACE to restart");
		 // set the character size
		 text.setCharacterSize(24); // in pixels, not points!
		 // set the color
		 text.setFillColor(sf::Color::Red);
		 // set the text style
		 text.setStyle(sf::Text::Bold | sf::Text::Underlined);
		 text.setPosition(200,400);




		 sf::Text text1;
		 // select the font
		 text1.setFont(font); // font is a sf::Font
		 // set the string to display
		 text1.setString(
			 "Congrats! You survived and this corner of the universe is no longer under threat"
			 );

		 // set the character size
		 text1.setCharacterSize(16); // in pixels, not points!
		 // set the color
		 text1.setFillColor(sf::Color::Green);
		 // set the text style
		 text1.setStyle(sf::Text::Bold);
		 text1.setPosition(36,36);

		 // inside the main loop, between window.clear() and window.display()
		 window.draw(text);
		 window.draw(text1);

		 if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		 {
			 Levels::finish(Levels::RESTART);
		 }
	 }
 };



 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 


 
 class FailureLevel:public Level
 {
	 sf::Font font;

 public:
	 FailureLevel()
	 {
		 if (!font.loadFromFile("C:\\Windows\\fonts\\arial.ttf"))
			 // if (!font.loadFromFile("../res/bkg.png"))
		 {
			 std::cout << "Shit pommes frites" << std::endl;
			 // error...
		 }

	 }

	 void draw(sf::RenderTarget& window)
	 {




		 sf::Text text;
		 // select the font
		 text.setFont(font); // font is a sf::Font
		 // set the string to display
		 text.setString("Press SPACE to restart");
		 // set the character size
		 text.setCharacterSize(24); // in pixels, not points!
		 // set the color
		 text.setFillColor(sf::Color::Red);
		 // set the text style
		 text.setStyle(sf::Text::Bold | sf::Text::Underlined);
		 text.setPosition(200,400);




		 sf::Text text1;
		 // select the font
		 text1.setFont(font); // font is a sf::Font
		 // set the string to display
		 text1.setString(
			 "Oh no! You were defeated and the future of the universe is unsure..."
			 );

		 // set the character size
		 text1.setCharacterSize(16); // in pixels, not points!
		 // set the color
		 text1.setFillColor(sf::Color::Green);
		 // set the text style
		 text1.setStyle(sf::Text::Bold);
		 text1.setPosition(36,36);

		 // inside the main loop, between window.clear() and window.display()
		 window.draw(text);
		 window.draw(text1);

		 if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		 {
			 Levels::finish(Levels::RESTART);
		 }
	 }
 };


 
 
 
 
 
 
 
 void Levels::draw(sf::RenderTarget& window)
	{
		pItsLevel->draw(window);


//#if 0
		switch(m_result)
		{
		case SUCCESS:
		case PROCEED:
			current_level++;
			start_level(current_level);
		break;
		case RESTART:
			current_level = 1;
			start_level(current_level);
			break;
		case FAILURE:
			current_level = -1;
			start_level(current_level);
			break;

		}
//#endif
		//if(NOT_DEFINED != m_result)
		//{
			//start_level(0);
			m_result = NOT_DEFINED;
		//}
	}

void Levels::finish(Result result)
{
	m_result = result;
}



void Levels::start_level(int level)
	{
			if(pItsLevel)delete pItsLevel;

		switch(level) {
		case -1:
			pItsLevel = new FailureLevel();
			break;
		case 0:
			pItsLevel = new IntroLevel();

			break;

		case 1:
			pItsLevel = new Secundo::ALevel();
			break;


		case 2:
			pItsLevel = new PrimusLevel();
			break;


			break;
		default:
			pItsLevel = new AfterTextLevel();
			break;
		}
	}


Level * Levels::pItsLevel;
int Levels::current_level;
Levels::Result Levels::m_result;






int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Saucer");
//	sf::Window window(sf::VideoMode(800, 600, 32), "SFML Window", sf::Style::Default, sf::ContextSettings(32));
	

	////////////////////////////////////////////////////////////
    /// This line is added! Make sure the name of your image is the same
    ///
	window.setIcon( gimp_image.width,  gimp_image.height,  gimp_image.pixel_data); 
    ///
    ////////////////////////////////////////////////////////////



#if 0
		 sf::Font font;
		 if (!font.loadFromFile("C:\\Windows\\fonts\\arial.ttf"))
			{
				std::cout << "Shit pommes frites yo" << std::endl;
				// error...
			}
#endif





#if 0
	sf::Texture bkgTexture;
	bkgTexture.loadFromFile("../res/bkg.png");
	sf::Sprite bkgSprite;
	bkgSprite.setTexture(bkgTexture);

	Saucer saucer;
	ThingKeeper::enlist(&saucer);

	Primus::Monster monster;
	ThingKeeper::enlist(&monster);
#endif

//	sf::Sprite bkgSprite;
	/*bkgSprite = */Levels::start_level(0);


//	HealthMeter healthMeter(200,100,10,10);

	sf::Clock clock; // starts the clock
	sf::Time timeSinceLastUpdate = sf::Time::Zero;





	sf::View view; // bara testar lite...
	view.reset(sf::FloatRect(0, 0, 800, 600));
	//view.setViewport(sf::FloatRect(0, 0, 0.5f, 0.5f));


	while (window.isOpen())
    {



		// 2. Check the clock and update if time is due
		//

		timeSinceLastUpdate += clock.restart();
		while(timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			ThingKeeper::update();			
		}


		// 3. Check and manage collisions
		//

		ThingKeeper::collide();

		// 4. Clean up dead objects
		//
		ThingKeeper::remove();


		// 5. Draw
		//
        sf::Event event;
        while (window.pollEvent(event))
        {
			switch(event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			}
        }
        window.clear();
//		window.draw(bkgSprite);

		window.setView(view);
		Levels::draw(window);
		ThingKeeper::draw(window);
//		window.draw(healthMeter);

        window.display();

    }

    return 0;
}


