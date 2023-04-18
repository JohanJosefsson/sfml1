#include "saucergame.hpp"
#include <vector>
#include <iostream>


#include <cassert>
#include "StateManager.hpp"































namespace Secundo {

class Thingy : public Thing
{
public:
//	Thingy(){};
	virtual void draw(sf::RenderTarget& window)
	{
		itsSprite.setPosition(x, y);
		if(visible)
			window.draw(itsSprite);
	}

//	virtual void update() = 0;
	virtual Area getArea() const
	{
		return Area(x, y, w, h);
	}
	virtual bool toBeDeleted() const
	{
		return deleteFlag;
	}
	virtual CollisionSignature getColSig() const
	{
		return c;
	}
//	virtual void collideWith(CollisionSignature c);
//	virtual ~Thingy();

protected:
	//
	void move()
	{
		x += dx;
		y += dy;
	}
//public:
protected:
	float x;
	float y;
	float dx;
	float dy;
	int w;
	int h;
	CollisionSignature c;
	sf::Sprite itsSprite;
	bool visible;
	bool deleteFlag;

};


class Timeout_cb
{
public:
	virtual void on_timeout(int sig) = 0;
};

class TimerManager
{
	struct Timer {int value; int sig;};
	void onTimeout(int sig)
	{
//		std::cout << "timeout has occurred" << std::endl;
		itsTimeout_cb->on_timeout(sig);
	}
public:
	void print()
	{
//		std::cout << "TimerManager:" << std::endl;
//		std::cout << "size=" << timers.size() << std::endl;

		std::for_each(timers.begin(), timers.end(), [](Timer & timer)
		{
//			std::cout << timer.sig << " " << timer.value << std::endl;
		});


	}



	void addTimer(int id, int value, int sig)
	{
		Timer t;
		t.value = value;
		t.sig = sig;
		timers[id] = t;
	}


	void cancelTimer(int id)
	{
		Timer t;
		t.value = 0;
		t.sig = 0;
		timers[id] = t;
	}


	void serviceTimers()
	{
		std::for_each(timers.begin(), timers.end(), [&](Timer & timer)
		{
			if(!(--timer.value))
			{
				onTimeout(timer.sig);
			};
		});
	};

	TimerManager(int num_of, Timeout_cb * tmocb):timers(num_of)
	{
		itsTimeout_cb = tmocb;
	}

private:
	Timeout_cb * itsTimeout_cb;
//	struct Timer {int value; int sig;};
	std::vector<struct Timer>timers;
};










class Walker :public  Thingy, Timeout_cb
{
public:
	Walker():itsStateManager(this, states, NUMBER_OF_STATES, going_right), itsTimerManager(NUMBER_OF_STATES, this), inited(false)

	{


		x = 100.0;
		y = 100.0;
		w = 32;
		h = 32;
		c = UNDEF;
		dx = 0;
		dy = 0;
		visible = true;
		deleteFlag = false;


		itsTexture.loadFromFile("../res/sec.png");
		itsSprite.setTexture(itsTexture);
		itsSprite.setPosition(x,y);
	};

	void on_timeout(int sig)
	{
//		std::cout << "on_timeout" << std::endl;
		itsStateManager.send_synchd(TMO);
		//	itsStateManager.process();

	};

	virtual void update()
	{
		if(!inited)
		{
			itsStateManager.send(INIT);
			itsStateManager.process();
			inited = true;
		}
	//	itsTimerManager.print();
		itsTimerManager.serviceTimers();
		move();
	};


private:
	sf::Texture itsTexture;


private:

	enum State
	{
		top,
		  going_right,
		  going_left,
	

		NUMBER_OF_STATES,
		NO_STATE,
	};

	enum Signal
	{
		ON_ENTRY,
		ON_EXIT,
		ON_INIT,
		// user signals:
		TMO,
		INIT,
	};



