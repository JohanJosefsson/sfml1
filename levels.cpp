#if 0


#include "saucergame.hpp"




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
#endif
