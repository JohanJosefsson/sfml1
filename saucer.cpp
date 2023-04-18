#include "saucer.hpp"
#include <SFML/Audio.hpp>

class Laser : public Thing
{
	float/*int*/ x;
	float /*int*/ y;
	sf::Texture itsTexture;
	sf::Sprite itsSprite;
	int cnt;

	static sf::SoundBuffer buffer;
	static bool bufferIsLoaded;
	sf::Sound sound;
	static const int VISIBLE_CNT = (7*32);
	static const int LIFETIME_CNT = (4*14*32);

public:
	int getY();
	void draw(sf::RenderTarget& window);
	void update();
	Laser(int x, int y);
	Area getArea() const;
	bool toBeDeleted() const;


	CollisionSignature getColSig() const;

};

//sf::SoundBuffer Laser::buffer;
bool Laser::bufferIsLoaded;

CollisionSignature Laser::getColSig() const
{
	if(cnt < VISIBLE_CNT)
		return LASER;
	return UNDEF;
}


bool Laser::toBeDeleted() const
{
	if(cnt > LIFETIME_CNT)
	{
		return true; 
    } else {
		return false;
	}
}


Area Laser::getArea() const
{
	return Area(x, y, 2, 32);
}



int Laser::getY()
{
	return y;
}

Laser::Laser(int x, int y)
{
	this->x = x;
	this->y = y;
	cnt = 0;
	itsTexture.loadFromFile("../res/laser.png", sf::IntRect(0, 0, 2, 32));
	itsSprite.setTexture(itsTexture);
	itsSprite.setPosition(x,y);

	if(!bufferIsLoaded)
	{
//		buffer.loadFromFile("../res/skyrocket-01.wav");
		bufferIsLoaded = true;
	}
//	sound.setBuffer(buffer);



	//Detta sabba alltihop!!

//	SoundPlayer soundPlayer(sound);
	sound.play();


}

void Laser::draw(sf::RenderTarget& window)
{	
	if(cnt < VISIBLE_CNT)
	{
		itsSprite.setPosition(x,y);
		window.draw(itsSprite);

//		std::cout << x << "," << y << std::endl;
	}
}
void Laser::update()
{
	y-=7;
	cnt+=7;
}






void Saucer::collideWith(CollisionSignature c)
{

	switch(c)
	{
	case MONSTER:
		alive = false;
		m_health = 0;
		healthMeter.setHealth(m_health);
		break;
	case MONSTER_WEAK_WEPON:
		m_health -= 10;
		healthMeter.setHealth(m_health);
		break;
	}
}


void Saucer::shoot()
{
	if(!alive)return;
	if(inhibitLaser < 250/7)return;
	inhibitLaser = 0;
	Laser * pLaser = new Laser (x + 32/2, y - 32);
	ThingKeeper::enlist(pLaser);
	
}


CollisionSignature Saucer::getColSig() const
{
	return SAUCER;
}

Area Saucer::getArea() const
{
	return Area(x, y, 32, 32);
}



void Saucer::goLeft()
{
	dx = -7;
}
void Saucer::goRight()
{
	dx = 7;
}
void Saucer::goUp()
{
	dy = -7;
}
void Saucer::goDown()
{
	dy = 7;
}
void Saucer::goNot_lr()
{
	dx = 0;
//	dy = 0;
}

void Saucer::goNot_ud()
{
//	dx = 0;
	dy = 0;
}

void Saucer::scanKeyboard()
{

		// 1. Scan the keyboard and manage input
		//
		bool going_lr = false, going_ud = false;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
				goLeft();
				going_lr = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
				goRight();
				going_lr = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
				goUp();
				going_ud = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
				goDown();
				going_ud = true;
		}

		if(!going_lr)
		{
			goNot_lr();
		}



		if(!going_ud)
		{
			goNot_ud();
		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
				shoot();
		}


}

void Saucer::update()
{

	if(!(--failCtr)){
		Levels::finish(Levels::FAILURE);
	}
	if(!alive)
	{
		return;
	}
	if(m_health<=0){
		if(failCtr<=0)failCtr = 500;
		return;
	}

	scanKeyboard();








	x += dx;
	y += dy;

	x += (x < 16)?(800-16):0;
	x %= 800 - 16;
	y += (y < 16)?(600-16):0;
	y %= 600 - 16;

	inhibitLaser++;
}

Saucer::Saucer():
x(400), y(500), dx(0), dy(0), inhibitLaser(0), alive(true),m_health(100),healthMeter(HealthMeter::GREEN, 50,580,100,10),failCtr(0)
{
	itsTexture.loadFromFile("../res/saucer.png");
	itsSprite.setTexture(itsTexture);
	itsSprite.setPosition(x,y);
}

void Saucer::draw(sf::RenderTarget& window)
{
	window.draw(healthMeter);
	if(!alive)return;
	itsSprite.setPosition(x,y);
	window.draw(itsSprite);
}
