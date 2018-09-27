/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: DVD
 *
 * Created on 24 сентября 2018 г., 17:11
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

void initWindow(sf::RenderWindow&, std::vector<std::vector<sf::Vertex>>, int, int, int);
void incrementTimeString(std::string&);
sf::Color getPlayerColor(int);
std::vector<std::vector<sf::Vertex>> generateRoad(int, int, int);
sf::Vector2f closestGridIntersection(int, int, int);
bool mouseInRange(int, int, int, int, int);
void drawPath(sf::RenderWindow&, std::vector<sf::Vertex>);
void drawArrow(sf::RenderWindow&, sf::Color, int, int);
void drawMarkers(sf::RenderWindow&, sf::Vertex, sf::Vertex, int, int, int);
std::vector<bool> checkWinningConditions(std::vector<std::vector<sf::Vertex>>);

int main()
{
    srand(time(0));
    rand();
    int rows = 45, cols = 40, distance = 15;
    int currentPlayer = 0, numberOfPlayers = 2;
    int winStatus = 0;
    sf::RenderWindow window(sf::VideoMode(cols * distance, rows * distance), "Race! (LMB to move, RMB to skip)");
    sf::Font font;
    font.loadFromFile("resources/StudioScriptC.otf");
    std::string timeString = "0";
    sf::Text time;
    time.setFont(font);
    time.setString("Time: " + timeString);
    time.setCharacterSize(30);
    time.setPosition(20, 0);
    time.setFillColor(sf::Color::Black);
    sf::Text win;
    win.setFont(font);
    win.setCharacterSize(30);
    win.setPosition(20, rows * distance - 75);
    win.setFillColor(sf::Color::Black);
    std::vector<std::vector<sf::Vertex>> road = generateRoad(rows, cols, distance);
    std::vector<std::vector<sf::Vertex>> path(numberOfPlayers);
    initWindow(window, road, rows, cols, distance);
    window.draw(time);
    window.display();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            
            if (event.type == sf::Event::GainedFocus)
            {
                initWindow(window, road, rows, cols, distance);
                for (int i = 0; i < numberOfPlayers; i++) 
                {
                    drawPath(window, path[i]);
                }
                window.draw(time);
                if (winStatus)
                {
                    window.draw(win);
                }
                window.display();
            }
            
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Key::R)
                {
                    winStatus = 0;
                    currentPlayer = 0;
                    timeString = "0";
                    time.setString("Time: " + timeString);
                    win.setPosition(20, rows * distance - 75);
                    road = generateRoad(rows, cols, distance);
                    std::vector<std::vector<sf::Vertex>> newPath(numberOfPlayers);
                    path = newPath;
                    initWindow(window, road, rows, cols, distance);
                    window.draw(time);
                    window.display();
                }
            }
            
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !winStatus)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
                {
                    break;
                }
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                if (path[currentPlayer].size() >= 1)
                {
                    initWindow(window, road, rows, cols, distance);
                    std::vector<sf::Vertex> tempPath(2);
                    tempPath[0] = path[currentPlayer][path[currentPlayer].size() - 1];
                    tempPath[1] = sf::Vertex(closestGridIntersection(mousePosition.x, mousePosition.y, distance));
                    sf::Vector2f expectedPoint = sf::Vector2f
                    (
                        2 * path[currentPlayer][path[currentPlayer].size() - 1].position.x - path[currentPlayer][path[currentPlayer].size() - 2 + (path[currentPlayer].size() == 1)].position.x,
                        2 * path[currentPlayer][path[currentPlayer].size() - 1].position.y - path[currentPlayer][path[currentPlayer].size() - 2 + (path[currentPlayer].size() == 1)].position.y
                    );
                    if (mouseInRange(expectedPoint.x, expectedPoint.y, tempPath[1].position.x, tempPath[1].position.y, distance))
                    {
                        tempPath[0].color = tempPath[1].color = getPlayerColor(currentPlayer);
                    }
                    else
                    {
                        tempPath[0].color = tempPath[1].color = sf::Color(128, 128, 128);
                    }
                    if (expectedPoint.y < 0)
                    {
                        tempPath[1].position = expectedPoint; 
                        tempPath[0].color = tempPath[1].color = getPlayerColor(currentPlayer);
                        
                    }
                    window.draw(&tempPath[0], 2, sf::Lines);
                    for (int i = 0; i < numberOfPlayers; i++) 
                    {
                        drawPath(window, path[i]);
                    }
                    if (expectedPoint.y >= 0)
                    {
                        int radius = 2;
                        sf::CircleShape marker(radius);
                        sf::Vector2f closest = closestGridIntersection(mousePosition.x, mousePosition.y, distance);
                        marker.setPosition(closest.x - radius, closest.y - radius);
                        marker.setFillColor(sf::Color(128, 128, 128));
                        window.draw(marker);
                        drawMarkers
                        (
                            window,
                            path[currentPlayer][path[currentPlayer].size() - 2 + (path[currentPlayer].size() == 1)],
                            path[currentPlayer][path[currentPlayer].size() - 1],
                            distance,
                            mousePosition.x,
                            mousePosition.y
                        );
                    }
                    else
                    {
                        drawArrow(window, getPlayerColor(currentPlayer), mousePosition.x, mousePosition.y);
                    }
                    window.draw(time);
                    window.display();
                }
                else
                {
                    int radius = 3;
                    sf::CircleShape marker(radius);
                    sf::Vector2f closest = closestGridIntersection(mousePosition.x, mousePosition.y, distance);
                    marker.setPosition(closest.x - radius, closest.y - radius);
                    marker.setFillColor(getPlayerColor(currentPlayer));
                    initWindow(window, road, rows, cols, distance);
                    for (int i = 0; i < numberOfPlayers; i++) 
                    {
                        drawPath(window, path[i]);
                    }
                    window.draw(marker);
                    window.draw(time);
                    window.display();
                }
            }
            
            if (event.type == sf::Event::MouseButtonReleased && !winStatus)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Color color = getPlayerColor(currentPlayer);
                    sf::Vertex newPoint = sf::Vertex(closestGridIntersection(event.mouseButton.x, event.mouseButton.y, distance), color);
                    if (path[currentPlayer].size() >= 1)
                    {
                        sf::Vector2f expectedPoint;
                        if (path[currentPlayer].size() == 1)
                        {
                            expectedPoint = sf::Vector2f(path[currentPlayer][0].position.x, path[currentPlayer][0].position.y);
                        }
                        else
                        {
                            expectedPoint = sf::Vector2f
                            (
                                2 * path[currentPlayer][path[currentPlayer].size() - 1].position.x - path[currentPlayer][path[currentPlayer].size() - 2].position.x,
                                2 * path[currentPlayer][path[currentPlayer].size() - 1].position.y - path[currentPlayer][path[currentPlayer].size() - 2].position.y
                            );
                        }
                        if (expectedPoint.y < 0)
                        {
                            path[currentPlayer].push_back(sf::Vertex(expectedPoint, path[currentPlayer][0].color));
                            currentPlayer = (currentPlayer + 1) % numberOfPlayers;
                            if (currentPlayer == 0)
                            {
                                incrementTimeString(timeString);
                                time.setString("Time: " + timeString);
                            }
                        }
                        else
                        if (mouseInRange(expectedPoint.x, expectedPoint.y, newPoint.position.x, newPoint.position.y, distance))
                        {
                            path[currentPlayer].push_back(newPoint);
                            currentPlayer = (currentPlayer + 1) % numberOfPlayers;
                            if (currentPlayer == 0)
                            {
                                incrementTimeString(timeString);
                                time.setString("Time: " + timeString);
                            }
                        }
                    }
                    else
                    {
                        path[currentPlayer].push_back(newPoint);
                        currentPlayer = (currentPlayer + 1) % numberOfPlayers;
                        if (currentPlayer == 0)
                        {
                            incrementTimeString(timeString);
                            time.setString("Time: " + timeString);
                        }
                    }
                    std::vector<bool> winners = checkWinningConditions(path);
                    if (!currentPlayer)
                    {
                        std::string firstWinner;
                        for (int i = 0; i < winners.size(); i++)
                        {
                            if (winners[i])
                            {
                                std::string winnerNumber = "0";
                                if (winStatus == 1)
                                {
                                    win.setString("Winning players: " + firstWinner);
                                }
                                if (winStatus)
                                {
                                    winStatus = 2;
                                }
                                else
                                {
                                    winStatus = 1;
                                }
                                for (int j = 0; j < i + 1; j++)
                                {
                                    incrementTimeString(winnerNumber);
                                }
                                if (winStatus == 1)
                                {
                                    win.setString("Player " + winnerNumber + " won!");
                                    firstWinner = winnerNumber;
                                }
                                if (winStatus == 2)
                                {
                                    win.setString(win.getString() + ", " + winnerNumber);
                                }
                            }
                        }
                    }
                    initWindow(window, road, rows, cols, distance);
                    for (int i = 0; i < numberOfPlayers; i++)
                    {
                        drawPath(window, path[i]);
                    }
                    window.draw(time);
                    if (winStatus)
                    {
                        window.draw(win);
                        win.setString("Press R to restart.");
                        win.setPosition(20, rows * distance - 45);
                        window.draw(win);
                    }
                    window.display();
                }
                
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    currentPlayer = (currentPlayer + 1) % numberOfPlayers;
                    if (currentPlayer == 0)
                    {
                        incrementTimeString(timeString);
                        time.setString("Time: " + timeString);
                    }
                    initWindow(window, road, rows, cols, distance);
                    for (int i = 0; i < numberOfPlayers; i++) 
                    {
                        drawPath(window, path[i]);
                    }
                    window.draw(time);
                    window.display();
                }
            }
        }
    }
    
    return 0;
}

