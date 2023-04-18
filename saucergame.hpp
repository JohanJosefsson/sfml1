#ifndef SAUCERGAME_HPP
#define SAUCERGAME_HPP

#include <SFML/Graphics.hpp>

#include <list>
#include <cassert>

#define timePerFrame (sf::seconds(1.f / (100*1.f)))

class Area
{
	float x;
	float y;
	int width;
	int height;
	Area();
public:
	Area(float x, float y, int w, int h);
	bool isOverlapping(Area & that) const;

	int getCenterX() const;
	int getCenterY() const;

};



// TBD find out how to do this application independent
enum CollisionSignature {
	UNDEF,
    SAUCER,
    MONSTER,
	LASER,
	MONSTER_WEAK_WEPON
};

class Thing
{
public:
	virtual void draw(sf::RenderTarget& window) = 0;
	virtual void update() = 0;
	virtual Area getArea() const = 0;
	virtual bool toBeDeleted() const;
	virtual CollisionSignature getColSig() const;
	virtual void collideWith(CollisionSignature c);
	virtual ~Thing();
};


class ThingKeeper
{
	static	std::list<Thing *> L;
	static void collideThings(Thing& t1, Thing& t2);
public: 
	static void enlist(Thing * p);
	static void remove();

	static void update();
	static void collide();
	static void draw(sf::RenderTarget& window);
	static void delete_all();
//	static void clean();
};



 class Level
 {
 public:
	 virtual ~Level()
	 {}
	virtual void draw(sf::RenderTarget& window) = 0;
 };




 class AaLevel:public Level
 {
	 sf::Texture bkgTexture;
	 sf::Sprite bkgSprite;
 protected:
	 void construct(const char * filename)
	 {
		 bool res;
		 res = bkgTexture.loadFromFile(filename);
		 assert(res);
		 bkgSprite.setTexture(bkgTexture);

		 construct_also();

	 }
	 virtual void construct_also()
	 {
/*       // Example:		 
		 Saucer * pSaucer = new Saucer;
		 ThingKeeper::enlist(pSaucer);

		 Walker * pWalker = new Walker;
		 ThingKeeper::enlist(pWalker);

		 Primus::Monster * pMonster = new Primus::Monster;
		 ThingKeeper::enlist(pMonster);
*/
	 }
	 void draw(sf::RenderTarget& window)
	 {
		 window.draw(bkgSprite);
	 }

	 virtual ~AaLevel()
	 {
		 ThingKeeper::delete_all();
	 }
 };






 
class Levels
{
public:
	enum Result {
		NOT_DEFINED,
		SUCCESS,
		FAILURE,
		PROCEED,
		RESTART,
	};
	static void draw(sf::RenderTarget& window);
	static void finish(Result result);

	static void start_level(int level);

private:
	static int current_level;
	static Level * pItsLevel;
	static Result m_result;
};






#endif
