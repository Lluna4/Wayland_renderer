#include "library.h"
#include <stdio.h>

int point1 = 0;
int point2 = 0;

geometrical_4axis create_four_axis(int x_start, int x_end, int y_start, int y_end, struct color_alpha color)
{
    const geometrical_4axis ret = 
    {y_start, y_end, x_start, x_end, color,
        0, 0, y_start, y_end, x_start, x_end};

    return ret;
}

void allign(geometrical_4axis *shape, int allign_type, struct window_meta meta)
{
    geometrical_4axis buffer = *shape;
    switch (allign_type)
    {
        case NO_ALLIGN:
            break;
        case CENTER_Y:
            shape->y_start = meta.height/2 - ((shape->y_end - shape->y_start)/2);
            shape->y_end = shape->y_start + (buffer.y_end - buffer.y_start);
            break;
        case CENTER_X:
            shape->x_start = meta.width/2 - ((shape->x_end - shape->x_start)/2);
            shape->x_end = shape->x_start + (buffer.x_end - buffer.x_start);
            break;
        case OTHERSIDE_X:
            shape->x_start = meta.width - shape->initial_x_end;
            shape->x_end = meta.width - shape->initial_x_start;
            break;
        case FULL_CENTER:
            shape->y_start = meta.height/2 - ((shape->y_end - shape->y_start)/2);
            shape->y_end = shape->y_start + (buffer.y_end - buffer.y_start);
            shape->x_start = meta.width/2 - ((shape->x_end - shape->x_start)/2);
            shape->x_end = shape->x_start + (buffer.x_end - buffer.x_start);
            break;
    }
}


int process_movement_rebound(geometrical_4axis *shapes, size_t shape_size, int border_size, struct window_meta meta)
{
    int ret = 0;
    for (int i = 0; i < shape_size; i++)
    {
        if (shapes[i].x_start <= border_size)
        {
            //allign(&shapes[i], FULL_CENTER, meta);
            shapes[i].x_speed *= -1;
            point2 += 1;
            printf("points scored! %i-%i now\n", point1, point2);
            ret = 1;
        }
        if (shapes[i].x_end >= meta.width - border_size)
        {
            //allign(&shapes[i], FULL_CENTER, meta);
            shapes[i].x_speed *= -1;
            point1 += 1;
            printf("points scored! %i-%i now\n", point1, point2);
            ret = 1;
        }
        if (shapes[i].y_start <= border_size || shapes[i].y_end >= meta.height - border_size)
        {
            shapes[i].y_speed *= -1;
            if (shapes[i].x_speed > 0)
                shapes[i].x_speed++;
            else if (shapes[i].x_speed < 0)
                shapes[i].x_speed--;
        }
        shapes[i].x_start += shapes[i].x_speed;
        shapes[i].x_end += shapes[i].x_speed;
        shapes[i].y_start += shapes[i].y_speed;
        shapes[i].y_end += shapes[i].y_speed;
    }
    return ret;
}

void process_movement(geometrical_4axis *shapes, size_t shape_size, int border_size, struct window_meta meta)
{
    for (int i = 0; i < shape_size; i++)
    {
        if (shapes[i].x_start <= border_size || shapes[i].x_end >= meta.width - border_size)
        {
            shapes[i].x_speed = 0;
        }
        if (shapes[i].y_start <= border_size)
        {
            if (shapes[i].y_speed < 0)
                shapes[i].y_speed = 0;
        }
        else if (shapes[i].y_end >= meta.height - border_size)
        {
            if (shapes[i].y_speed > 0)
                shapes[i].y_speed = 0;
        }
        shapes[i].x_start += shapes[i].x_speed;
        shapes[i].x_end += shapes[i].x_speed;
        shapes[i].y_start += shapes[i].y_speed;
        shapes[i].y_end += shapes[i].y_speed;
    }
}

void keyboard_movement(geometrical_4axis *shape, int pressed_s, int pressed_d)
{
    if (!pressed_d && !pressed_s)
        shape->y_speed = 0;
}

void process_collisions(geometrical_4axis *shapes, geometrical_4axis *shapes_collision, size_t shape_size, size_t collision_size)
{
    for (int i = 0; i < shape_size; i++)
    {
        for (int y = 0; y < collision_size; y++)
        {
            if (shapes[i].x_start <= shapes_collision[y].x_end && shapes[i].x_start >= shapes_collision[y].x_start)
            {
                if (shapes[i].y_start <= shapes_collision[y].y_end && shapes[i].y_start >= shapes_collision[y].y_start)
                    shapes[i].x_speed *= -1;
            }
            else if (shapes[i].x_end >= shapes_collision[y].x_start && shapes[i].x_end <= shapes_collision[y].x_end)
            {
                if (shapes[i].y_start <= shapes_collision[y].y_end && shapes[i].y_start >= shapes_collision[y].y_start)
                    shapes[i].x_speed *= -1;
            }
            if (shapes[i].y_start <= shapes_collision[y].y_end && shapes[i].y_start >= shapes_collision[y].y_start)
            {
                if (shapes[i].x_start <= shapes_collision[y].x_end && shapes[i].x_start >= shapes_collision[y].x_start)
                {
                    if ((shapes[i].x_end - shapes_collision[y].y_start) > (shapes_collision[y].y_end - shapes_collision[y].y_start)/2)
                        shapes[i].x_speed *= -1;
                }
            }
            else if (shapes[i].y_end >= shapes_collision[y].y_start && shapes[i].y_end <= shapes_collision[y].y_end)
            {
                if (shapes[i].x_start <= shapes_collision[y].x_end && shapes[i].x_start >= shapes_collision[y].x_start)
                    shapes[i].x_speed *= -1;
            }
        }
    }
}