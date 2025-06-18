#include <vector>


// работа со временем
#include <chrono>
#include <thread>
#include "Entities.h"

void RefreshMap(std::vector<Entity*> entities);


void DeleteMissile(std::vector<Entity*>& entities, Missile* missile)
{
    // Ищем снаряд в entities и удаляем
    auto it = std::find(entities.begin(), entities.end(), missile);
    if (it != entities.end())
    {
        entities.erase(it);
        delete missile;
    }
}

// Функция, двигающая все снаряды в векторе, наносит урон зомби, убирает снаряды и зомби если хп <1
void Enginefun(std::vector<Entity*>& entities, int& linelength, int& gameover)
{
    // Сдвигаем каждый снаряд вправо и проверяем взаимодействие Реализация Шага 1
    for (Entity* e : entities)
    {
        Missile* missile = dynamic_cast<Missile*>(e);
        if (missile != nullptr)
        {
            missile->X += 1;

            // Если снаряд вышел за границу, помечаем его на удаление

            if (missile->X >= linelength)
            {
                DeleteMissile(entities, missile);
                continue;
                // снаряд улетел и его не нужно проверять на столкновение
            }

            // Проверяем столкновение с зомби Реализация Шага 2
            for (auto it = entities.begin(); it != entities.end(); ++it)
            {
                Entity* e = *it;
                if (e->type == EntityType::ZOMBIE)
                {
                    Zombe* zombie = dynamic_cast<Zombe*>(e);

                    if (zombie != nullptr && zombie->X == missile->X && zombie->Y == missile->Y)
                    {
                        zombie->GetDamage(missile->Damage);

                        DeleteMissile(entities, missile);

                        // Помечаем снаряд на удаление

                        // Шаг 3: удаляем зомби с Health <= 0
                        if (zombie->Health < 1) {
                            entities.erase(it);
                            delete zombie;
                            break;
                        }

                        break; // снаряд попал, дальше проверять не нужно
                    }
                }
            }

        }
    }

   // Двигаем зомби шаг 4 5
    for (Entity* e : entities)
    {
        Zombe* zombie = dynamic_cast<Zombe*>(e);
        if (zombie != nullptr)
        {
            for (Entity* e2 : entities)
            {
                if (zombie->Y == e2->Y && zombie->X - 1 == e2->X)
                {
                    Plant* plant;
                    Home* home;
                    switch (e2->type)
                    {
                    case EntityType::PLANT:
                        plant = dynamic_cast<Plant*>(e2);
                        plant->GetDamage(zombie->Damage);
                        if (plant->Health < 1)
                        {
                            auto it = std::find(entities.begin(), entities.end(), plant);
                            if (it != entities.end())
                            {
                                entities.erase(it);
                                delete plant;
                            }
                        }
                        break;

                    case EntityType::HOME:
                        gameover = 1;
                        break;

                    case EntityType::ZOMBIE:
                        break;
                    default:
                        zombie->X -= 1;
                        break;
                    }
                }
            }
        }
    }
    

    // Растение спавнит снаряд
    for (Entity* e : entities)
    {
        
        Plant* plant = dynamic_cast<Plant*>(e);
        if (plant != nullptr)
        {
            plant->SpawnMissile(plant->X, plant->Y, 1);
        }
    }

    RefreshMap(entities);
}


void Start(int rows, int columns, int house_offset, int FLy, int linelength)
{
    static int gameover;
    static std::vector<Entity*> entities;
    for (int i = 0; i < rows; i++)
    {
        Entity* entity = new Home(house_offset, FLy, i+1);
        entities.push_back(entity);
        FLy += 2;

    }
    entities.push_back(new Plant(house_offset + 1, FLy, 3));
    entities.push_back(new Zombe(linelength - 1, FLy, 5, 1));
    //
    const int n = 120; // количество раз в секунду
    const std::chrono::duration<double> interval(1.0 / n); // Делим секунду на интеравалы (делим секунду на n частей)
    for (;;)
    {
        auto start = std::chrono::steady_clock::now();
        Enginefun(entities, linelength, gameover);
        // Основная часть

        // Если снаряд спавнится перед растением, то шаг 1 и шаг 2 меняем местами

        // Сделано Шаг 1: двигаем снаряды (начиная с самого правого)
        // Сделано Шаг 2: Проверяем попал ли снаряд. Если снаряд попал уменьшаем хп
        // Сделано Шаг 3: Убираем мёртвых зомби из вектора
        // Шаг 4: Двигаем зомби, если нужно инициируем атаку
        // Шаг 5: Убираем мёртвые растения
        // Шаг 6: Растения создают снаряды


        // Конец основной части
        
        // Бездействуем и ждём (чтобы в секунду выполнилось именно n раз)
        std::this_thread::sleep_until(start + interval);
    }

}