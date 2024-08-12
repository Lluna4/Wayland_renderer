#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "xdg-shell-client-protocol.h"
//#include "png_lib.h"
#include "time.h"

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
    char allign;
    int x_speed;
    int y_speed;
}geometrical_4axis;


struct wl_compositor *compositor;
struct wl_surface *surface;
struct wl_buffer *frame_buff;
struct wl_shm *shared_memory;
struct xdg_wm_base *shell;
struct xdg_toplevel *toplevel;


unsigned char *pixel;
int width = 1300;
int height = 1300;
uint8_t c = 0;
uint8_t cl = 0;
//unsigned char **img;
//struct metadata meta;
geometrical_4axis *to_draw;
int to_draw_size = 0;
int sum = 0;
int x_increase = 0;
int first = 1;

void prepare_pixels()
{
    int index = 0;
    int pxl_index = 0;
    for (int y = 0; y < height;y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char r = 0;
            unsigned char g = 0;
            unsigned char b = 0;
            pixel[pxl_index] = r;
            pxl_index++;
            pixel[pxl_index] = g;
            pxl_index++;
            pixel[pxl_index] = b;
            pxl_index++;
            pixel[pxl_index] = 255;
            pxl_index++;
        }
    }
}

void render_forms()
{
    int pxl_index = 0;
    geometrical_4axis buff;
    for (int i = 0; i < to_draw_size; i++)
    {
        buff = to_draw[i];
        if (to_draw[i].allign == 1)
        {  
            buff.x_start = width - to_draw[i].x_start;
            buff.x_end = width - to_draw[i].x_end;
        }
        if (to_draw[i].allign == 2)
        {  
            buff.y_start = width - to_draw[i].y_start;
            buff.y_end = width - to_draw[i].y_end;
        }
        if (to_draw[i].allign == 3)
        {  
            buff.y_start = height/2 - ((to_draw[i].y_end - to_draw[i].y_start)/2);
            buff.y_end = height/2 + ((to_draw[i].y_end - to_draw[i].y_start)/2);
        }
        if (to_draw[i].allign == 4)
        {  
            buff.y_start = height/2 - ((to_draw[i].y_end - to_draw[i].y_start)/2);
            buff.y_end = height/2 + ((to_draw[i].y_end - to_draw[i].y_start)/2);
            buff.x_start = width - to_draw[i].x_start;
            buff.x_end = width - to_draw[i].x_end;
        }
        if (to_draw[i].allign == 5)
        {  
            buff.y_start = height/2 - ((to_draw[i].y_end - to_draw[i].y_start)/2);
            buff.y_end = height/2 + ((to_draw[i].y_end - to_draw[i].y_start)/2);
            buff.x_start = width/2 - ((to_draw[i].x_end - to_draw[i].x_start)/2);
            buff.x_end = width/2 + ((to_draw[i].x_end - to_draw[i].x_start)/2);
        }
        for (int y = buff.y_start; y < buff.y_end;y++)
        {
            pxl_index = (buff.x_start + width * y) * 4;
            for (int x = buff.x_start; x < buff.x_end; x++)
            {
                unsigned char r = buff.color.r;
                unsigned char g = buff.color.g;
                unsigned char b = buff.color.b;
                pixel[pxl_index] = r;
                pxl_index++;
                pixel[pxl_index] = g;
                pxl_index++;
                pixel[pxl_index] = b;
                pxl_index++;
                pixel[pxl_index] = 255;
                pxl_index++;
            }
        }
    }
}

void clear_forms()
{
    int pxl_index = 0;
    for (int i = 0; i < to_draw_size; i++)
    {
        for (int y = to_draw[i].y_start; y < to_draw[i].y_end;y++)
        {
            pxl_index = (to_draw[i].x_start + width * y) * 4;
            for (int x = to_draw[i].x_start; x < to_draw[i].x_end; x++)
            {
                unsigned char r = 0;
                unsigned char g = 0;
                unsigned char b = 0;
                pixel[pxl_index] = r;
                pxl_index++;
                pixel[pxl_index] = g;
                pxl_index++;
                pixel[pxl_index] = b;
                pxl_index++;
                pixel[pxl_index] = 255;
                pxl_index++;
            }
        }
    }
}

