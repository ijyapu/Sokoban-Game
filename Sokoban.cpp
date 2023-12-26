// Copyright 2023 Bikash Shrestha

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>
#include <SFML/Graphics.hpp>
#include "Sokoban.hpp"

/* This file contains the implementation of the Sokoban game logic. 
It defines the behavior of the game objects such as the player, crates, walls, and targets. 
The code includes functions for moving the player and crates, drawing the game board, and resetting the game to its initial state. 
It also includes overloaded operators for reading and writing the game board to a stream.*/

// loads the textures for the game
Sokoban::Sokoban() {
    bool texturesLoaded = true;
    texturesLoaded &= wallTexture.loadFromFile("block_06.png");
    texturesLoaded &= crateTexture.loadFromFile("crate_03.png");
    texturesLoaded &= playerTexture.loadFromFile("player_05.png");
    texturesLoaded &= floorTexture.loadFromFile("ground_01.png");
    texturesLoaded &= targetTexture.loadFromFile("ground_04.png");
    playerTextureUp.loadFromFile("player_08.png");
    playerTextureDown.loadFromFile("player_05.png");
    playerTextureLeft.loadFromFile("player_20.png");
    playerTextureRight.loadFromFile("player_17.png");

  currentPlayerTexture = &playerTextureDown;  // Default direction is down

    if (!texturesLoaded) {
        std::cerr << "Error: One or more textures failed to load." << std::endl;
        return;
    }
}

// returns the width of the game board
int Sokoban::width() const {
    if (!tiles.empty()) {
        return tiles[0].size();
    }
    return 0;
}

// returns the height of the game board
int Sokoban::height() const {
    return tiles.size();
}

// returns true if the game is won
bool Sokoban::isWon() const {
    // Check if each box is on any target position.
    bool allBoxesOnTargets = std::all_of(cratePositions.begin(), cratePositions.end(),
        [this](const sf::Vector2u& crate) {
            return std::any_of(targetPositions.begin(), targetPositions.end(),
                [&crate](const sf::Vector2u& target) {
                    return crate == target;
                });
        });

    // Check if each target position has any box on it.
    bool allTargetsHaveBoxes = std::all_of(targetPositions.begin(), targetPositions.end(),
        [this](const sf::Vector2u& target) {
            return std::any_of(cratePositions.begin(), cratePositions.end(),
                [&target](const sf::Vector2u& crate) {
                    return crate == target;
                });
        });

    // Determine the winning condition based on the number of boxes and targets.
    if (cratePositions.size() <= targetPositions.size()) {
        // If there are equal or more targets than boxes, all boxes must be on targets.
        return allBoxesOnTargets;
    } else {
        // If there are more boxes than targets, all targets must have boxes.
        return allTargetsHaveBoxes;
    }
}

// moves the player in the given direction
bool Sokoban::movePlayer(Direction d) {
    // Lambda to check if a position matches the crate's position
    auto crateAtPosition = [this](unsigned int x, unsigned int y) {
        return std::find_if(cratePositions.begin(), cratePositions.end(),
                            [x, y](const sf::Vector2u& crate) {
                                return crate.x == x && crate.y == y;
                            }) != cratePositions.end();
    };

      int dx = 0, dy = 0;
    switch (d) {
        case Up:
            dy = -1;
            currentPlayerTexture = &playerTextureUp;
            break;
        case Down:
            dy = 1;
            currentPlayerTexture = &playerTextureDown;
            break;
        case Left:
            dx = -1;
            currentPlayerTexture = &playerTextureLeft;
            break;
        case Right:
            dx = 1;
            currentPlayerTexture = &playerTextureRight;
            break;
    }

    unsigned int newX = playerPos.x + dx;
    unsigned int newY = playerPos.y + dy;


    // Bounds checking for the player
    if (newX >= width() || newY >= height() || newX < 0 || newY < 0 || tiles[newY][newX] == '#') {
        return false;
    }

    // Find the crate at the new position
    auto it = std::find_if(cratePositions.begin(), cratePositions.end(),
                           [newX, newY](const sf::Vector2u& crate) {
                               return crate.x == newX && crate.y == newY;
                           });

    if (it != cratePositions.end()) {
        // Crate found at the new position, now let's try to move it
        unsigned int newCrateX = it->x + dx;
        unsigned int newCrateY = it->y + dy;

        // Check bounds and collisions for the new crate position
        if (newCrateX >= width() || newCrateY >= height() || newCrateX < 0 || newCrateY < 0 ||
            tiles[newCrateY][newCrateX] == '#' || crateAtPosition(newCrateX, newCrateY)) {
            return false;  // Crate cannot be moved
        } else {
            // Move the crate
            it->x = newCrateX;
            it->y = newCrateY;
        }
    }

    // Move the player
    playerPos.x = newX;
    playerPos.y = newY;
    return true;
}