	bool top_handler(int sig)
	{
		return false;
	}



	
	bool going_right_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			itsTimerManager.addTimer( going_right, 100, TMO);
			dx = 1;
			return true;
		case ON_EXIT:
			dx = 0;
			return true;
		case TMO:
			itsStateManager.set_next_state(going_left);
			return true;
		}
		return false;
	}


	
	
	bool going_left_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			itsTimerManager.addTimer( going_right, 100, TMO);
			dx = -1;
			return true;
		case ON_EXIT:
			dx = 0;
			return true;
		case TMO:
			itsStateManager.set_next_state(going_right);
			return true;
		}
		return false;
	}





	StateManager<Walker> itsStateManager;
	static StateManager<Walker>::StateDesc states[];

	TimerManager itsTimerManager;
	bool inited;
	

};

StateManager<Walker>::StateDesc Walker::states[] = 
{
	{top, NO_STATE, NO_STATE, &Walker::top_handler, "top"},
		{going_right, top, NO_STATE, &Walker::going_right_handler, "going_right"},
		{going_left, top, NO_STATE, &Walker::going_left_handler, "going_left"},
};











class KeySampler
{
public:
	KeySampler(){};///////////
	enum Keys {K00 = 0, K01 = 1, K10 = 2, K11 = 3,NOF_KEYS};
	enum Keys keys_was;
	enum TableVal { L, R, S, N, NOF_TABLE_VAL};
	typedef TableVal TableRow[NOF_KEYS];
	KeySampler(sf::Keyboard::Key leftKey, sf::Keyboard::Key rightKey,int leftSig, int rightSig, int stopSig, int noSig):
		keys(K00), isOneKey(false), oneKey()
	{
		keymap[L] = leftSig;
		keymap[R] = rightSig;
		keymap[S] = stopSig;
		keymap[N] = noSig;
		this->leftKey = leftKey;
		this->rightKey = rightKey;
	}
	KeySampler(sf::Keyboard::Key key, int sig, int noSig)
	{
		oneKey.key = key;
		oneKey.sig = sig;
		oneKey.noSig = noSig;
		isOneKey = true;
			
	}
private:
	sf::Keyboard::Key leftKey;
	sf::Keyboard::Key rightKey;
		
	bool isOneKey;
	struct {
		sf::Keyboard::Key key;
		bool wasPressed;
		int sig;
		int noSig;
	}oneKey;
public:
	int service()
	{
		if(isOneKey)
		{
			int ret = oneKey.noSig;
			bool k = sf::Keyboard::isKeyPressed(oneKey.key);
			if(k && ! oneKey.wasPressed)
			{
				ret = oneKey.sig;
			}
			oneKey.wasPressed = k;
			return ret;
		} else {
			Keys gk = get_keys();
			TableVal tv = table[keys][gk];
			int ret = keymap[tv];
			keys = gk;
			return ret;
		}
	}
private:
	int keymap[NOF_TABLE_VAL];
private:
public:
	static const TableRow table[NOF_KEYS];
private:
	Keys keys;
	Keys get_keys()
	{
		bool k1 = sf::Keyboard::isKeyPressed(leftKey);
		bool k2 = sf::Keyboard::isKeyPressed(rightKey);
		return (enum Keys)((int)k1 << 1 | (int)k2);
	}
};



#define L KeySampler::TableVal::L
#define R KeySampler::TableVal::R
#define S KeySampler::TableVal::S
#define N KeySampler::TableVal::N

const KeySampler::TableRow KeySampler::table[KeySampler::NOF_KEYS] =
{
 // 00 01 10 11
   {N, R, L, N}, // 00
   {S, N, L, S}, // 01
   {S, R, N, S}, // 10
   {S, R, L, N}, // 11
};

#undef L
#undef R
#undef S
#undef N








class Laser :public  Thingy, Timeout_cb
{
public:
	Laser(float x, float y):itsStateManager(this, states, NUMBER_OF_STATES, top), itsTimerManager(NUMBER_OF_STATES, this), inited(false)

	{

		std::cout << x << "Laser\n" << std::endl;
		this->x = x;
		this->y = y;
		w = 2;
		h = 32;
		c = LASER;
		dx = 0;
		dy = 0;
		visible = true;
		deleteFlag = false;

		
		if(!bufferIsLoaded)
		{
//			buffer.loadFromFile("../res/skyrocket-01.wav");
			bufferIsLoaded = true;
		}
//		sound.setBuffer(buffer);


		itsTexture.loadFromFile("../res/laser.png", sf::IntRect(0, 0, 2, 32));
		itsSprite.setTexture(itsTexture);


		itsSprite.setPosition(x,y);
	};


