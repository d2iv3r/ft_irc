#include "../Includes/command.hpp"

Command::Replies::Replies(std::string rep) : _rep(rep) {
}

const char *Command::Replies::what() const throw()
{
	return(this->_rep.c_str());
}

Command::Replies::~Replies() throw() {}
