#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <mutex>
#include <condition_variable>

#include <string>

#include "Entities.h"

void RefreshMap(std::vector<Entity*> entities);

std::string _inputMessage;
bool _pause = false;

char WaitForUserInput(std::chrono::seconds timeout) {
    std::promise<char> prom;
    std::future<char> fut = prom.get_future();

    // Запускаем поток для чтения ввода
    std::thread input_thread([&prom]() {
        char ch;
        std::cin >> ch; // ожидает ввод пользователя
        prom.set_value(ch);
        });

    // Ждём либо ввода, либо истечения таймаута
    if (fut.wait_for(timeout) == std::future_status::ready) {
        // Ввод получен вовремя
        char ch = fut.get();
        input_thread.join();
        return ch;
    }
    else {
        // Таймаут истёк, завершаем поток чтения
        // Важно: std::cin не прерывается, поэтому поток может остаться заблокированным,
        // но для простоты примера мы его оставим. В реальных приложениях можно использовать
        // более сложные механизмы или отключить ввод.
        input_thread.detach(); // или оставить как есть
        return '\0'; // возвращаем нулевой символ, если ввод не был сделан
    }
}


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

// Простая пузырьковая сортировка вектора указателей на Entity по X
// ascending = true — сортировка по возрастанию, false — по убыванию
void SortEntitiesByX(std::vector<Entity*>& entities, bool ascending)
{
    size_t n = entities.size();
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 1; j < n - i; ++j)
        {
            bool condition = ascending ?
                (entities[j - 1]->X > entities[j]->X) :
                (entities[j - 1]->X < entities[j]->X);

            if (condition)
            {
                Entity* temp = entities[j - 1];
                entities[j - 1] = entities[j];
                entities[j] = temp;
            }
        }
    }
}

