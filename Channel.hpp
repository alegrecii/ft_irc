#pragma once

#include "utils.hpp"

class Server;

class Channel
{
private:
	std::string						_name;
	std::string						_passKey;
	std::string						_topic;
	std::string						_creationTime;

	bool							_inviteOnly;
	bool							_topicRestrict;
	int								_clientsLimit;

	std::vector<Client *>			_invitedClients;
	std::map<std::string, Client*>	_clients;
	std::map<std::string, Client*>	_clientsOp;

public:
	Channel();
	Channel(const std::string &name, Client *creator);
	~Channel();
	Channel&	operator=(const Channel &obj);

	const std::string					&getName() const;
	const std::string					&getTopic() const;
	const std::string					&getPasskey() const;
	unsigned int						getMaxUsers() const;
	bool								getInviteOnly() const;
	bool								getTopicRestrict() const;
	int									getLimit() const;
	void								inviteHere(Client *client);
	bool								isInvited(Client *client) const;
	void								removeFromInvited(Client *client);
	void								setInviteOnly(bool plus, Client &client);
	void								setClients(Client *client);
	void								setTopicRestrict(bool plus, Client &client);
	void								setTopic(const std::string &newTopic);
	void								setPass(bool plus, Client &client, std::string pass);
	void								setOperator(bool plus, Client &client, std::string nick);
	void								setLimit(Client &client, std::string n);
	void								printModes(Client &client) const;
	void								sendToAll(const std::string &msg) const;
	unsigned int						getSize() const;
	std::vector<Client *>				getAllClients() const;
	std::map<std::string, Client *>		getClientsOp() const;
	Client*								findClient(const std::string &cl) const;
	bool								isOperator(const std::string &cl);
	void								deleteClientFromChannel(const std::string &cl);
	void								updateNickInChannel(const std::string &oldName, const std::string &newName);
};