	void on_timeout(int sig)
	{
		itsStateManager.send_synchd(TMO);
		//itsStateManager.process();

	};


	void sendMe(int sig)
	{
		if(sig < NOF_SIGNAL)
		{
			itsStateManager.send(sig);
			itsStateManager.process();
		}
	}

	virtual void update()
	{
		if(!inited)
		{
			itsStateManager.send(INIT);
			itsStateManager.process();
			inited = true;
		}
		itsTimerManager.serviceTimers();
		move();
	};

private:
	static sf::SoundBuffer buffer;
	static bool bufferIsLoaded;
	sf::Sound sound;
	sf::Texture itsTexture;
		
private:
	// state machine:

	enum State
	{
		top,
			ray,
			invisible,
			done,
	
		NUMBER_OF_STATES,
		NO_STATE,
	};

	enum Signal
	{
		ON_ENTRY,
		ON_EXIT,
		ON_INIT,
		// user signals:
		TMO,
		INIT,

		EXAMPLE,

		NOF_SIGNAL
	};

	bool top_handler(int sig)
	{
		return false;
	}
	bool ray_handler(int sig)
	{
		switch(sig)
		{
		case ON_INIT:
			visible = true;
		//	sound.play();
			itsTimerManager.addTimer(ray, 32, TMO);
			dy = -7;
			return true;
		case ON_EXIT:
			dy = 0;
			return true;
		case TMO:
			itsStateManager.set_next_state(invisible);
			return true;
		}
		return false;
	}
	bool invisible_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			itsTimerManager.addTimer(invisible, 8*32, TMO);
			visible = false;
			c = UNDEF;
			return true;
		case TMO:
			itsStateManager.set_next_state(done);
			return true;
		}
		return false;
	}
	bool done_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			deleteFlag = true;

			return true;
		}
		return false;
	}

	StateManager<Laser> itsStateManager;
	static StateManager<Laser>::StateDesc states[];

	TimerManager itsTimerManager;
	bool inited;
	
};
StateManager<Laser>::StateDesc Laser::states[] = 
{
	{top, NO_STATE, ray, &Laser::top_handler, "top"},
		{ray, top, NO_STATE, &Laser::ray_handler, "ray"},
		{invisible, top, NO_STATE, &Laser::invisible_handler, "invisible"},
		{done, top, NO_STATE, &Laser::done_handler, "done"},

};

//sf::SoundBuffer Laser::buffer;
bool Laser::bufferIsLoaded;







class FireCtrl: Timeout_cb
{
public:
	FireCtrl():itsTimerManager(NUMBER_OF_STATES, this), itsStateManager(this, states, NUMBER_OF_STATES, laser), inited(false)
	{
	   itsStateManager.send(INIT);
	   itsStateManager.process();
	
	}
	void fire(float x, float y)
	{
			this->x = x;
			this->y = y;
			itsStateManager.send(FIRE);
			itsStateManager.process();
			std::cout << x << "firectrl" << std::endl;

	}
	void on_timeout(int sig)
	{
		itsStateManager.send_synchd(sig);
		//itsStateManager.process();

	}
	void service()
	{
		itsTimerManager.serviceTimers();
	}

private:
	float x;
	float y;

private:
	enum State
	{
		top,
			laser,
				laser_ready,
				laser_inhibit,

				NUMBER_OF_STATES,
				NO_STATE,
	};
	
	enum Signal
	{
		ON_ENTRY,
		ON_EXIT,
		ON_INIT,
		// user signals:
		INIT,
		TMO,
		FIRE,
		NOF_SIGNAL
	};



	bool top_handler(int sig)
	{
		return false;
	}
	bool laser_handler(int sig)
	{
		return false;
	}
	bool laser_ready_handler(int sig)
	{
		switch(sig)
		{
		case FIRE:
			std::cout << "Fire!" << std::endl;
			Laser * pLaser = new Laser(x, y);
			ThingKeeper::enlist(pLaser);
			itsStateManager.set_next_state(laser_inhibit);
			return true;
		}
		return false;
	}
	bool laser_inhibit_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			itsTimerManager.addTimer(laser_inhibit, 250/7, TMO);
			return true;
		case TMO:
			itsStateManager.set_next_state(laser_ready);
			return true;

		}

		return false;
	}






	StateManager<FireCtrl> itsStateManager;
	static StateManager<FireCtrl>::StateDesc states[];

	TimerManager itsTimerManager;
	bool inited;

};


