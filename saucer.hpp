#ifndef SAUCER_H_
#define SAUCER_H_

#include "saucergame.hpp"

#include "HealthMeter.hpp"

class Saucer : public Thing
{
	int failCtr;
	HealthMeter 	healthMeter;
	bool  alive;
	int x;
	int y;
	int dx;
	int dy;
	sf::Texture itsTexture;
	sf::Sprite itsSprite;
	int inhibitLaser;
public:
	Saucer();
	void draw(sf::RenderTarget& window);
	void update();
private:
	void goLeft();
	void goRight();
	void goUp();
	void goDown();
	void goNot_lr();
	void goNot_ud();

	void scanKeyboard();

	int m_health;

public:
	Area getArea() const;

	CollisionSignature getColSig() const;
	void collideWith(CollisionSignature c);

	void shoot();
};

#endif