void initWindow(sf::RenderWindow& window, std::vector<std::vector<sf::Vertex>> road, int rows, int cols, int distance)
{
    window.clear(sf::Color::White);
    sf::VertexArray line(sf::Lines, 2);
    line[0].color = line[1].color = sf::Color(192, 192, 192);
    for (int i = 1; i < rows; i++)
    {
        line[0].position = sf::Vector2f(0, i * distance);
        line[1].position = sf::Vector2f(cols * distance, i * distance);
        window.draw(line);
    }
    for (int i = 1; i < cols; i++)
    {
        line[0].position = sf::Vector2f(i * distance, 0);
        line[1].position = sf::Vector2f(i * distance, rows * distance);
        window.draw(line);
    }
    window.draw(&road[1][0], road[1].size(), sf::LineStrip);
    window.draw(&road[2][0], road[2].size(), sf::LineStrip);
    return;
}

void incrementTimeString(std::string& timeString)
{
    int currentDigit = timeString.length() - 1;
    timeString[currentDigit]++;
    while (timeString[currentDigit] == '0' + 10)
    {
        timeString[currentDigit] = '0';
        currentDigit--;
        if (currentDigit < 0) {
            timeString = "1" + timeString;
        }
        else
        {
            timeString[currentDigit]++;
        }
    }
    return;
}

