#ifndef UNTITLED2_LIBRARY_H
#define UNTITLED2_LIBRARY_H

#include <stdio.h>
#include <stdlib.h>

struct color_alpha
{
    int r;
    int g;
    int b;
    int alpha;
};

typedef struct four_axis
{
    int y_start;
    int y_end;
    int x_start;
    int x_end;
    struct color_alpha color;
    int y_speed;
    int x_speed;
    int initial_y_start;
    int initial_y_end;
    int initial_x_start;
    int initial_x_end;
}geometrical_4axis;

struct window_meta
{
    int width;
    int height;
};

enum align_shape
{
    NO_ALLIGN,
    CENTER_Y,
    CENTER_X,
    OTHERSIDE_X,
    FULL_CENTER
};

geometrical_4axis create_four_axis(int x_start, int x_end, int y_start, int y_end, struct color_alpha color);
void allign(geometrical_4axis *shape, int allign_type, struct window_meta meta);
int process_movement_rebound(geometrical_4axis *shapes, size_t shape_size, int border_size, struct window_meta meta);
void process_movement(geometrical_4axis *shapes, size_t shape_size, int border_size, struct window_meta meta);
void keyboard_movement(geometrical_4axis *shape, int pressed_s, int pressed_d);
void process_collisions(geometrical_4axis *shapes, geometrical_4axis *shapes_collision, size_t shape_size, size_t collision_size);
#endif //UNTITLED2_LIBRARY_H