// draws the game board
void Sokoban::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (size_t i = 0; i < tiles.size(); ++i) {
    for (size_t j = 0; j < tiles[0].size(); ++j) {
        sf::Sprite sprite;
        // decide which texture to use based on the tile type
        switch (tiles[i][j]) {
            case '#':
                sprite.setTexture(wallTexture);
                break;
            case '.':
            default:
                sprite.setTexture(floorTexture);
                break;
        }
        sprite.setPosition(j * 64, i * 64);
        target.draw(sprite, states);
    }
}

    for (const auto& position : targetPositions) {
        sf::Sprite drawTA;
        drawTA.setPosition(position.x * 64, position.y * 64);
        drawTA.setTexture(targetTexture);
        target.draw(drawTA, states);
    }

    for (const auto& position : cratePositions) {
        sf::Sprite drawbox;
        drawbox.setPosition(position.x * 64, position.y * 64);
        drawbox.setTexture(crateTexture);
        target.draw(drawbox, states);
    }

  // draw the player with the current direction texture
  sf::Sprite playerSprite(*currentPlayerTexture);
  playerSprite.setPosition(static_cast<float>(playerPos.x * 64),
  static_cast<float>(playerPos.y * 64));
  target.draw(playerSprite, states);
}

// reads the game board from a stream
std::istream& operator>>(std::istream &is, Sokoban& board) {
    unsigned int rowCount, colCount;
    is >> rowCount >> colCount;

    board.tiles.resize(rowCount, std::vector<char>(colCount));
    std::vector<std::vector<char>> initialTilesTemp(rowCount, std::vector<char>(colCount));
    std::vector<sf::Vector2u> initialCratePositionsTemp;
    sf::Vector2u initialPlayerPosTemp;

    for (unsigned int i = 0; i < rowCount; i++) {
        for (unsigned int j = 0; j < colCount; j++) {
            char tile;
            is >> tile;
            initialTilesTemp[i][j] = tile;  // Store the initial tile layout
            board.tiles[i][j] = tile;

            switch (tile) {
                case '@':  // Player
                    initialPlayerPosTemp = sf::Vector2u(j, i);
                    board.playerPos = sf::Vector2u(j, i);
                    break;
                case 'A':  // Box
                    initialCratePositionsTemp.emplace_back(j, i);
                    board.cratePositions.emplace_back(j, i);
                    break;
                case 'a':  // Storage location
                    board.targetPositions.emplace_back(j, i);
                    break;
                case '1':  // Box on storage location
                    initialCratePositionsTemp.emplace_back(j, i);
                    board.cratePositions.emplace_back(j, i);
                    board.targetPositions.emplace_back(j, i);
                    break;
            }
        }
    }

    // After reading the entire level, set the initial state
    board.initialTiles = initialTilesTemp;
    board.initialPlayerPos = initialPlayerPosTemp;
    board.initialCratePositions = initialCratePositionsTemp;

    return is;
}

// writes the game board to a stream
std::ostream& operator<<(std::ostream &os, const Sokoban& board) {
    os << board.tiles.size() << " " << board.tiles[0].size() << "\n";
    for (const auto& row : board.tiles) {
        for (char tile : row) {
            os << tile;
        }
        os << "\n";
    }
    return os;
}

// reset the game to its initial state
void Sokoban::resetLevel() {
    playerPos = initialPlayerPos;
    cratePositions = initialCratePositions;
    tiles = initialTiles;
    currentPlayerTexture = &playerTextureDown;
}
