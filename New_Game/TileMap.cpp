#include "stdafx.h"
#include "TileMap.h"

void TileMap::clear()
{
	for (int x = 0; x < this->maxSizeWorldGrid.x; x++)
	{
		for (int y = 0; y < this->maxSizeWorldGrid.y; y++)
		{
			for (int z = 0; z < this->layers; z++)
			{
				for (int k = 0; k < this->map[x][y][z].size(); k++)
				{
					delete this->map[x][y][z][k];
					this->map[x][y][z][k] = NULL;
				}
				this->map[x][y][z].clear();
			}
			this->map[x][y].clear();
		}
		this->map[x].clear();
	}
	this->map.clear();
}

TileMap::TileMap(float gridSize, int width, int height, std::string texture_file)
{
	this->gridSizeF = gridSize;
	this->gridSizeI = static_cast<int>(this->gridSizeF);
	this->maxSizeWorldGrid.x = width;
	this->maxSizeWorldGrid.y = height;
	this->maxSizeWorldF.x = static_cast<float>(width) * gridSize;
	this->maxSizeWorldF.y = static_cast<float>(height) * gridSize;
	this->layers = 1;
	this->textureFile = texture_file;

	this->fromX = 0;
	this->toX = 0;
	this->fromY = 0;
	this->toY = 0;
	this->layer = 0;

	this->map.resize(this->maxSizeWorldGrid.x, std::vector<std::vector< std::vector<Tile*> > >());
	for (int x = 0; x < this->maxSizeWorldGrid.x; x++)
	{

		for (int y = 0; y < this->maxSizeWorldGrid.y; y++)
		{
			this->map[x].resize(this->maxSizeWorldGrid.y, std::vector<std::vector<Tile*>>());
			for (int z = 0; z < this->layers; z++)
			{
				this->map[x][y].resize(this->layers, std::vector<Tile*>());
			}
		}
	}

	if (!this->tileSheet.loadFromFile(texture_file))
		std::cout << "ERROR::TILEMAP::FAILED TO LOAD TILETEXTURESHEET::FILENAME:" << texture_file << "\n";

	this->collisionBox.setSize(sf::Vector2f(gridSize, gridSize));
	this->collisionBox.setFillColor(sf::Color(255, 0, 0, 50));
	this->collisionBox.setOutlineColor(sf::Color::Red);
	this->collisionBox.setOutlineThickness(-1.f);
}

TileMap::~TileMap()
{
	this->clear();
}

const sf::Texture* TileMap::getTileSheet() const
{
	return &this->tileSheet;
}

const int TileMap::getLayerSize(const int x, const int y, const int layer) const
{
	if (x >= 0 && x < this->map.size())
	{
		if (y >= 0 && y < this->map[x].size())
		{
			if (layer >= 0 && x < this->map[x][y].size())
			{
				return this->map[x][y][layer].size();
			}
		}
	}

	return -1;
}

