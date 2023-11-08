#pragma once

#include "utils.hpp"

class Server;

class Channel
{
private:
	std::string					_name;
	std::string					_passKey;
	std::string					_topic;
	unsigned int				_maxUsers;

	bool						_inviteOnly;
	bool						_topicRestrict;
	int							_clientsLimit;

	std::map<std::string, Client*>	_clients;
	std::map<std::string, Client*>	_clientsOp;

public:
	Channel();
	Channel(const std::string &name, const std::string &pass, Client *creator);
	~Channel();
	Channel&	operator=(const Channel &obj);

	const std::string					&getName() const;
	const std::string					&getTopic() const;
	const std::string					&getPasskey() const;
	unsigned int						getMaxUsers() const;
	bool								getInviteOnly() const;
	bool								getTopicRestrict() const;
	bool								getIsLimit() const;
	void								setClients(Client *client);
	void								setTopic(const std::string &newTopic);
	void								sendToAll(const std::string &msg) const;

	std::vector<Client *>				getAllClients() const;
	std::map<std::string, Client *>		getClientsOp() const;
	Client*								findClient(const std::string &cl);
	bool								isOperator(const std::string &cl);
	void								deleteClientFromChannel(const std::string &cl);
	void								updateNickInChannel(const std::string &oldName, const std::string &newName);
};
