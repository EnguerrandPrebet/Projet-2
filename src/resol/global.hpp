#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "option.hpp"

#include <ostream>

#define ALWAYS 0

/* ! Ne pas abuser de global.hpp, seuls les paramètres et les options utilisées dans tout le programme doivent être ici ! */

namespace Global // pas de using namespace sur Global
{
	extern Option option;

	std::ostream& DEBUG();
	std::ostream& DEBUG(unsigned int required);
	std::ostream& WARNING();
	std::ostream& ERROR();
}

#endif // GLOBAL_HPP