const bool TileMap::getMonsterCollision(Entity* entity, const float& dt)
{
	this->layer = 0;

	this->fromX = entity->getGridPosition(this->gridSizeI).x - 1;
	if (this->fromX < 0)
		this->fromX = 0;
	else if (this->fromX > this->maxSizeWorldGrid.x)
		this->fromX = this->maxSizeWorldGrid.x;

	this->toX = entity->getGridPosition(this->gridSizeI).x + 3;
	if (this->toX < 0)
		this->toX = 0;
	else if (this->toX > this->maxSizeWorldGrid.x)
		this->toX = this->maxSizeWorldGrid.x;

	this->fromY = entity->getGridPosition(this->gridSizeI).y - 1;
	if (this->fromY < 0)
		this->fromY = 0;
	else if (this->fromY > this->maxSizeWorldGrid.y)
		this->fromY = this->maxSizeWorldGrid.y;

	this->toY = entity->getGridPosition(this->gridSizeI).y + 3;
	if (this->toY < 0)
		this->toY = 0;
	else if (this->toY > this->maxSizeWorldGrid.y)
		this->toY = this->maxSizeWorldGrid.y;

	for (int x = this->fromX; x < this->toX; x++)
	{
		for (int y = this->fromY; y < this->toY; y++)
		{
			for (int k = 0; k < this->map[x][y][this->layer].size(); k++)
			{
				sf::FloatRect bulletsBounds = entity->getGlobalBounds();
				sf::FloatRect wallBounds = this->map[x][y][this->layer][k]->getGlobalBounds();
				sf::FloatRect nextPositionBounds = entity->getNextPositionBounds(dt);
				if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::COLLISION)
				{
					//Right collision
					if (bulletsBounds.left < wallBounds.left
						&& bulletsBounds.left + bulletsBounds.width < wallBounds.left + wallBounds.width
						&& bulletsBounds.top < wallBounds.top + wallBounds.height
						&& bulletsBounds.top + bulletsBounds.height > wallBounds.top
						)
					{
						return true;
					}

					//Left collision
					else if (bulletsBounds.left > wallBounds.left
						&& bulletsBounds.left + bulletsBounds.width > wallBounds.left + wallBounds.width
						&& bulletsBounds.top < wallBounds.top + wallBounds.height
						&& bulletsBounds.top + bulletsBounds.height > wallBounds.top
						)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

const bool TileMap::getBulletCollision(Entity* entity, const float& dt)
{
	if (entity->getPosition().x < 0.f)
	{
		return true;
	}
	else if (entity->getPosition().x + entity->getGlobalBounds().width > maxSizeWorldF.x)
	{
		return true;
	}
	if (entity->getPosition().y < 0.f)
	{
		return true;
	}
	else if (entity->getPosition().y + entity->getGlobalBounds().height > maxSizeWorldF.y)
	{
		return true;
	}

	this->layer = 0;

	this->fromX = entity->getGridPosition(this->gridSizeI).x - 1;
	if (this->fromX < 0)
		this->fromX = 0;
	else if (this->fromX > this->maxSizeWorldGrid.x)
		this->fromX = this->maxSizeWorldGrid.x;

	this->toX = entity->getGridPosition(this->gridSizeI).x + 3;
	if (this->toX < 0)
		this->toX = 0;
	else if (this->toX > this->maxSizeWorldGrid.x)
		this->toX = this->maxSizeWorldGrid.x;

	this->fromY = entity->getGridPosition(this->gridSizeI).y - 1;
	if (this->fromY < 0)
		this->fromY = 0;
	else if (this->fromY > this->maxSizeWorldGrid.y)
		this->fromY = this->maxSizeWorldGrid.y;

	this->toY = entity->getGridPosition(this->gridSizeI).y + 3;
	if (this->toY < 0)
		this->toY = 0;
	else if (this->toY > this->maxSizeWorldGrid.y)
		this->toY = this->maxSizeWorldGrid.y;

	for (int x = this->fromX; x < this->toX; x++)
	{
		for (int y = this->fromY; y < this->toY; y++)
		{
			for (int k = 0; k < this->map[x][y][this->layer].size(); k++)
			{
				sf::FloatRect bulletsBounds = entity->getGlobalBounds();
				sf::FloatRect wallBounds = this->map[x][y][this->layer][k]->getGlobalBounds();
				sf::FloatRect nextPositionBounds = entity->getNextPositionBounds(dt);
				if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::COLLISION)
				{
					//Bottom collision
					if (bulletsBounds.top < wallBounds.top
						&& bulletsBounds.top + bulletsBounds.height < wallBounds.top + wallBounds.height
						&& bulletsBounds.left < wallBounds.left + wallBounds.width
						&& bulletsBounds.left + bulletsBounds.width > wallBounds.left
						)
					{
						return true;
					}

					//Top collision
					else if (bulletsBounds.top > wallBounds.top
						&& bulletsBounds.top + bulletsBounds.height > wallBounds.top + wallBounds.height
						&& bulletsBounds.left < wallBounds.left + wallBounds.width
						&& bulletsBounds.left + bulletsBounds.width > wallBounds.left
						)
					{
						return true;
					}

					//Right collision
					if (bulletsBounds.left < wallBounds.left
						&& bulletsBounds.left + bulletsBounds.width < wallBounds.left + wallBounds.width
						&& bulletsBounds.top < wallBounds.top + wallBounds.height
						&& bulletsBounds.top + bulletsBounds.height > wallBounds.top
						)
					{
						return true;
					}

					//Left collision
					else if (bulletsBounds.left > wallBounds.left
						&& bulletsBounds.left + bulletsBounds.width > wallBounds.left + wallBounds.width
						&& bulletsBounds.top < wallBounds.top + wallBounds.height
						&& bulletsBounds.top + bulletsBounds.height > wallBounds.top
						)
					{
						return true;
					}
				}
				else if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::COLLISIONTOP)
				{
					if (bulletsBounds.top < wallBounds.top
						&& bulletsBounds.top + bulletsBounds.height < wallBounds.top + wallBounds.height
						&& bulletsBounds.left < wallBounds.left + wallBounds.width
						&& bulletsBounds.left + bulletsBounds.width > wallBounds.left
						)
					{
						return true;
					}
				}
			}

		}
	}

	return false;
}

const bool TileMap::getCheckPoint(Entity* entity, const float& dt)
{
	this->layer = 0;

	this->fromX = entity->getGridPosition(this->gridSizeI).x - 1;
	if (this->fromX < 0)
		this->fromX = 0;
	else if (this->fromX > this->maxSizeWorldGrid.x)
		this->fromX = this->maxSizeWorldGrid.x;

	this->toX = entity->getGridPosition(this->gridSizeI).x + 3;
	if (this->toX < 0)
		this->toX = 0;
	else if (this->toX > this->maxSizeWorldGrid.x)
		this->toX = this->maxSizeWorldGrid.x;

	this->fromY = entity->getGridPosition(this->gridSizeI).y - 1;
	if (this->fromY < 0)
		this->fromY = 0;
	else if (this->fromY > this->maxSizeWorldGrid.y)
		this->fromY = this->maxSizeWorldGrid.y;

	this->toY = entity->getGridPosition(this->gridSizeI).y + 3;
	if (this->toY < 0)
		this->toY = 0;
	else if (this->toY > this->maxSizeWorldGrid.y)
		this->toY = this->maxSizeWorldGrid.y;

	for (int x = this->fromX; x < this->toX; x++)
	{
		for (int y = this->fromY; y < this->toY; y++)
		{
			for (int k = 0; k < this->map[x][y][this->layer].size(); k++)
			{
				sf::FloatRect bulletsBounds = entity->getGlobalBounds();
				sf::FloatRect wallBounds = this->map[x][y][this->layer][k]->getGlobalBounds();
				sf::FloatRect nextPositionBounds = entity->getNextPositionBounds(dt);
				if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::CHECKPOINT)
				{
					//Bottom collision
					if (bulletsBounds.top < wallBounds.top
						&& bulletsBounds.top + bulletsBounds.height < wallBounds.top + wallBounds.height
						&& bulletsBounds.left < wallBounds.left + wallBounds.width
						&& bulletsBounds.left + bulletsBounds.width > wallBounds.left
						)
					{
						return true;
					}

					//Top collision
					else if (bulletsBounds.top > wallBounds.top
						&& bulletsBounds.top + bulletsBounds.height > wallBounds.top + wallBounds.height
						&& bulletsBounds.left < wallBounds.left + wallBounds.width
						&& bulletsBounds.left + bulletsBounds.width > wallBounds.left
						)
					{
						return true;
					}

					//Right collision
					if (bulletsBounds.left < wallBounds.left
						&& bulletsBounds.left + bulletsBounds.width < wallBounds.left + wallBounds.width
						&& bulletsBounds.top < wallBounds.top + wallBounds.height
						&& bulletsBounds.top + bulletsBounds.height > wallBounds.top
						)
					{
						return true;
					}

					//Left collision
					else if (bulletsBounds.left > wallBounds.left
						&& bulletsBounds.left + bulletsBounds.width > wallBounds.left + wallBounds.width
						&& bulletsBounds.top < wallBounds.top + wallBounds.height
						&& bulletsBounds.top + bulletsBounds.height > wallBounds.top
						)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

const bool TileMap::getDamageCollision(Entity* entity, const float& dt)
{
	this->layer = 0;

	this->fromX = entity->getGridPosition(this->gridSizeI).x - 1;
	if (this->fromX < 0)
		this->fromX = 0;
	else if (this->fromX > this->maxSizeWorldGrid.x)
		this->fromX = this->maxSizeWorldGrid.x;

	this->toX = entity->getGridPosition(this->gridSizeI).x + 3;
	if (this->toX < 0)
		this->toX = 0;
	else if (this->toX > this->maxSizeWorldGrid.x)
		this->toX = this->maxSizeWorldGrid.x;

	this->fromY = entity->getGridPosition(this->gridSizeI).y - 1;
	if (this->fromY < 0)
		this->fromY = 0;
	else if (this->fromY > this->maxSizeWorldGrid.y)
		this->fromY = this->maxSizeWorldGrid.y;

	this->toY = entity->getGridPosition(this->gridSizeI).y + 3;
	if (this->toY < 0)
		this->toY = 0;
	else if (this->toY > this->maxSizeWorldGrid.y)
		this->toY = this->maxSizeWorldGrid.y;

	for (int x = this->fromX; x < this->toX; x++)
	{
		for (int y = this->fromY; y < this->toY; y++)
		{
			for (int k = 0; k < this->map[x][y][this->layer].size(); k++)
			{
				sf::FloatRect playerBounds = entity->getGlobalBounds();
				sf::FloatRect wallBounds = this->map[x][y][this->layer][k]->getGlobalBounds();
				sf::FloatRect nextPositionBounds = entity->getNextPositionBounds(dt);
				if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::DAMAGEHALFBOT)
				{
					//Bottom collision
					if (playerBounds.top < wallBounds.top
						&& playerBounds.top + playerBounds.height < wallBounds.top + wallBounds.height
						&& playerBounds.left < wallBounds.left + wallBounds.width
						&& playerBounds.left + playerBounds.width > wallBounds.left
						)
					{
						return true;
					}

					//Top collision
					else if (playerBounds.top > wallBounds.top
						&& playerBounds.top + playerBounds.height > wallBounds.top + wallBounds.height
						&& playerBounds.left < wallBounds.left + wallBounds.width
						&& playerBounds.left + playerBounds.width > wallBounds.left
						)
					{
						return true;
					}

					//Right collision
					if (playerBounds.left < wallBounds.left
						&& playerBounds.left + playerBounds.width < wallBounds.left + wallBounds.width
						&& playerBounds.top < wallBounds.top + wallBounds.height
						&& playerBounds.top + playerBounds.height > wallBounds.top
						)
					{
						return true;
					}

					//Left collision
					else if (playerBounds.left > wallBounds.left
						&& playerBounds.left + playerBounds.width > wallBounds.left + wallBounds.width
						&& playerBounds.top < wallBounds.top + wallBounds.height
						&& playerBounds.top + playerBounds.height > wallBounds.top
						)
					{
						return true;
					}
				}
				else if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::DAMAGEHALFTOPRIGHT)
				{
					if (playerBounds.left < wallBounds.left + wallBounds.width / 2.f
						&& playerBounds.left + playerBounds.width / 2.f < (wallBounds.left + wallBounds.width)
						&& playerBounds.top < (wallBounds.top + wallBounds.height)
						&& playerBounds.top + playerBounds.height > wallBounds.top
						)
					{
						return true;
					}
				}
				else if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::DAMAGEHALFTOPLEFT)
				{
					if (playerBounds.left + playerBounds.width > wallBounds.left + wallBounds.width / 2.f
						&& playerBounds.top < wallBounds.top + wallBounds.height
						&& playerBounds.top + playerBounds.height > wallBounds.top
						)
					{
						return true;
					}
				}
			}

		}
	}

	return false;
}

void TileMap::addTile(const int x, const int y, const int z, const sf::IntRect& texture_rect, const bool& collision, const short& type)
{
	if (x < this->maxSizeWorldGrid.x && x >= 0 &&
		y < this->maxSizeWorldGrid.y && y >= 0 &&
		z < this->layers && z >= 0)
	{
			this->map[x][y][z].push_back(new Tile(x, y, this->gridSizeF, this->tileSheet, texture_rect, collision, type));
			std::cout << "DEBUG: ADDED TILE!" << "\n";
	}
}

void TileMap::removeTile(const int x, const int y, const int z)
{
	if (x < this->maxSizeWorldGrid.x && x >= 0 &&
		y < this->maxSizeWorldGrid.y && y >= 0 &&
		z < this->layers && z >= 0)
	{
		if (!this->map[x][y][z].empty())
		{
			delete this->map[x][y][z][this->map[x][y][z].size()-1];
			this->map[x][y][z].pop_back();
			std::cout << "DEBUG: REMOVED TILE!" << "\n";
		}
	}
}

void TileMap::saveToFile(const std::string file_name)
{
	/*Saves the entire tilemap to a text-file
	Formal:
	Basic:
	Size x y
	gridSize
	layers
	texture file
	
	All tiles:
	gridPos x y layer, Texture rect x y, collision, type
	*/

	std::ofstream out_file;

	out_file.open(file_name);

	if (out_file.is_open())
	{
		out_file << this->maxSizeWorldGrid.x << " " << this->maxSizeWorldGrid.y << "\n"
			<< this->gridSizeI << "\n"
			<< this->layers << "\n"
			<< this->textureFile << "\n";

		for (int x = 0; x < this->maxSizeWorldGrid.x; x++)
		{
			for (int y = 0; y < this->maxSizeWorldGrid.y; y++)
			{
				for (int z = 0; z < this->layers; z++)
				{
					if(!this->map[x][y][z].empty())
						for (int k = 0; k < this->map[x][y][z].size(); k++)
						{
							out_file << x << " " << y << " " << z << " " << this->map[x][y][z][k]->getAsString() << " ";
						}
				}
			}
		}
	}
	else
	{
		std::cout << "ERROR::TILEMAP::COULD NOT SAVE TO FILE::FILENAME:" << file_name << "\n";
	}
}

void TileMap::loadFromFile(const std::string file_name)
{
	/*Loads the entire tilemap to a text-file
	Formal:
	Basic:
	Size x y
	gridSize
	layers
	texture file

	All tiles:
	gridPos x y, Texture rect x y, collision, type
	*/

	std::ifstream in_file;

	in_file.open(file_name);

	if (in_file.is_open())
	{
		sf::Vector2i size;
		int gridSize = 0;
		int layers = 0;
		std::string texture_file = "";
		int x = 0;
		int y = 0;
		int z = 0;
		int trX = 0;
		int trY = 0;
		bool collision = false;
		short type = 0;

		//Basics
		in_file >> size.x >> size.y >> gridSize >> layers >> texture_file;
		
		//Tiles
		this->gridSizeF = static_cast<float>(gridSize);
		this->gridSizeI = gridSize;
		this->maxSizeWorldGrid.x = size.x;
		this->maxSizeWorldGrid.y = size.y;
		this->layers = layers;
		this->textureFile = texture_file;

		this->clear();

		this->map.resize(this->maxSizeWorldGrid.x, std::vector<std::vector< std::vector<Tile*> > >());
		for (int x = 0; x < this->maxSizeWorldGrid.x; x++)
		{
			for (int y = 0; y < this->maxSizeWorldGrid.y; y++)
			{
				this->map[x].resize(this->maxSizeWorldGrid.y, std::vector<std::vector<Tile*>>());

				for (int z = 0; z < this->layers; z++)
				{
					this->map[x][y].resize(this->layers, std::vector<Tile*>());
				}
			}
		}

		if (!this->tileSheet.loadFromFile(texture_file))
			std::cout << "ERROR::TILEMAP::FAILED TO LOAD TILETEXTURESHEET::FILENAME:" << texture_file << "\n";

		//Load all tiles
		while (in_file >> x >> y >> z >> trX >> trY >> collision >> type)
		{
			this->map[x][y][z].push_back(
				new Tile(
					x, y,
					this->gridSizeF,
					this->tileSheet,
					sf::IntRect(trX, trY, this->gridSizeI, this->gridSizeI),
					collision,
					type
				)
			);
		}
	}
	else
	{
		std::cout << "ERROR::TILEMAP::COULD NOT LOAD FORM FILE::FILENAME:" << file_name << "\n";
	}

	in_file.close();
}

void TileMap::updateCollision(Entity* entity, const float& dt)
{
	//WORLD BOUNDS
	if (entity->getPosition().x < 0.f)
	{
		entity->setPosition(0.f, entity->getPosition().y);
		entity->stopVelocityX();
	}
	else if (entity->getPosition().x + entity->getGlobalBounds().width > maxSizeWorldF.x)
	{
		entity->setPosition(maxSizeWorldF.x - entity->getGlobalBounds().width, entity->getPosition().y);
		entity->stopVelocityX();
	}
	if (entity->getPosition().y < 0.f)
	{
		entity->setPosition(entity->getPosition().x, 0.f);
		entity->stopVelocityY();
	}
	else if (entity->getPosition().y + entity->getGlobalBounds().height > maxSizeWorldF.y)
	{
		entity->setPosition(entity->getPosition().x, maxSizeWorldF.y - entity->getGlobalBounds().height);
		entity->stopVelocityY();
	}

	//TILES
	this->layer = 0;

	this->fromX = entity->getGridPosition(this->gridSizeI).x - 1;
	if (this->fromX < 0)
		this->fromX = 0;
	else if (this->fromX > this->maxSizeWorldGrid.x)
		this->fromX = this->maxSizeWorldGrid.x;

	this->toX = entity->getGridPosition(this->gridSizeI).x + 3;
	if (this->toX < 0)
		this->toX = 0;
	else if (this->toX > this->maxSizeWorldGrid.x)
		this->toX = this->maxSizeWorldGrid.x;

	this->fromY = entity->getGridPosition(this->gridSizeI).y - 1;
	if (this->fromY < 0)
		this->fromY = 0;
	else if (this->fromY > this->maxSizeWorldGrid.y)
		this->fromY = this->maxSizeWorldGrid.y;

	this->toY = entity->getGridPosition(this->gridSizeI).y + 3;
	if (this->toY < 0)
		this->toY = 0;
	else if (this->toY > this->maxSizeWorldGrid.y)
		this->toY = this->maxSizeWorldGrid.y;

	for (int x = this->fromX; x < this->toX; x++)
	{
		for (int y = this->fromY; y < this->toY; y++)
		{
			for (int k = 0; k < this->map[x][y][this->layer].size(); k++)
			{
				sf::FloatRect playerBounds = entity->getGlobalBounds();
				sf::FloatRect wallBounds = this->map[x][y][this->layer][k]->getGlobalBounds();
				sf::FloatRect nextPositionBounds = entity->getNextPositionBounds(dt);
				if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::COLLISION)
				{
					//Bottom collision
					if (playerBounds.top < wallBounds.top
						&& playerBounds.top + playerBounds.height < wallBounds.top + wallBounds.height
						&& playerBounds.left < wallBounds.left + wallBounds.width
						&& playerBounds.left + playerBounds.width > wallBounds.left
						)
					{
						//std::cout << "On Top!!" << "\n";
						entity->resetJumpCount();
						entity->stopVelocityY();
						entity->setPosition(playerBounds.left, wallBounds.top - playerBounds.height);
					}

					//Top collision
					else if (playerBounds.top > wallBounds.top
						&& playerBounds.top + playerBounds.height > wallBounds.top + wallBounds.height
						&& playerBounds.left < wallBounds.left + wallBounds.width
						&& playerBounds.left + playerBounds.width > wallBounds.left
						)
					{
						//std::cout << "Under Bottom!!" << "\n";
						entity->stopVelocityY();
						entity->setPosition(playerBounds.left, wallBounds.top + wallBounds.height);
					}

					//Right collision
					if (playerBounds.left < wallBounds.left
						&& playerBounds.left + playerBounds.width < wallBounds.left + wallBounds.width
						&& playerBounds.top < wallBounds.top + wallBounds.height
						&& playerBounds.top + playerBounds.height > wallBounds.top
						)
					{
						//std::cout << "On Left!!" << "\n";
						entity->stopVelocityX();
						entity->setPosition(wallBounds.left - playerBounds.width - 1.f, playerBounds.top);
					}

					//Left collision
					else if (playerBounds.left > wallBounds.left
						&& playerBounds.left + playerBounds.width > wallBounds.left + wallBounds.width
						&& playerBounds.top < wallBounds.top + wallBounds.height
						&& playerBounds.top + playerBounds.height > wallBounds.top
						)
					{
						//std::cout << "On Right!!" << "\n";
						entity->stopVelocityX();
						entity->setPosition(wallBounds.left + wallBounds.width + 1.f, playerBounds.top);
					}
				}
				else if (this->map[x][y][this->layer][k]->getCollision() &&
					this->map[x][y][this->layer][k]->intersects(nextPositionBounds) &&
					this->map[x][y][this->layer][k]->getType() == TileTypes::COLLISIONTOP)
				{	
					if (playerBounds.top < wallBounds.top
						&& playerBounds.top + playerBounds.height < wallBounds.top + wallBounds.height
						&& playerBounds.left < wallBounds.left + wallBounds.width
						&& playerBounds.left + playerBounds.width > wallBounds.left
						)
					{
						//std::cout << "On Top!!" << "\n";
						entity->resetJumpCount();
						entity->stopVelocityY();
						entity->setPosition(playerBounds.left, wallBounds.top - playerBounds.height);
					}
				}
			}

		}
	}
}

void TileMap::update()
{

}

void TileMap::render(sf::RenderTarget& target, const sf::Vector2i& gridPosition)
{
	this->layer = 0;

	this->fromX = gridPosition.x - 40;
	if (this->fromX < 0)
		this->fromX = 0;
	else if (this->fromX > this->maxSizeWorldGrid.x)
		this->fromX = this->maxSizeWorldGrid.x;

	this->toX = gridPosition.x + 41;
	if (this->toX < 0)
		this->toX = 0;
	else if (this->toX > this->maxSizeWorldGrid.x)
		this->toX = this->maxSizeWorldGrid.x;

	this->fromY = gridPosition.y - 30;
	if (this->fromY < 0)
		this->fromY = 0;
	else if (this->fromY > this->maxSizeWorldGrid.y)
		this->fromY = this->maxSizeWorldGrid.y;

	this->toY = gridPosition.y + 31;
	if (this->toY < 0)
		this->toY = 0;
	else if (this->toY > this->maxSizeWorldGrid.y)
		this->toY = this->maxSizeWorldGrid.y;

	for (int x = this->fromX; x < this->toX; x++)
	{
		for (int y = this->fromY; y < this->toY; y++)
		{
			for (int k = 0; k < this->map[x][y][this->layer].size(); k++)
			{
				if (this->map[x][y][this->layer][k]->getType() == TileTypes::INFRONT)
				{
					this->defferredRenderStack.push(this->map[x][y][this->layer][k]);
				}
				else
				{
					this->map[x][y][this->layer][k]->render(target);
				}
				
				if (this->map[x][y][this->layer][k]->getCollision())
				{
					this->collisionBox.setPosition(this->map[x][y][this->layer][k]->getPosition());
					target.draw(this->collisionBox);
				}
			}
		}
	}
}

void TileMap::renderDeferred(sf::RenderTarget& target)
{
	while (!this->defferredRenderStack.empty())
	{
		defferredRenderStack.top()->render(target);
		defferredRenderStack.pop();
	}
}