void process_movement()
{
    for (int i = 0; i < to_draw_size; i++)
    {
        if (to_draw[i].y_end >= height - 20)
            to_draw[i].y_speed = to_draw[i].y_speed * -1;
        if (to_draw[i].y_start <= 0 + 20)
            to_draw[i].y_speed = to_draw[i].y_speed * -1;
        if (to_draw[i].x_end >= width - 20)
            to_draw[i].x_speed = to_draw[i].x_speed * -1;
        if (to_draw[i].x_start <= 0 + 20)
            to_draw[i].x_speed = to_draw[i].x_speed * -1;
        to_draw[i].y_start += to_draw[i].y_speed;
        to_draw[i].y_end += to_draw[i].y_speed;
        to_draw[i].x_start += to_draw[i].x_speed;
        to_draw[i].x_end += to_draw[i].x_speed;
        if (to_draw[i].x_speed != 0 || to_draw[i].y_speed != 0)
            to_draw[i].allign = 0;
    }
}

void check_collisions()
{
    geometrical_4axis buff;
    for (int i = 0; i < to_draw_size; i++)
    {
        for (int y = 0; y < to_draw_size; y++)
        {
            buff = to_draw[y];
            if (to_draw[y].allign == 1)
            {  
                buff.x_start = width - to_draw[y].x_start;
                buff.x_end = width - to_draw[y].x_end;
            }
            if (to_draw[y].allign == 2)
            {  
                buff.y_start = width - to_draw[y].y_start;
                buff.y_end = width - to_draw[y].y_end;
            }
            if (to_draw[y].allign == 3)
            {  
                buff.y_start = height/2 - ((to_draw[y].y_end - to_draw[y].y_start)/2);
                buff.y_end = height/2 + ((to_draw[y].y_end - to_draw[y].y_start)/2);
            }
            if (to_draw[y].allign == 4)
            {  
                buff.y_start = height/2 - ((to_draw[y].y_end - to_draw[y].y_start)/2);
                buff.y_end = height/2 + ((to_draw[y].y_end - to_draw[y].y_start)/2);
                buff.x_start = width - to_draw[y].x_start;
                buff.x_end = width - to_draw[y].x_end;
            }
            if (to_draw[y].allign == 5)
            {  
                buff.y_start = height/2 - ((to_draw[y].y_end - to_draw[y].y_start)/2);
                buff.y_end = height/2 + ((to_draw[y].y_end - to_draw[y].y_start)/2);
                buff.x_start = width/2 - ((to_draw[y].x_end - to_draw[y].x_start)/2);
                buff.x_end = width/2 + ((to_draw[y].x_end - to_draw[y].x_start)/2);
            }
            if (i != y)
            {
                if (to_draw[i].x_start <= buff.x_end && to_draw[i].x_start >= buff.x_start)
                {
                    to_draw[i].x_speed *= -1;
                }
                else if (to_draw[i].x_end >= buff.x_start && to_draw[i].x_end <= buff.x_end)
                {
                    to_draw[i].x_speed *= -1;
                }
            }
        }
    }
}

int32_t allocate_shared_memory(uint64_t size)
{
    int8_t *name = calloc(9, sizeof(int8_t));
    name[0] = '/';
    name[7] = 0;
    for (int i = 1; i < 6; i++)
        name[i] = 'a' + i;
    
    int32_t fd = shm_open(name, O_RDWR | O_EXCL | O_CREAT, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
    shm_unlink(name);
    ftruncate(fd, size);
    free(name);
    return fd;
}

void resize()
{
    int32_t fd = allocate_shared_memory(width * height * 4);
    
    pixel = mmap(0, width * height * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    struct wl_shm_pool *pool = wl_shm_create_pool(shared_memory, fd, width * height * 4);
    frame_buff = wl_shm_pool_create_buffer(pool, 0, width, height, width * 4, WL_SHM_FORMAT_ABGR8888);
    wl_shm_pool_destroy(pool);
    prepare_pixels();
    render_forms();
    if (first == 1)
    {
        to_draw[2].y_start = height/2 - ((to_draw[2].y_end - to_draw[2].y_start)/2);
        to_draw[2].y_end = height/2 + ((to_draw[2].y_end - to_draw[2].y_start)/2);
        to_draw[2].x_start = width/2 - ((to_draw[2].x_end - to_draw[2].x_start)/2);
        to_draw[2].x_end = width/2 + ((to_draw[2].x_end - to_draw[2].x_start)/2);
        printf("%i, %i, %i, %i\n", to_draw[2].x_start, to_draw[2].x_end, to_draw[2].y_start, to_draw[2].y_end);
        first = 0;
    }
    close(fd);
}

void draw()
{
    wl_surface_attach(surface, frame_buff, 0, 0);
    wl_surface_damage_buffer(surface, 0, 0, width, height);
    wl_surface_commit(surface);
}

struct wl_callback_listener callback_listener;



void render_frame(void *data, struct wl_callback *callback, unsigned int callback_data)
{
    clock_t begin = clock();
    wl_callback_destroy(callback);
    callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &callback_listener, 0);
    clear_forms();
    check_collisions();
    process_movement();
    render_forms();
    draw();
    clock_t end = clock();
    double time_spent = (double)(end - begin);
    printf("Frame time: %fms\n", time_spent/1000);
}

struct wl_callback_listener callback_listener = {.done = render_frame};

void shell_ping(void *data, struct xdg_wm_base *sh, unsigned int serial)
{
    xdg_wm_base_pong(sh, serial);
}

void toplevel_configure(void *data, struct xdg_toplevel *toplevel, int nwidth, int nheight, struct wl_array *states)
{
    if (!nwidth && !nheight)
        return;
    
    if (height != nheight || width != nwidth)
    {
        munmap(pixel, height * width * 4);
        height = nheight;
        width = nwidth;
        resize();
    }
}
void toplevel_close(void *data,struct xdg_toplevel *xdg_toplevel)
{
    cl = 1;
}

void toplevel_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height)
{
    
}

