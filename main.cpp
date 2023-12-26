// Copyright 2023 Bikash Shrestha

#include <iostream>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Sokoban.hpp"


int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <level>\n";
        return 1;
    }

    Sokoban game;  // create a Sokoban game object
    std::string filename = argv[1];  // get the filename from the command line arguments

    std::ifstream file(filename);  // open the file
    if (!file) {
        std::cerr << "Unable to open file: " << filename << '\n';
        return 1;
    }

    file >> game;
    file.close();

    const int tileSize = 64;
    const int windowSizeX = game.width() * tileSize;
    const int windowSizeY = game.height() * tileSize;
    sf::RenderWindow window(sf::VideoMode(windowSizeX, windowSizeY), "Sokoban");

    sf::Clock clock;  // create a clock to keep track of elapsed time
    sf::Time timeTaken;  // To keep track of the time taken to solve the game
    bool gameWon = false;  // To keep track of the game state

    sf::SoundBuffer buffer;  // To load the victory fanfare sound effect
    if (!buffer.loadFromFile("champion.wav")) {
        std::cerr << "Unable to load victory fanfare sound effect!\n";
        return 1;
    }

    sf::Sound victorySound;  // To play the victory fanfare sound effect
    victorySound.setBuffer(buffer);
    bool soundPlayed = false;  // To keep track of whether the sound has been played

    sf::Font font;
    // create a font to render the elapsed time
    if (!font.loadFromFile("font.ttf")) {
        std::cerr << "Unable to load font!\n";
        return 1;
    }

    // create a text to display when the game is won
    sf::Text winText;
    winText.setFont(font);
    winText.setString("Congratulations! You've won!");
    winText.setCharacterSize(30);
    winText.setFillColor(sf::Color::Green);
    winText.setPosition(windowSizeX / 2 - winText.getGlobalBounds().width / 2,
    windowSizeY / 2 - winText.getGlobalBounds().height);

    sf::Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(20);
    timeText.setFillColor(sf::Color::Black);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    game.resetLevel();  // Reset the game state
                    clock.restart();   // Restart the clock
                    gameWon = false;   // Reset the game won state
                    timeTaken = sf::Time::Zero;  // Reset the time taken
                    soundPlayed = false;  // Reset the sound played flag
                    victorySound.stop();  // Stop the victory sound if it's playing
                } else if (!gameWon) {  // Handle other keypresses only if the game is not won
                    Direction dir;
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                        case sf::Keyboard::Up:
                            dir = Up;
                            break;
                        case sf::Keyboard::S:
                        case sf::Keyboard::Down:
                            dir = Down;
                            break;
                        case sf::Keyboard::A:
                        case sf::Keyboard::Left:
                            dir = Left;
                            break;
                        case sf::Keyboard::D:
                        case sf::Keyboard::Right:
                            dir = Right;
                            break;
                        default:
                            continue;
                    }
                    game.movePlayer(dir);
                    if (game.isWon()) {
                        gameWon = true;
                        timeTaken = clock.getElapsedTime();
                    }
                }
            }
        }

        window.clear();
        window.draw(game);  // Draw the game

        if (gameWon) {
            if (!soundPlayed) {
                victorySound.play();  // Play the victory fanfare
                soundPlayed = true;
            }
            window.draw(winText);  // Draw the win text if the game is won

            // Display the time taken to solve the game
            std::stringstream ss;
            ss << "Solved in: " << static_cast<int>(timeTaken.asSeconds()) << " seconds";
            timeText.setString(ss.str());
            timeText.setPosition(windowSizeX / 2 - timeText.getGlobalBounds().width / 2,
            winText.getPosition().y + winText.getGlobalBounds().height + 10);
            window.draw(timeText);
        } else {
            // Update and display the ongoing time only if the game hasn't been won
            int totalElapsedTime = static_cast<int>(clock.getElapsedTime().asSeconds());
            int minutes = totalElapsedTime / 60;
            int seconds = totalElapsedTime % 60;
            std::stringstream ss;
            ss << "Elapsed Time: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
            timeText.setString(ss.str());
            timeText.setPosition(windowSizeX - timeText.getGlobalBounds().width - 10, 10);
            window.draw(timeText);
        }

        window.display();
    }

    return 0;
}