StateManager<FireCtrl>::StateDesc FireCtrl::states[] = 
{

	{top, NO_STATE, NO_STATE, &FireCtrl::top_handler, "top"},
			{laser, top, laser_ready, &FireCtrl::laser_handler, "laser"},
				{laser_ready, laser, NO_STATE, &FireCtrl::laser_ready_handler, "laser_ready"},
				{laser_inhibit, laser, NO_STATE, &FireCtrl::laser_inhibit_handler, "laser_inhibit"},




};




class Saucer :public  Thingy, Timeout_cb
{
public:
	Saucer():itsStateManager(this, states, NUMBER_OF_STATES, flying), itsTimerManager(NUMBER_OF_STATES, this),
		inited(false), step0(7.00), healthMeter(HealthMeter::GREEN, 50,580,100,10)

	{


		x = 200.0;
		y = 200.0;
		w = 32;
		h = 32;
		c = SAUCER;
		dx = 0;
		dy = 0;
		visible = true;
		deleteFlag = false;
		health = 100;
		old_health = 100;

		m_flyingTexture.loadFromFile("../res/saucer.png");
		m_hurtTexture.loadFromFile("../res/saucer_hurt.png");
		m_defeatedTexture.loadFromFile("../res/saucer_defeated.png");

		itsSprite.setPosition(x,y);

		keySampler_lr = KeySampler(sf::Keyboard::Left, sf::Keyboard::Right, LEFT, RIGHT, STOP_LR, NOF_SIGNAL);
		keySampler_ud = KeySampler(sf::Keyboard::Up, sf::Keyboard::Down, UP, DOWN, STOP_UD, NOF_SIGNAL);
		keySampler_space = KeySampler(sf::Keyboard::Space, FIRE, NOF_SIGNAL);
	};


	virtual void collideWith(CollisionSignature c)
	{
		std::cout << "Saucer collided with: " << c << std::endl;
		switch(c)
		{
		//UNDEF,
		//SAUCER,
		case MONSTER:
			sendMe(COLLISION_W_MONSTER);
			break;
		//LASER,
		case MONSTER_WEAK_WEPON:
			sendMe(GOT_HIT);
			break;

		}
	}


	
	void draw(sf::RenderTarget& window)
	{
		window.draw(healthMeter);
		Thingy::draw(window);
	}

	void on_timeout(int sig)
	{
		std::cout << "on_timeout" << std::endl;
		itsStateManager.send_synchd(sig);
			//itsStateManager.process();

	};


	void sendMe(int sig)
	{
		if(sig < NOF_SIGNAL)
		{
			itsStateManager.send(sig);
			itsStateManager.process();
		}
	}

	virtual void update()
	{
		if(!inited)
		{
			itsStateManager.send(INIT);
			itsStateManager.process();
			inited = true;
		}
		itsTimerManager.serviceTimers();
		itsFireCtrl.service();
		sendMe(keySampler_lr.service());
		sendMe(keySampler_ud.service());
		sendMe(keySampler_space.service());
		move();
	};

private:
	sf::Texture m_flyingTexture;
	sf::Texture m_hurtTexture;
	sf::Texture m_defeatedTexture;
	KeySampler keySampler_lr;
	KeySampler keySampler_ud;
	KeySampler keySampler_space;

	FireCtrl itsFireCtrl;

	int health;
	int old_health;
	HealthMeter 	healthMeter;

		
private:
	// state machine:

	enum State
	{
		top,
		  flying,
			  unhurt,
			  hurt,
		defeated,
		eaten,
	
		NUMBER_OF_STATES,
		NO_STATE,
	};

	enum Signal
	{
		ON_ENTRY,
		ON_EXIT,
		ON_INIT,
		// user signals:
		TMO,
		INIT,
		LEFT,
		RIGHT,
		STOP_LR,
		UP,
		DOWN,
		STOP_UD,
		FIRE,
		HEALTH_UPDATED,
		GOT_HIT,
		COLLISION_W_MONSTER,
		GOT_HURT,
		GOT_ZERO_HEALTH,