sf::Color getPlayerColor(int player)
{
    switch (player)
    {
        case 0:
            return sf::Color::Red;
        case 1:
            return sf::Color::Blue;
        case 2:
            return sf::Color(0, 192, 0);
        case 3:
            return sf::Color(192, 192, 0);
        default:
            return sf::Color((154 * (player - 3)) % 255, (133 * (player - 3)) % 255, (74 * (player - 3)) % 255);
    }
}

std::vector<std::vector<sf::Vertex>> generateRoad(int rows, int cols, int distance)
{
    std::vector<std::vector<sf::Vertex>> road (3);
    int x = (rand() % (cols / 3) + cols / 3) * distance, y = rows * distance;
    int maxDeviation = 3;
    int roadHalfWidth = 4;
    int roadShiftPositive = 0;
    int roadShiftNegative = 0;
    road[0].push_back(sf::Vertex(sf::Vector2f(x, y), sf::Color::Black));
    road[1].push_back(sf::Vertex(sf::Vector2f(x - roadHalfWidth * distance, y), sf::Color::Black));
    road[2].push_back(sf::Vertex(sf::Vector2f(x + roadHalfWidth * distance, y), sf::Color::Black));
    int deviationX = rand() % (maxDeviation * 2 + 1) - maxDeviation;
    bool positiveDeviationChange = (deviationX < 0);
    while (y > 0)
    {
        x += deviationX * distance;
        y -= (rand() % 3 + 1) * distance;
        if (x - roadHalfWidth * distance <= 0)
        {
            roadShiftPositive = 
                (roadShiftPositive > ((roadHalfWidth + 1) * distance) - x)
                    ? roadShiftPositive
                    : ((roadHalfWidth + 1) * distance) - x;
        }
        if (x + roadHalfWidth * distance >= cols * distance)
        {
            roadShiftNegative =
                (roadShiftNegative < cols * distance - x - ((roadHalfWidth + 1) * distance))
                    ? roadShiftNegative
                    : cols * distance - x - ((roadHalfWidth + 1) * distance);
        }
        if (y < 0)
        {
            y = 0;
        }
        road[0].push_back(sf::Vertex(sf::Vector2f(x, y), sf::Color::Black));
        road[1].push_back(sf::Vertex(sf::Vector2f(x - roadHalfWidth * distance, y), sf::Color::Black));
        road[2].push_back(sf::Vertex(sf::Vector2f(x + roadHalfWidth * distance, y), sf::Color::Black));
        if ((2 * positiveDeviationChange - 1) * deviationX > maxDeviation)
        {
            positiveDeviationChange = !positiveDeviationChange;
        }
        deviationX += (2 * positiveDeviationChange - 1) * (rand() % (maxDeviation - 1) + 1);
    }
    if (roadShiftPositive && roadShiftNegative)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < road[i].size(); j++) 
            {
                road[i][j].position.x += (roadShiftPositive + roadShiftNegative) / 2;
                //road[i][j].color = sf::Color::Blue;
            }
        }
    }
    else
    {
        if (roadShiftPositive || roadShiftNegative)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < road[i].size(); j++) 
                {
                    road[i][j].position.x += (roadShiftPositive + roadShiftNegative);
                    //road[i][j].color = sf::Color::Red;
                }
            }
        } 
    }
    return road;
}

