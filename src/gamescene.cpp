#include "gamescene.h"
#include "utils.h"
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include "starsystem.h"

GameScene::GameScene(QObject *parent)
    : QGraphicsScene(parent)
{
    onUserCreate();
}

GameScene::~GameScene()
{

}

void GameScene::loop()
{
    m_deltaTime = m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();

    m_loopTime += m_deltaTime;
    while(m_loopTime > m_loopSpeed)
    {
        m_loopTime -= m_loopSpeed;
        handlePlayerInput();

        int nSectorsX = SCREEN::PHYSICAL_SIZE.width() / (16 * SCREEN::CELL_SIZE.width());
        int nSectorsY = SCREEN::PHYSICAL_SIZE.height() / (16 * SCREEN::CELL_SIZE.height());

        QVector2D mousePos;
        mousePos.setX( m_mouse->m_x / (16 * SCREEN::CELL_SIZE.width()) );
        mousePos.setY( m_mouse->m_y / (16 * SCREEN::CELL_SIZE.height()) );

        QVector2D galaxyMousePos;
        galaxyMousePos.setX( mousePos.x() + offset.x());
        galaxyMousePos.setY( mousePos.y() + offset.y());

        int screenSectorX = 0;
        int screenSectorY = 0;


        clear();
        for (screenSectorX = 0; screenSectorX < nSectorsX; ++screenSectorX) {
            for (screenSectorY = 0; screenSectorY < nSectorsY; ++screenSectorY)
            {
                quint32 seed1 = (quint32)galaxyMousePos.x() + (quint32)screenSectorX;
                quint32 seed2 = (quint32)galaxyMousePos.y() + (quint32)screenSectorY;
                StarSystem star(seed1, seed2);
                if (star.starExists) {
                    int planetOffset = (32 - (int)star.starDiameter) / 2;
                    addEllipse(screenSectorX * 32 + planetOffset, screenSectorY * 32 + planetOffset,(int)star.starDiameter,
                               (int)star.starDiameter, QPen(star.starColour), QBrush(star.starColour));

                    if (mousePos.x() + 1 > screenSectorX && screenSectorX + 1 > mousePos.x()
                        && mousePos.y() + 1 > screenSectorY && screenSectorY + 1 > mousePos.y() ) {
                        addEllipse(screenSectorX * 32, screenSectorY * 32, 32, 32, QPen(Qt::yellow), QBrush(Qt::NoBrush));
                    }
                }
            }
        }

        if (m_mouse->m_released) {
            quint32 seed1 = (quint32)galaxyMousePos.x() + (quint32)mousePos.x();
            quint32 seed2 = (quint32)galaxyMousePos.y() + (quint32)mousePos.y();

            StarSystem star(seed1, seed2);
            if (star.starExists)
            {
                bStarSelected = true;
                nSelectedStarSeed1 = seed1;
                nSelectedStarSeed2 = seed2;
            }
            else {
                bStarSelected = false;
            }
        }

        if(bStarSelected)
        {
            StarSystem star(nSelectedStarSeed1, nSelectedStarSeed2, true);

            addRect(16, 2*240, 2*496, 2*232, QPen(Qt::white), QBrush(Qt::darkBlue));
            QVector2D vBody(2*14, 1.5*356);
            vBody.setX(vBody.x() + (star.starDiameter * 1.375 * 2));
            addEllipse(vBody.x(), vBody.y(), (star.starDiameter * 1.375 * 2) + 16, (star.starDiameter * 1.375 * 2) + 16, QPen(star.starColour), QBrush(star.starColour));
            vBody.setX(vBody.x() + (star.starDiameter * 1.375 * 3 + 16));

            for (auto& planet : star.vPlanets) {
                if (vBody.x() + planet.diameter >= 496)
                {
                    break;
                }

                vBody.setX(vBody.x() + planet.diameter);
                addEllipse(vBody.x(), vBody.y(), planet.diameter, planet.diameter, QPen(Qt::red), QBrush(Qt::red));

                QVector2D vMoon = vBody;

                for (auto& moon : planet.vMoons) {
                    vMoon.setY(vMoon.y() + moon);
                    addEllipse(vMoon.x(), vMoon.y(), moon, moon, QPen(Qt::gray), QBrush(Qt::gray));
                    vMoon.setY(vMoon.y() + moon + (10*2));
                }
                vBody.setX(vBody.x() + (planet.diameter + 8 * 3));
            }
        }


        resetStatus();
    }
}

void GameScene::onUserCreate()
{
    setBackgroundBrush(COLOR_STYLE::BACKGROUND);
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i] = new KeyStatus();
    }
    m_mouse = new MouseStatus();
    setSceneRect(0,0, SCREEN::PHYSICAL_SIZE.width(), SCREEN::PHYSICAL_SIZE.height());
    connect(&m_timer, &QTimer::timeout, this, &GameScene::loop);
    m_timer.start(int(1000.0f/FPS));
    m_elapsedTimer.start();

    offset.setX(0);
    offset.setY(0);
    bStarSelected = false;
    nSelectedStarSeed1 = 0;
    nSelectedStarSeed2 = 0;
}

void GameScene::renderScene()
{
    static int index = 0;
    QString fileName = QDir::currentPath() + QDir::separator() + "screen" + QString::number(index++) + ".png";
    QRect rect = sceneRect().toAlignedRect();
    QImage image(rect.size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    render(&painter);
    image.save(fileName);
    qDebug() << "saved " << fileName;
}

void GameScene::handlePlayerInput()
{
    if(m_keys[KEYBOARD::KeysMapper[Qt::Key_Z]]->m_released)
    {
        renderScene();//uncoment if want to make screenshots
    }

    int speed = 1;
    if (m_keys[KEYBOARD::KeysMapper[Qt::Key_W]]->m_held) {
        offset.setY(offset.y() - speed);
    }
    else if (m_keys[KEYBOARD::KeysMapper[Qt::Key_D]]->m_held) {
        offset.setX(offset.x() + speed);
    }
    else if (m_keys[KEYBOARD::KeysMapper[Qt::Key_S]]->m_held) {
        offset.setY(offset.y() + speed);
    }
    else if (m_keys[KEYBOARD::KeysMapper[Qt::Key_A]]->m_held) {
        offset.setX(offset.x() - speed);
    }
}

void GameScene::resetStatus()
{
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i]->m_released = false;
    }
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i]->m_pressed = false;
    }
    m_mouse->m_released = false;
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    if(KEYBOARD::KeysMapper.contains(event->key()))
    {
        if(event->isAutoRepeat())
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held = true;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = false;
        }
        else
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = true;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held    = false;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void GameScene::keyReleaseEvent(QKeyEvent *event)
{
    if(KEYBOARD::KeysMapper.contains(event->key()))
    {
        if(!event->isAutoRepeat())
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held = false;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = false;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_released = true;
        }

    }
    QGraphicsScene::keyReleaseEvent(event);
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    m_mouse->m_pressed = true;
    QGraphicsScene::mousePressEvent(event);
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    QGraphicsScene::mouseMoveEvent(event);
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    m_mouse->m_pressed = false;
    m_mouse->m_released = true;
    QGraphicsScene::mouseReleaseEvent(event);
}
