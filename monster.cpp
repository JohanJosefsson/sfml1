#include "monster.hpp"

#include <iostream>

namespace Primus {




class Spit:public Thing
{
private:
	float m_x;
	float m_y;

	static const int W = 12;
	static const int H = 12;

	sf::Texture itsTexture;
	sf::Sprite itsSprite;

	bool active;

public:

	~Spit()
	{
		std::cout << "active = " << active << std::endl;
	}

	Spit(int x, float y):
	m_x(x - W/2),m_y(y - H/2),active(true)
	{
		itsTexture.loadFromFile("../res/spit.png", sf::IntRect(0, 0, W, H));
		itsSprite.setTexture(itsTexture);
	}

	CollisionSignature getColSig() const
	{
		if(active)
		{
			return MONSTER_WEAK_WEPON;
		} else {
			return UNDEF;
		}
	}

	Area getArea() const
	{
		return Area(m_x, m_y, W, H);
	}


	void update()
	{
		m_y += 0.3 * 7;
	}

	void draw(sf::RenderTarget& window)
	{
		itsSprite.setPosition(m_x,m_y);
		window.draw(itsSprite);

	}

	void collideWith(CollisionSignature c)
	{

//		std::cout << "collide!" << std::endl;
		switch(c) {
		case SAUCER:
			active = false;
			break;
		}
	}

	bool toBeDeleted() const
	{
		if(m_y < 600/* && active*/)
		{
			return false;
		} else {
			return true;
		}
	}
	

};



CollisionSignature Monster::getColSig() const
{
	if(FLYING == state)
		return MONSTER;
	return UNDEF;
}

Area Monster::getArea() const
{
	return Area(x, y, 40, 40);
}

Monster::Monster()
: x(300), y(100), dx(0), dy(0), updateCnt(0), ringFactor(3.0), isHit(0), centers_index(0), state(FLYING), life(250), particleSystem(350/*500*/),
healthMeter(HealthMeter::RED,50,8,100,10)
{
	itsTextures[0].loadFromFile("../res/monster2.png", sf::IntRect(0, 0, 40, 40));
	itsTextures[1].loadFromFile("../res/monster2.png", sf::IntRect(40, 0, 40, 40));
	itsTextures[2].loadFromFile("../res/monster2.png", sf::IntRect(80, 0, 40, 40));
	itsTextureTriumph.loadFromFile("../res/monster_triumph.png", sf::IntRect(0, 0, 40, 40));

	itsSprite.setTexture(itsTextures[2]);
	itsSprite.setPosition(x,y);

	ringTexture.loadFromFile("../res/ring.png");
	ringSprite.setTexture(ringTexture);
	ringSprite.setOrigin(sf::Vector2f(200, 200));

	triumphBuffer.loadFromFile("../res/lion-01.wav");
	triumphSound.setBuffer(triumphBuffer);
	defeatedBuffer.loadFromFile("../res/explosion-01.wav");
	defeatedSound.setBuffer(defeatedBuffer);

//	healthMeter(20,2,100,10);

}

Monster::~Monster()
{
//delete pItsSprite;	

}

#define sig(x) (0==x)?0:((x<0)?(-1):1)

void Monster::update()
{
	if(explosionCtr)
	{
		if(!(--explosionCtr))	Levels::finish(Levels::SUCCESS);
	}

	if(triumphCtr)
	{
#if 0
		if(!(--triumphCtr))	Levels::finish(Levels::FAILURE);
#endif
	}


	for(int i = 0; i < 7; i++) { // framerate was divided by seven
	if(state == DEFEATED) {
		particleSystem.update(timePerFrame);
		return;
	}
	if(state == TRIUMPHANT)return;


	// Spit with some probability
	if(!(rand()%500))
	{
		Spit * pSpit = new Spit(x + 20, y + 40);
		ThingKeeper::enlist(pSpit);
	};

	// Let the tentacles and face move
	if(updateCnt%500 == 0)
	itsSprite.setTexture(itsTextures[(updateCnt)%3]);
#define	SLOWNESSS 140

	// Set direction
	if(0 == rand()%SLOWNESSS)
	{
		dx = rand()%3 -1;
	}

	if(0 == rand()%SLOWNESSS)
	{
		dy = rand()%3 -1;
	}



	static const struct {int x; int y;} centers[] = {{300, 200},{600, 400},{600, 200},{300, 400}};

	if(rand()%500 == 0)
	{
		centers_index = rand()%4;
	}

	// ...increase the pobability to move towards the center (300, 200), if far away form it
	int center_x = (x-centers[centers_index].x)*(x-centers[centers_index].x)* (rand()%100);
	if(center_x > 2000000)
	{
		dx = sig(centers[centers_index].x-x);
	}
	int center_y = (y-centers[centers_index].y)*(y-centers[centers_index].y)* (rand()%200);
	if(center_y > 2000000)
	{
		dy = sig(centers[centers_index].y-y);
	}


	// Move
	if(0 == updateCnt%5)
	{
		x += dx;
		y += dy;
	}

	// Handle the ring
	ringFactor =  0.1*(updateCnt%10);

	// count
	updateCnt++;
	}
}

void Monster::draw(sf::RenderTarget& window)
{
		window.draw(healthMeter);

	if(state == DEFEATED)
	{
		window.draw(particleSystem);
		return;
	}
	itsSprite.setPosition(x,y);
	window.draw(itsSprite);


	ringSprite.setScale(ringFactor, ringFactor);
	ringSprite.setPosition(  x + 20 , y + 20);
	if(isHit > 0){
		window.draw(ringSprite);
		isHit--;
	}
}

void Monster::collideWith(CollisionSignature c)
{
	if(state == DEFEATED) return;
	if(state == TRIUMPHANT) return;
	switch(c)
	{
	case LASER:
		life--;
		healthMeter.setHealth(100 * life / 250.f);
//		std::cout << life << std::endl;
		if(0 == life)
		{
			state = DEFEATED;
			defeatedSound.play();
			particleSystem.setEmitter(sf::Vector2f(x, y));
			explosionCtr = 250;

		}
		isHit = 500;
		break;
	case SAUCER:
		state = TRIUMPHANT;
		triumphCtr = 250;
		triumphSound.play();
		itsSprite.setTexture(itsTextureTriumph);
		break;
	}
}

}//namespace