sf::Vector2f closestGridIntersection(int x, int y, int distance)
{
    return sf::Vector2f(x - x % distance + distance * (x % distance >= distance / 2), y - y % distance + distance * (y % distance >= distance / 2));
}

bool mouseInRange(int x, int y, int mouseX, int mouseY, int distance)
{
    sf::Vector2f closest = closestGridIntersection(mouseX, mouseY, distance);
    return (abs(x - closest.x) < distance * 2) && (abs(y - closest.y) < distance * 2);
}

void drawPath(sf::RenderWindow& window, std::vector<sf::Vertex> path)
{
    int radius = 3;
    window.draw(&path[0], path.size(), sf::LineStrip);
    for (int i = 0; i < path.size(); i++)
    {
        sf::CircleShape marker(radius);
        marker.setFillColor(path[i].color);
        marker.setPosition(path[i].position.x - radius, path[i].position.y - radius);
        window.draw(marker);
    }
    return;
}

void drawArrow(sf::RenderWindow& window, sf::Color color, int x, int y)
{
    sf::RectangleShape handle(sf::Vector2f(5, 9));
    handle.setPosition(x - 3, y - 5);
    handle.setFillColor(color);
    sf::VertexArray pointer(sf::Triangles, 3);
    pointer[0].position.x = x;
    pointer[0].position.y = y - 8;
    pointer[1].position.x = x - 5;
    pointer[1].position.y = y - 3;
    pointer[2].position.x = x + 5;
    pointer[2].position.y = y - 3;
    pointer[0].color = pointer[1].color = pointer[2].color = color;
    window.draw(handle);
    window.draw(pointer);
    return;
}

void drawMarkers(sf::RenderWindow& window, sf::Vertex from, sf::Vertex to, int distance, int mouseX, int mouseY)
{
    int radius = 2;
    sf::CircleShape marker(radius);
    marker.setFillColor(to.color);
    int x = 2 * to.position.x - from.position.x - radius, y = 2 * to.position.y - from.position.y - radius;
    
    marker.setPosition(x - distance, y);
    window.draw(marker);
    marker.setPosition(x, y);
    window.draw(marker);
    marker.setPosition(x + distance, y);
    window.draw(marker);
    
    y -= distance;
    
    marker.setPosition(x - distance, y);
    window.draw(marker);
    marker.setPosition(x, y);
    window.draw(marker);
    marker.setPosition(x + distance, y);
    window.draw(marker);
    
    y += 2 * distance;
    
    marker.setPosition(x - distance, y);
    window.draw(marker);
    marker.setPosition(x, y);
    window.draw(marker);
    marker.setPosition(x + distance, y);
    window.draw(marker);
    
    y -= distance;
    
    if (mouseInRange(x + radius, y + radius, mouseX, mouseY, distance))
    {
        sf::Vector2f closest = closestGridIntersection(mouseX, mouseY, distance);
        marker.setPosition(closest.x - radius, closest.y - radius);
        marker.setFillColor(sf::Color::Green);
        window.draw(marker);
    }
    
    return;
}

std::vector<bool> checkWinningConditions(std::vector<std::vector<sf::Vertex>> path)
{
    int maxHeight = 0;
    std::vector<bool> winningPlayers (path.size(), false);
    for (int i = 0; i < path.size(); i++)
    {
        if (path[i].size())
        {
            if (path[i][path[i].size() - 1].position.y <= maxHeight)
            {
                maxHeight = path[i][path[i].size() - 1].position.y;
            }
        }
    }
    for (int i = 0; i < path.size(); i++)
    {
        if (path[i].size())
        {
            if (path[i][path[i].size() - 1].position.y == maxHeight)
            {
                winningPlayers[i] = true;
            }
        }
    }
    return winningPlayers;
}