// Основная функция игрового движка
void Enginefun(std::vector<Entity*>& entities, int& linelength, int& gameover)
{
    std::vector<Missile*> missilesToRemove;
    std::vector<Zombe*> zombiesToRemove;
    std::vector<Plant*> plantsToRemove;

    // --- Обработка снарядов ---
    for (auto it = entities.begin(); it != entities.end(); )
    {
        Missile* missile = dynamic_cast<Missile*>(*it);
        if (missile == nullptr)
        {
            ++it;
            continue;
        }

        bool hitOccurred = false;

        // Сначала проверяем, есть ли зомби на текущей позиции снаряда (на следующей)
        for (Entity* e : entities)
        {
            Zombe* zombie = dynamic_cast<Zombe*>(e);
            if (zombie != nullptr && zombie->X+1 == missile->X && zombie->Y == missile->Y)
            {
                zombie->GetDamage(missile->Damage);
                hitOccurred = true;
                if (zombie->Health < 1)
                {
                    zombiesToRemove.push_back(zombie);
                }
                break; // Нанесли урон первому найденному зомби, прекращаем поиск
            }
        }

        if (hitOccurred)
        {
            // Снаряд попал — удаляем его
            it = entities.erase(it);
            delete missile;
            continue; // Переходим к следующему элементу
        }

        // Если не попал, двигаем снаряд
        missile->X += 1;

        // Проверяем, не вышел ли снаряд за пределы
        if (missile->X >= linelength+1)
        {
            it = entities.erase(it);
            delete missile;
            continue;
        }

        // Проверяем попадание после движения
        hitOccurred = false;
        for (Entity* e : entities)
        {
            Zombe* zombie = dynamic_cast<Zombe*>(e);
            if (zombie != nullptr && zombie->X == missile->X && zombie->Y == missile->Y)
            {
                zombie->GetDamage(missile->Damage);
                hitOccurred = true;
                if (zombie->Health < 1)
                {
                    zombiesToRemove.push_back(zombie);
                }

                break;
            }
        }

        if (hitOccurred)
        {
            it = entities.erase(it);
            delete missile;
            continue;
        }

        ++it;
    }

    // --- Удаление зомби ---
    for (Zombe* zombie : zombiesToRemove)
    {
        auto it = std::find(entities.begin(), entities.end(), zombie);
        if (it != entities.end())
        {
            delete* it;
            entities.erase(it);
        }
    }

    // --- Обработка зомби: движение и атака ---

    // Собираем зомби в отдельный вектор
    std::vector<Entity*> zombies;
    for (Entity* e : entities)
    {
        if (e->type == EntityType::ZOMBIE)
            zombies.push_back(e);
    }

    // Сортируем зомби по убыванию X (чтобы двигать правых первыми)
    // Переделал на возрастание, чтобы двигать левых первым
    SortEntitiesByX(zombies, true);



    for (Entity* e : zombies)
    {
        Zombe* zombie = dynamic_cast<Zombe*>(e);
        if (zombie != nullptr)
        {
            bool attackedOrMoved = false;

            // Проверяем соседнюю клетку слева
            for (Entity* e2 : entities)
            {
                if (zombie->Y == e2->Y && zombie->X - 1 == e2->X)
                {
                    switch (e2->type)
                    {
                    case EntityType::PLANT:
                    {
                        Plant* plant = dynamic_cast<Plant*>(e2);
                        if (plant != nullptr)
                        {
                            plant->GetDamage(zombie->Damage);
                            if (plant->Health < 1)
                            {
                                plantsToRemove.push_back(plant);
                            }
                        }
                        attackedOrMoved = true;
                        break;
                    }
                    case EntityType::HOME:
                        gameover = 1;
                        attackedOrMoved = true;
                        break;
                    case EntityType::ZOMBIE:
                        attackedOrMoved = true;
                        break;
                    default:
                        break;
                    }
                    if (attackedOrMoved)
                        break;
                }
            }

            if (!attackedOrMoved)
            {
                // Проверяем, свободна ли клетка слева
                bool canMoveLeft = true;
                for (Entity* other : entities)
                {
                    if (other == zombie || other->type == EntityType::MISSILE) continue;
                    if (other->X == zombie->X - 1 && other->Y == zombie->Y)
                    {
                        canMoveLeft = false;
                        break;
                    }
                }
                if (canMoveLeft)
                {
                    zombie->X -= 1;
                }
            }

            if (zombie->Health < 1)
            {
                zombiesToRemove.push_back(zombie);
            }
        }
    }



    // --- Удаление снарядов ---
    for (Missile* missile : missilesToRemove)
    {
        auto it = std::find(entities.begin(), entities.end(), missile);
        if (it != entities.end())
        {
            delete* it;
            entities.erase(it);
        }
    }

    // --- Удаление зомби ---
    for (Zombe* zombie : zombiesToRemove)
    {
        auto it = std::find(entities.begin(), entities.end(), zombie);
        if (it != entities.end())
        {
            delete* it;
            entities.erase(it);
        }
    }

    // --- Удаление растений ---
    for (Plant* plant : plantsToRemove)
    {
        auto it = std::find(entities.begin(), entities.end(), plant);
        if (it != entities.end())
        {
            delete* it;
            entities.erase(it);
        }
    }

    // --- Растения создают снаряды ---
    std::vector<Missile*> newMissiles;
    for (Entity* e : entities)
    {
        Plant* plant = dynamic_cast<Plant*>(e);
        if (plant != nullptr)
        {
            Missile* newMissile = plant->SpawnMissile(plant->X, plant->Y, 1);
            newMissiles.push_back(newMissile);
        }
    }
    for (Missile* m : newMissiles)
    {
        entities.push_back(m);
    }

    // Обновляем карту
    RefreshMap(entities);
    std::cout << _inputMessage;
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

    //entities.push_back(new Home(house_offset+5, FLy, 1));
    entities.push_back(new Plant(house_offset + 1, FLy - 2, 3));
    entities.push_back(new Zombe(linelength - 1, FLy - 2, 5, 1));
    entities.push_back(new Zombe(linelength - 2, FLy - 2, 10, 1));
    entities.push_back(new Zombe(linelength - 3, FLy - 2, 10, 1));
    entities.push_back(new Zombe(linelength - 4, FLy - 2, 10, 1));
    //
    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        //char user_input = WaitForUserInput(std::chrono::seconds(1));
        //if (user_input != '\0') {
        //    _inputMessage += user_input;
        //    //_pause = true;
        //}

        if (!_pause) {
            Enginefun(entities, linelength, gameover);
        }
        if (gameover == 1)
        {
            exit(0);
        }
        // Основная часть

        // Если снаряд спавнится перед растением, то шаг 1 и шаг 2 меняем местами

        // Сделано Шаг 1: двигаем снаряды (начиная с самого правого)
        // Сделано Шаг 2: Проверяем попал ли снаряд. Если снаряд попал уменьшаем хп
        // Сделано Шаг 3: Убираем мёртвых зомби из вектора
        // Шаг 4: Двигаем зомби, если нужно инициируем атаку
        // Шаг 5: Убираем мёртвые растения
        // Шаг 6: Растения создают снаряды


        // Конец основной части
    }

}