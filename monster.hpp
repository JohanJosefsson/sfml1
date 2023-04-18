#include "saucergame.hpp"
#include <SFML/Audio.hpp>

#include "ParticleSystem.hpp"
#include "HealthMeter.hpp"

namespace Primus {
class Monster : public Thing
{
	int explosionCtr;
	int triumphCtr;
	HealthMeter 	healthMeter;
	ParticleSystem particleSystem;
	/*static*/ sf::SoundBuffer triumphBuffer;
	/*static*/ sf::SoundBuffer defeatedBuffer;
	sf::Sound triumphSound;
	sf::Sound defeatedSound;
	int life;
	enum State
	{
		FLYING,
		DEFEATED,
		TRIUMPHANT,
	};
	State state;
	sf::Texture itsTextures[3];
	sf::Texture itsTextureTriumph;
	sf::Sprite itsSprite;
	float  x;
	float y;
	int dx;
	int dy;
	int updateCnt;

	sf::Texture ringTexture;
	sf::Sprite  ringSprite;
	float ringFactor;

	int isHit;

	int centers_index;

public:
	Monster();
	void update();
	void draw(sf::RenderTarget& window);
	Area getArea() const;
	~Monster();

	CollisionSignature getColSig() const;
	void collideWith(CollisionSignature c);


};
};