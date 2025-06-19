#pragma once

// Объявляем перечисление (enum) для типов сущностей
enum class EntityType {
    ZOMBIE,
    PLANT,
    WALL,
    HOME,
    MISSILE,
    END_GAME_ZONE,
    // Добавлено для расширяемости
};


// Основная сущность
struct Entity {
    EntityType type;
    // Абсолютные координаты в консоле
    int X;
    int Y;

    virtual void Handle()
    {
    }
};

struct Home : public Entity
{
    int LineNumber;
    Home(int x, int y, int lineNubmer) : LineNumber(lineNubmer) {
        this->type = EntityType::HOME;
        this->X = x;
        this->Y = y;
    }

};

struct Zombe : public Entity
{
    int Health;
    int Damage;

    Zombe(int x, int y, int health, int damage) : Health(health), Damage(damage)
    {
        this->type = EntityType::ZOMBIE;
        this->X = x;
        this->Y = y;
    }

    void GetDamage(int damage)
    {
        Health -= damage;
    }
};

// Снаряд ( в полёте уже который, если будем их рисовать, конечно)
struct Missile : public Entity {

    int Damage; // Урон или какой снаряд метать
    Missile(int x, int y, int damage) : Damage(damage)
    {
        this->type = EntityType::MISSILE;
        this->X = x;
        this->Y = y;
    }
    void DoDamage(Zombe* zombe)
    {
        
    }
};

struct Plant : public Entity {
    int Health;

    Plant(int x, int y, int health) : Health(health)
    {
        this->type = EntityType::PLANT;
        this->X = x;
        this->Y = y;
    }

    void GetDamage(int damage)
    {
        Health -= damage;
    };

    Missile* SpawnMissile(int x, int y, int damage) 
    {
        return new Missile(x, y, damage);
    };

};