		NOF_SIGNAL
	};

	bool top_handler(int sig)
	{
		switch(sig)
		{
		case FIRE:
			itsFireCtrl.fire(x + 32/2, y - 32);
			return true;
		case HEALTH_UPDATED:
			healthMeter.setHealth(health);
			if(old_health > 50 && health <= 50){itsStateManager.send(GOT_HURT);};
			if(old_health > 0 && health <= 0){itsStateManager.send(GOT_ZERO_HEALTH);};

			old_health = health;
			return true;
		}
		return false;
	}
	bool flying_handler(int sig)
	{
		switch(sig)
		{
		case ON_EXIT:			
			dx = 0;
			dy = 0;
			return true;
		case LEFT:
			dx = (-1)*step;
			return true;
		case RIGHT:
			dx = step;
			return true;
		case STOP_LR:
			dx = 0;
			return true;

		case UP:
			dy = (-1)*step;
			return true;
		case DOWN:
			dy = step;
			return true;
		case STOP_UD:
			dy = 0;
			return true;

		case GOT_HIT:
			health -= 10;
			itsStateManager.send(HEALTH_UPDATED);
			return true;

		case COLLISION_W_MONSTER:
			itsStateManager.set_next_state(eaten);
			return true;
		case GOT_ZERO_HEALTH:
			itsStateManager.set_next_state(defeated);
			return true;

		}

		return false;
	}
	bool unhurt_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			itsSprite.setTexture(m_flyingTexture);
			step = step0;
			return true;
		case GOT_HURT:
			itsStateManager.set_next_state(hurt);
			return true;

		}
		return false;
	}
	bool hurt_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			itsSprite.setTexture(m_hurtTexture);
			step = step0/5;
			return true;
		case ON_EXIT:
			step = 0;
			return true;
		}
		return false;
	}
	bool defeated_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			itsSprite.setTexture(m_defeatedTexture);
			itsTimerManager.addTimer(defeated, 100, TMO);
			return true;
		case TMO:
			Levels::finish(Levels::FAILURE);
			return true;
		}
		return false;
	}
	bool eaten_handler(int sig)
	{
		switch(sig)
		{
		case ON_ENTRY:
			visible = false;
			health = 0;
			itsStateManager.send(HEALTH_UPDATED);
			return true;
		}
		return false;
	}

	StateManager<Saucer> itsStateManager;
	static StateManager<Saucer>::StateDesc states[];

	TimerManager itsTimerManager;
	bool inited;
	
	float step;
	const float step0;

};

StateManager<Saucer>::StateDesc Saucer::states[] = 
{

	{top, NO_STATE, NO_STATE, &Saucer::top_handler, "top"},
		{flying, top, unhurt, &Saucer::flying_handler, "flying"},
			{unhurt, flying, NO_STATE, &Saucer::unhurt_handler, "unhurt"},
			{hurt, flying, NO_STATE, &Saucer::hurt_handler, "hurt"},
		{defeated, top, NO_STATE, &Saucer::defeated_handler, "defeated"},
		{eaten, top, NO_STATE, &Saucer::eaten_handler, "eaten"},
};













#if 0
 class ALevel:public Level
 {
	 sf::Texture bkgTexture;
	 sf::Sprite bkgSprite;
 public:
	 ALevel()
	 {
		 			//sf::Texture bkgTexture;
			bkgTexture.loadFromFile("../res/bkg.png");
			//sf::Sprite bkgSprite;
			bkgSprite.setTexture(bkgTexture);

			Saucer * pSaucer = new Saucer;
			ThingKeeper::enlist(pSaucer);

			//::Saucer * pOldSaucer = new ::Saucer;
			//ThingKeeper::enlist(pOldSaucer);



			Walker * pWalker = new Walker;
			ThingKeeper::enlist(pWalker);


			Primus::Monster * pMonster = new Primus::Monster;
			ThingKeeper::enlist(pMonster);

			// TODO: 
			// rensa listan


	 }
	void draw(sf::RenderTarget& window)
	 {
		 window.draw(bkgSprite);
	}

	 ~ALevel()
	 {
		 ThingKeeper::delete_all();
	 }


 };
