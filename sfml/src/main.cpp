#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <iostream>

using namespace std;
using namespace sf;

constexpr int windowWidth{800};
constexpr int windowHeight{800};

constexpr float rockRadius{350.f};
constexpr float maxRockRadius{30};
constexpr int maxRocks{100};
constexpr float rockDefaultVelocity{2.0f};
//constexpr float maxRockVelocity{2.0f};

constexpr float shipVelocity{3.5f};
constexpr float shipRadius{10.0f};

constexpr int bulletDelayMs{250};       // minimum time in ms between consectuve bullt shots
constexpr float bulletLength{25.f};
constexpr float bulletWidth{2.f};
constexpr int maxBullets{500};

constexpr int scoreAliveTimoutMs{1000};

bool debugMode{false};
bool joystickOn{false};

// Helper functions
//-----------------------------------------------------------------------------
int iRando(int max) {
    // returns a random number from 0 to max
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> ud(0, max);
    return ud(e);
}
float rRando(float max) {
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_real_distribution<float> ud(0, max);
    return ud(e);
}

// Game entities
//-----------------------------------------------------------------------------
struct Rock {
    CircleShape shape;

    Vector2f velocity{0.f, -rockDefaultVelocity};   // velocity is always going straight up (for now)

    bool destroyed{false};

    explicit Rock(const sf::Texture& texture) {
        // initial position random X and is off screen on the bottom edge
        auto mX = (float)iRando(windowWidth);
        shape.setPosition(mX, windowHeight + maxRockRadius);
        // shape?
        auto points = iRando(2) + 7;
        shape.setPointCount(points);
        // size?
        auto mRadius = (float)iRando(20) + 10.0f;
        shape.setRadius(mRadius);

        shape.setOrigin(mRadius, mRadius);
        // texture
        shape.setTexture(&texture);
        // for some flare rotate it so they all don't look the same
        shape.rotate((float)iRando(359));
        // vary the rate of motion
        velocity.y += rRando(1.0f);
    }

    void update() {
        shape.move(velocity);

        if (bottom() < 0) {
            velocity.y = 0;
            destroyed = true;
        }
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getRadius(); }
    float right() { return x() + shape.getRadius(); }
    float top() { return y() - shape.getRadius(); }
    float bottom() { return y() + shape.getRadius(); }
};

struct Ship {
    CircleShape shape{shipRadius, 3};

    Vector2f velocity{0.0f, 0.0f};

    bool destroyed{false};

    explicit Ship() {
        // initial position random X and is off screen on the bottom edge
        shape.setPosition(windowWidth / 2.0, shipRadius + 3.0f);
        shape.setFillColor(Color::Blue);
        shape.setOrigin(shipRadius, shipRadius);
        shape.rotate(60.0f);
    }

    void update() {
        shape.move(velocity);

        if (joystickOn) {
            auto jX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);     // in the range -100 to 100
            auto jY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);

            if ((jX < -10.f && left() > 0) || (jX > 10.f && right() < windowWidth)) {
                velocity.x = (shipVelocity * (jX / 100.f));
            }
            else velocity.x = 0;

            if ((jY < -10.f && top() > 0) || (jY > 10.f && bottom() < windowHeight /2.f)) {
                velocity.y = (shipVelocity * (jY / 100.f));
            }
            else velocity.y = 0;
        }
        else {
            if (Keyboard::isKeyPressed(Keyboard::Key::Left) && left() > 0) velocity.x = -shipVelocity;
            else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && right() < windowWidth) velocity.x = shipVelocity;
            else velocity.x = 0;
        }
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getRadius(); }
    float right() { return x() + shape.getRadius(); }
    float top() { return y() - shape.getRadius(); }
    float bottom() { return y() + shape.getRadius(); }
};

struct Bullet {
    RectangleShape shape;
    //Vector2f velocity{0.0f, 1.0f};
    Vector2f velocity{0.0f, shipVelocity + 1.f};

