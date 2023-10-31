#pragma once

#include "utils.hpp"

class Channel
{
private:
	std::string					_name;
	std::string					_passKey;
	std::string					_topic;
	unsigned int				_maxUsers;

	bool						_inviteOnly;
	bool						_topicRestrict;
	bool						_isLimit;

	std::map<std::string, bool>	_clientsOp;

public:
	Channel();
	Channel(const std::string &name, const std::string &pass, const std::string &nameClient);
	~Channel();
	Channel&	operator=(const Channel &obj);

	const std::string	&getName() const;
	const std::string	&getTopic() const;
	unsigned int		getMaxUsers() const;
	bool				getInviteOnly() const;
	bool				getTopicRestrict() const;
	bool				getIsLimit() const;
	void				setClients(const std::string &name);
};