#endif

 class ALevel:public AaLevel
 {
 public:
	 ALevel()
	 {
		 construct("../res/bkg.png");
	 }
	 void construct_also(){
		 Saucer * pSaucer = new Saucer;
		 ThingKeeper::enlist(pSaucer);
		 Walker * pWalker = new Walker;
		 ThingKeeper::enlist(pWalker);
		 Primus::Monster * pMonster = new Primus::Monster;
		 ThingKeeper::enlist(pMonster);

	 }
 };














 class ExampleOne: Timeout_cb
{
public:
	ExampleOne():itsTimerManager(NUMBER_OF_STATES, this), itsStateManager(this, states, NUMBER_OF_STATES, top), inited(false)
	{
	   itsStateManager.send(INIT);
	   itsStateManager.process();
	
	}
	void fire()
	{
		itsStateManager.send(EXAMPLE_SIG);
		itsStateManager.process();
	}
	void on_timeout(int sig)
	{
		itsStateManager.send_synchd(sig);
		//itsStateManager.process();
	}
	void service()
	{
		itsTimerManager.serviceTimers();
	}


private:
	enum State
	{
		top,
			example_one_substate,

				NUMBER_OF_STATES,
				NO_STATE,
	};
	
	enum Signal
	{
		ON_ENTRY,
		ON_EXIT,
		ON_INIT,
		// user signals:
		INIT, // Mandatory

		EXAMPLE_SIG,

		NOF_SIGNAL
	};

	bool top_handler(int sig)
	{
		std::cout << "ExampleOne top!" << std::endl;
		return false;
	}
	bool example_one_substate_handler(int sig)
	{
		switch(sig)
		{
		case EXAMPLE_SIG:
			std::cout << "ExampleOne top!" << std::endl;
			itsStateManager.set_next_state(example_one_substate);
			return true;
		}
		return false;
	}
	StateManager<ExampleOne> itsStateManager;
	static StateManager<ExampleOne>::StateDesc states[];
	TimerManager itsTimerManager;
	bool inited;

};


StateManager<ExampleOne>::StateDesc ExampleOne::states[] = 
{
	{top, NO_STATE, example_one_substate, &ExampleOne::top_handler, "top"},
			{example_one_substate, top, NO_STATE, &ExampleOne::example_one_substate_handler, "example_one_substate"},

};

























class ExampleTwo :public  Thingy, Timeout_cb
{
public:
	ExampleTwo():itsStateManager(this, states, NUMBER_OF_STATES, top), itsTimerManager(NUMBER_OF_STATES, this), inited(false)

	{


		x = 200.0;
		y = 200.0;
		w = 32;
		h = 32;
		c = UNDEF;
		dx = 0;
		dy = 0;
		visible = true;
		deleteFlag = false;

		itsSprite.setPosition(x,y);
	};


	void on_timeout(int sig)
	{
		itsStateManager.send_synchd(TMO);
		//itsStateManager.process();

	};


	void sendMe(int sig)
	{
		if(sig < NOF_SIGNAL)
		{
			itsStateManager.send(sig);
			itsStateManager.process();
		}
	}

	virtual void update()
	{
		if(!inited)
		{
			itsStateManager.send(INIT);
			itsStateManager.process();
			inited = true;
		}
		itsTimerManager.serviceTimers();
		move();
	};

private:
		
private:
	// state machine:

	enum State
	{
		top,
		example,
	
		NUMBER_OF_STATES,
		NO_STATE,
	};

	enum Signal
	{
		ON_ENTRY,
		ON_EXIT,
		ON_INIT,
		// user signals:
		TMO,
		INIT,

		EAMPLE,

		NOF_SIGNAL
	};

	bool top_handler(int sig)
	{
		return false;
	}
	bool example_handler(int sig)
	{
		return false;
	}

	StateManager<ExampleTwo> itsStateManager;
	static StateManager<ExampleTwo>::StateDesc states[];

	TimerManager itsTimerManager;
	bool inited;
	
};



StateManager<ExampleTwo>::StateDesc ExampleTwo::states[] = 
{

	{top, NO_STATE, NO_STATE, &ExampleTwo::top_handler, "top"},
		{example, top, NO_STATE, &ExampleTwo::example_handler, "example"},
};



} /* namespace */
