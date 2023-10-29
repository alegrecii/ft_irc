#pragma once

#include "utils.hpp"

class Channel
{
private:
	Channel();
	std::string					_name;
	std::string					_passKey;
	std::string					_topic;
	unsigned int				_maxUsers;

	bool						_inviteOnly;
	bool						_topicRestrict;
	bool						_isLimit;

	std::vector<Client>			_clients;
	std::vector<std::string>	_operators;
	std::vector<std::string>	_banList;

public:
	Channel(const std::string &name, const Client &creator);
	~Channel();
	Channel&	operator=(const Channel &obj);

	const std::string	&getName() const;
	const std::string	&getTopic() const;
	unsigned int		getMaxUsers() const;
	bool				getInviteOnly() const;
	bool				getTopicRestrict() const;
	bool				getIsLimit() const;
};