    bool destroyed {false};

    explicit Bullet(float mX, float mY) {
        shape.setPosition(mX, mY);
        shape.setSize({bulletWidth, bulletLength});
        shape.setFillColor(Color::White);
        shape.setOrigin(bulletWidth / 2.f, bulletLength / 2.f);
    }

    void update() {
        shape.move(velocity);
        if (shape.getPosition().y > windowHeight + bulletLength)
            destroyed = true;
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getSize().x / 2.f; }
    float right() { return x() + shape.getSize().x / 2.f; }
    float top() { return y() - shape.getSize().y / 2.f; }
    float bottom() { return y() + shape.getSize().y / 2.f; }
};

class ScoreBox : public sf::Drawable
{
private:
    sf::RectangleShape scoreBorder{sf::Vector2f{140.0f, 50.0f}};
    sf::Font scoreFont;
    sf::Text scoreText;
    sf::VertexArray m_vertices;
    int score{0};
    float xText{windowWidth - 4.f};
    float xMax{windowWidth - 4.f};
public:
    ScoreBox() {
        scoreBorder.setPosition(windowWidth - 142.0f, windowHeight - 51.0f);
        scoreBorder.setFillColor(Color::Black);
        scoreBorder.setOutlineThickness(1.0f);
        scoreBorder.setOutlineColor(Color::White);
        scoreFont.loadFromFile("UbuntuMono-B.ttf");
        scoreText.setPosition(xText, windowHeight - 51.0f);
        scoreText.setFont(scoreFont);
        scoreText.setFillColor(Color::Yellow);
        scoreText.setString(to_string(score));
        //m_vertices.append(scoreBorder);       // error: can't convert from sfRectanle to sfVertex
    }
    void addToScore(int amount = 1) {
        score += amount;
        updateScoreText();
    }
    void resetScore() {
        score = 0;
        updateScoreText();
    }

private:
    void updateScoreText() {
        scoreText.setString(to_string(score));
        // adjust the scores position so it's right aligned
        auto scoreBounds = scoreText.getGlobalBounds();
        if (scoreBounds.left + scoreBounds.width > xMax) xText = xMax - (scoreBounds.width + 4.f);
        scoreText.setPosition(xText, windowHeight - 51.f);
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        // draw the vertex array
        //target.draw(m_vertices, states);
        target.draw(scoreBorder);
        target.draw(scoreText);
    }

};


// Game functions
//-----------------------------------------------------------------------------

