/*
 * ilmenit's sample benchmark in the "how to optimize for cc65" tutorial.
 *
 * original: https://github.com/ilmenit/CC65-Advanced-Optimizations/blob/master/01-start/game.c
 * (first step, without optimization)
 *
 * Changes from original:
 *  - Benchmark framework replaced by a volatile access (the emulator does the bench in our case)
 *  - atari.h dependency replaced by a constant to a memory area for the screen
 *  - Removed useless semicolon after functions
 *  - Less unused space on screen (screen entirely fits 2 pages)
 *
 */

#define SCREEN_SIZE_X 40
#define SCREEN_SIZE_Y 24
#define NO_ENEMIES 30
#define _countof(array) (sizeof(array) / sizeof(array[0]))

unsigned char screen[SCREEN_SIZE_X*SCREEN_SIZE_Y];

typedef enum e_entity_type {
    ENTITY_DEAD,
    ENTITY_PLAYER,
    ENTITY_ENEMY
} e_entity_type;

typedef struct s_entity {
    int x;
    int y;
    int hp;
    e_entity_type type;
} s_entity;

typedef struct s_player {
    s_entity entity;
    int attack;
} s_player;


typedef struct s_game_state {
    s_entity enemies[NO_ENEMIES];
    s_player player;
} s_game_state;

void place_enemy(s_entity *e_ptr, int x, int y)
{
    e_ptr->x = x;
    e_ptr->y = y;
}

void set_entities(s_game_state *game_state)
{
    int index;
    s_entity *e;
    // set enemies
    for (index = 0; index < _countof(game_state->enemies); index++)
    {
        e = &game_state->enemies[index];
        place_enemy(e, (index*5) % SCREEN_SIZE_X, index / 5 + 1);
        e->hp = 99;
        e->type = ENTITY_ENEMY;
    };
    // set player
    game_state->player.entity.hp = 99;
    game_state->player.entity.x = SCREEN_SIZE_X/2;
    game_state->player.entity.type = ENTITY_PLAYER;
}

char get_entity_tile(e_entity_type type)
{
    switch(type)
    {
        case ENTITY_PLAYER:
            return 'p';
        case ENTITY_ENEMY:
            return 'e';
    }
    return 'x';
}

void draw_entity(unsigned char *screen_ptr, s_entity *e_ptr)
{
    const int FIRST_DIGIT_CHAR = 0x10;
    unsigned char *draw_ptr = &screen_ptr[e_ptr->y * SCREEN_SIZE_X + e_ptr->x];
    *draw_ptr = get_entity_tile(e_ptr->type);
    *(++draw_ptr) = e_ptr->hp / 10 + FIRST_DIGIT_CHAR;
    *(++draw_ptr) = e_ptr->hp % 10 + FIRST_DIGIT_CHAR;
}

void damage_enemy(s_entity *e_ptr)
{
    // damage
    if (e_ptr->hp > 0)
        e_ptr->hp--;
}

void one_frame(s_game_state *game_state, unsigned char *screen_ptr)
{
    int index;
    s_entity *e;

    // draw enemies
    for (index = 0; index < _countof(game_state->enemies); index++)
    {
        e = &game_state->enemies[index];
        damage_enemy(e);
        draw_entity(screen_ptr, e);
    };
    // draw player
    draw_entity(screen_ptr, &game_state->player.entity);

}

volatile char v;

int main(void)
{
    unsigned char *screen_ptr;
    unsigned int times;

    s_game_state game_state;

    screen_ptr = screen;
    set_entities(&game_state);

    for (times = 0; times < 100; ++times) {
        one_frame(&game_state, screen_ptr);

		// Store to a volatile to inhibit optimization.
        v = *(char*)&game_state;
        v = *screen_ptr;
	}
    return 0;
}
