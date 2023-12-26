// Copyright 2023 Bikash Shrestha

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/* This file defines a class Sokoban which represents a Sokoban game. 
The class contains methods to move the player in different directions, reset the level, check if the game is won, and overload the input and output stream operators. 
The class also contains private member variables to store the textures of the game objects, positions of the player, crates, and targets, and the tiles of the game. 
The class inherits from sf::Drawable and overrides the draw method to draw the game objects on the screen.*/

enum Direction {
  Up,
  Down,
  Left,
  Right
};


class Sokoban : public sf::Drawable {
 public:
  Sokoban();

  int width() const;
  int height() const;

  bool isWon() const;
  bool movePlayer(Direction d);

  void resetLevel();

  friend std::istream& operator>>(std::istream& is, Sokoban& game);
  friend std::ostream& operator<<(std::ostream& os, const Sokoban& game);

 private:
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  sf::Texture playerTexture;
  sf::Texture crateTexture;
  sf::Texture targetTexture;
  sf::Texture floorTexture;
  sf::Texture wallTexture;
  sf::Texture playerTextureUp;
  sf::Texture playerTextureDown;
  sf::Texture playerTextureLeft;
  sf::Texture playerTextureRight;

  sf::Texture* currentPlayerTexture;
  sf::Vector2u playerPos;
  std::vector<sf::Vector2u> cratePositions;
  std::vector<sf::Vector2u> targetPositions;

  sf::Vector2u initialPlayerPos;
  std::vector<sf::Vector2u> initialCratePositions;
  std::vector<std::vector<char>> initialTiles;

  std::vector<std::vector<char>> tiles;
};
