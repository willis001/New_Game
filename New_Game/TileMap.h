#ifndef TILEMAP_H
#define TILEMAP_H

#include "Tile.h"
#include "Entity.h"

class Tile;
class Entity;

class TileMap
{
private:
	void clear();

	float gridSizeF;
	int gridSizeI;
	sf::Vector2i maxSizeWorldGrid;
	sf::Vector2f maxSizeWorldF;
	int layers;
	std::vector< std::vector< std::vector< std::vector <Tile*> > > > map;
	std::stack<Tile*> defferredRenderStack;
	std::string textureFile;
	sf::Texture tileSheet;
	sf::RectangleShape collisionBox;

	int fromX;
	int toX;
	int fromY;
	int toY;
	int layer;

public:
	TileMap(float gridSize, int width, int height, std::string texture_file);
	virtual ~TileMap();

	const sf::Texture* getTileSheet() const;
	const int getLayerSize(const int x, const int y, const int layer) const;
	const bool getMonsterCollision(Entity* entity, const float& dt);
	const bool getBulletCollision(Entity* entity, const float& dt);
	const bool getCheckPoint(Entity* entity, const float& dt);
	const bool getDamageCollision(Entity* entity, const float& dt);
	const bool getBossStageCollision(Entity* entity, const float& dt);

	void addTile(const int x, const int y, const int z, const sf::IntRect& texture_rect, const bool& collision, const short& type);
	void removeTile(const int x, const int y, const int z);
	void saveToFile(const std::string file_name);
	void loadFromFile(const std::string file_name);

	void updateCollision(Entity* entity, const float& dt);

	void update();
	void render(sf::RenderTarget& target, const sf::Vector2i& gridPosition);
	void renderDeferred(sf::RenderTarget& target);

	
};

#endif