void toplevel_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities)
{
    
}

void surface_xdg_configure(void *data, struct xdg_surface *surface_xdg, unsigned int serial)
{
    xdg_surface_ack_configure(surface_xdg, serial);
    if (!pixel)
        resize();
    draw();
}

struct xdg_surface_listener surface_xdg_listener = {surface_xdg_configure};
struct xdg_toplevel_listener toplevel_listener = {.configure = toplevel_configure, .close = toplevel_close, .configure_bounds = toplevel_configure_bounds,
    .wm_capabilities = toplevel_wm_capabilities};
struct xdg_wm_base_listener shell_listener = {.ping = shell_ping};

void reg_global(void *data, struct wl_registry *wl_registry, uint32_t name, const char *interface, uint32_t version)
{
    if (strcmp(interface, wl_compositor_interface.name) == 0)
    {
        compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
        printf("Compositor binded!\n");
    }
    else if (strcmp(interface, wl_shm_interface.name) == 0)
    {
        shared_memory = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
    {
        shell = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(shell, &shell_listener, 0);
    }
}
void reg_global_remove(void *data, struct wl_registry *wl_registry, uint32_t nam)
{
    
}
struct wl_registry_listener listener = {.global = reg_global, .global_remove = reg_global_remove};




int main()
{
    struct wl_display *disp = wl_display_connect(0);
    if (!disp)
        return -1;
    printf("Display connected!\n");
    
    struct wl_registry *reg = wl_display_get_registry(disp);
    wl_registry_add_listener(reg, &listener, 0);
    wl_display_roundtrip(disp);
    surface = wl_compositor_create_surface(compositor);
    struct wl_callback *callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &callback_listener, 0);
    
    struct xdg_surface *surface_xdg = xdg_wm_base_get_xdg_surface(shell, surface);
    xdg_surface_add_listener(surface_xdg, &surface_xdg_listener, 0);
    toplevel = xdg_surface_get_toplevel(surface_xdg);
    xdg_toplevel_add_listener(toplevel, &toplevel_listener, 0);
    xdg_toplevel_set_title(toplevel, "AAAAAAAAAAAA");
    wl_surface_commit(surface);
    to_draw = calloc(4, sizeof(geometrical_4axis));
    geometrical_4axis a = {100, 250, 30, 50, {255, 255, 255, 255}, 3, 0, 0};
    geometrical_4axis b = {100, 250, 50, 30, {255, 255, 255, 255}, 4, 0, 0};
    geometrical_4axis c = {500, 530, 500, 510, {255, 255, 255, 255}, 6, 1, 0};
    to_draw[0] = a;
    to_draw[1] = b;
    to_draw[2] = c;
    to_draw_size = 3;
    
    
    while (wl_display_dispatch(disp))
    {
        if (cl != 0)
            break;
    }
    
    if (frame_buff)
    {
        wl_buffer_destroy(frame_buff);
    }
    xdg_toplevel_destroy(toplevel);
    xdg_surface_destroy(surface_xdg);
    wl_surface_destroy(surface);
    wl_display_disconnect(disp);
    return 0;
}