template<class T1, class T2> bool isIntersecting(T1&mA, T2& mB) {
    return mA.right() >= mB.left() && mA.left() <= mB.right() &&
           mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void testCollision(Ship& mShip, Rock& mRock) {
    if (!isIntersecting(mShip, mRock)) return;
    mShip.destroyed = true;
}
bool testCollision(Rock& mRock, Bullet& mBullet) {
    if (!isIntersecting(mRock, mBullet)) return false;
    mRock.destroyed = true;
    mBullet.destroyed = true;
    return true;
}

enum class GameState {
    Unknown,
    Playing,
    Dead
};
// MAIN
//-----------------------------------------------------------------------------
int main()
{
    // get a list of textures (pieces from a file)
    // the rock.png file is 509x450 pixels
    vector<sf::Texture> rockTextures;
    for (int i{0}; i < 320; i+=32) {
        rockTextures.emplace_back();
        rockTextures.back().loadFromFile("rock.png", sf::IntRect(i, 0, 32, 32));
    }

    vector<Rock> rocks;             // our rocky starfield
    Ship ship;                      // player ship
    ScoreBox scorebox{};            // Score stuff
    vector<Bullet> bullets;         // anything fired from the players ship

    // joystick testing
    sf::Text testText;
    sf::Font testFont;
    testFont.loadFromFile("UbuntuMono-B.ttf");
    testText.setPosition(10.0f, 10.0f);
    testText.setFont(testFont);
    testText.setFillColor(Color::Yellow);
    testText.setString("Test");

    sf::RectangleShape shipBorder(sf::Vector2f(10.f, 10.f));

    sf::SoundBuffer soundBuffer;
    soundBuffer.loadFromFile("pew.ogg");
    sf::Sound bulletSound;
    bulletSound.setBuffer(soundBuffer);

    // game state - ie: are you dead or not
    GameState gameState = GameState::Playing;

    // SFML housekeeping
    auto window = sf::RenderWindow{ { windowWidth, windowHeight }, "Star Crunch" };
    window.setFramerateLimit(144);

    float waitForNew {windowHeight};

    sf::Clock scoreClock;
    sf::Clock bulletClock;
    bool okToFire {true};

    if (sf::Joystick::isConnected(0)) joystickOn = true;


    while (window.isOpen()) {
        for (auto event = sf::Event{}; window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(Keyboard::Key::R)) {
                ship.destroyed = false;
                ship.shape.setFillColor(Color::Blue);
                scorebox.resetScore();
            }
            /* for testing the controller
            if (event.type == sf::Event::JoystickMoved) {
                unsigned int njoy = event.joystickMove.joystickId; // Joy Number
                sf::Joystick::Axis eje = event.joystickMove.axis;  // Axis moved
                testText.setString((to_string(njoy)));
                if (eje == sf::Joystick::Axis::PovX) testText.setString("PovX");
                if (eje == sf::Joystick::Axis::PovY) testText.setString("PovY");
                if (eje == sf::Joystick::Axis::R) testText.setString("R");
                if (eje == sf::Joystick::Axis::U) testText.setString("U");
                if (eje == sf::Joystick::Axis::V) testText.setString("V");
                if (eje == sf::Joystick::Axis::X) testText.setString("X");
                if (eje == sf::Joystick::Axis::Y) testText.setString("Y");
                if (eje == sf::Joystick::Axis::Z) testText.setString("Z");
                float pos = event.joystickMove.position; // New position
                testText.setString((to_string(pos)));
            }
            */
        }

        if (joystickOn) {
            if (sf::Joystick::isButtonPressed(0, 3) && gameState == GameState::Dead) {
                gameState = GameState::Playing;
                ship.destroyed = false;
                ship.shape.setFillColor(Color::Blue);
                scorebox.resetScore();
            }
            if (gameState == GameState::Playing && okToFire && sf::Joystick::isButtonPressed(0, 0)) {
                okToFire = false;
                bulletClock.restart();
                bullets.emplace_back(ship.left() + (ship.right() - ship.left()) /2.f, ship.bottom());
                bulletSound.play();
            }
            if (sf::Joystick::isButtonPressed(0, 0)) testText.setString("0");
            if (sf::Joystick::isButtonPressed(0, 1)) testText.setString("1");
            if (sf::Joystick::isButtonPressed(0, 2)) testText.setString("2");
            if (sf::Joystick::isButtonPressed(0, 3)) testText.setString("3");
            if (sf::Joystick::isButtonPressed(0, 4)) testText.setString("4");
            if (sf::Joystick::isButtonPressed(0, 5)) testText.setString("5");
            if (sf::Joystick::isButtonPressed(0, 6)) testText.setString("6");
            if (sf::Joystick::isButtonPressed(0, 7)) testText.setString("7");
            if (sf::Joystick::isButtonPressed(0, 8)) testText.setString("8");
            if (sf::Joystick::isButtonPressed(0, 9)) testText.setString("9");
            if (sf::Joystick::isButtonPressed(0, 10)) testText.setString("10");
            if (sf::Joystick::isButtonPressed(0, 11)) testText.setString("11");
            if (sf::Joystick::isButtonPressed(0, 12)) testText.setString("12");
            if (sf::Joystick::isButtonPressed(0, 13)) testText.setString("13");
            if (sf::Joystick::isButtonPressed(0, 14)) testText.setString("14");
            if (sf::Joystick::isButtonPressed(0, 15)) testText.setString("15");
            if (sf::Joystick::isButtonPressed(0, 16)) testText.setString("16");
            if (sf::Joystick::isButtonPressed(0, 17)) testText.setString("17");
            if (sf::Joystick::isButtonPressed(0, 18)) testText.setString("18");
            if (sf::Joystick::isButtonPressed(0, 19)) testText.setString("19");
            if (sf::Joystick::isButtonPressed(0, 20)) testText.setString("20");
            if (sf::Joystick::isButtonPressed(0, 21)) testText.setString("21");
            if (sf::Joystick::isButtonPressed(0, 22)) testText.setString("22");
            if (sf::Joystick::isButtonPressed(0, 23)) testText.setString("23");
            if (sf::Joystick::isButtonPressed(0, 24)) testText.setString("24");
            if (sf::Joystick::isButtonPressed(0, 25)) testText.setString("25");
            if (sf::Joystick::isButtonPressed(0, 26)) testText.setString("26");
            if (sf::Joystick::isButtonPressed(0, 27)) testText.setString("27");
            if (sf::Joystick::isButtonPressed(0, 28)) testText.setString("28");
        }

        window.clear(Color::Black);

        if (gameState == GameState::Playing) {
            // update
            //---------------------------------------------------------------------------
            ship.update();

            for (auto &rock: rocks) rock.update();
            // remove rocks that have gone off-screen
            rocks.erase(remove_if(begin(rocks), end(rocks),
                                  [](const Rock &mRock) { return mRock.destroyed; }),
                        end(rocks));

            for (auto &bullet: bullets) bullet.update();
            // remove bullets that have gone off screen
            bullets.erase(remove_if(begin(bullets), end(bullets),
                                    [](const Bullet &mBullet) { return mBullet.destroyed; }),
                          end(bullets));

            // if we're out of rocks, or it's time, make new ones
            if (rocks.empty()) {
                rocks.emplace_back(rockTextures[iRando(rockTextures.size() - 1)]);
                waitForNew = windowHeight - (float) iRando(10);
            } else {
                Rock &lastRock = rocks.back();
                if (lastRock.shape.getPosition().y < waitForNew && rocks.size() < maxRocks) {
                    rocks.emplace_back(rockTextures[iRando(rockTextures.size() - 1)]);
                    waitForNew = windowHeight - (float) iRando(10);
                }
            }

            // check if any rock has hit the ship or a bullet
            if (!debugMode) {
                for (Rock &rock: rocks) {
                    testCollision(ship, rock);
                    for (Bullet &bullet: bullets) {
                        if (testCollision(rock, bullet)) scorebox.addToScore(100);
                    }
                }
            }
            if (ship.destroyed) {
                gameState = GameState::Dead;
                ship.shape.setFillColor(Color::Red);
            }


            // score timer
            sf::Time elapsed = scoreClock.getElapsedTime();
            auto t = elapsed.asMilliseconds();
            if ((gameState == GameState::Playing && t > scoreAliveTimoutMs) || debugMode) {
                scorebox.addToScore(1);
                scoreClock.restart();
            }
            // bullet rate controller
            elapsed = bulletClock.getElapsedTime();
            auto tb = elapsed.asMilliseconds();
            if (tb > bulletDelayMs && bullets.size() < maxBullets) {
                okToFire = true;
            }


            // collision perimeter check
            shipBorder.setPosition(ship.left(), ship.top());
            //auto sss = ship.shape.getRadius();
            auto sss = ship.shape.getGlobalBounds();
            shipBorder.setSize(sss.getSize());
        }
        // now draw
        //---------------------------------------------------------------------------
        for (auto& bullet : bullets) window.draw(bullet.shape);
        for (auto& rock: rocks) window.draw(rock.shape);
        //window.draw(shipBorder);
        window.draw(ship.shape);
        window.draw(scorebox);
        if (debugMode) window.draw(testText);
        window.display();
    }
    return 0;
}