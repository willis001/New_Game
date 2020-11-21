#ifndef MOVEMENTCOMPONENT_H
#define MOVEMENTCOMPONENT_H

#include<iostream>
#include<ctime>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<vector>
#include<stack>
#include<map>

#include<SFML/System.hpp>
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Network.hpp>


class MovementComponent
{
private:
	sf::Sprite& sprite;
	
	float maxVelocity;

	sf::Vector2f velocity;
	sf::Vector2f acceleration;
	sf::Vector2f deacceleration;

public:
	MovementComponent(sf::Sprite& sprite, float maxVelocity);
	virtual ~MovementComponent();

	const sf::Vector2f& getVelocity() const;

	void move(const float dir_x, const float dir_y, const float& dt);
	void update(const float& dt);
};

#endif
