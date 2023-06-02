#include "util.h"
#include "math.h"

#include "idt.h"

#include "vga.h"

#include "timer.h"
#include "keyboard.h"

#define FPS 60

#define TOTAL_COLORS 16
const color_t palette[TOTAL_COLORS] = {
    0x1a1c2c, // 00 - Black
    0x5d275d, // 01 - Dark red
    0xb13e53, // 02 - Red
    0xef7d57, // 03 - Orange
    0xffcd75, // 04 - Yellow
    0xa7f070, // 05 - Light green
    0x38b764, // 06 - Green
    0x257179, // 07 - Dark green
    0x29366f, // 08 - Dark blue
    0x3b5dc9, // 09 - Blue
    0x41a6f6, // 10 - Light blue
    0x73eff7, // 11 - Lighter blue
    0xf4f4f4, // 12 - White
    0x94b0c2, // 13 - Light gray
    0x566c86, // 14 - Gray
    0x333c57, // 15 - Dark gray
};

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
u8 world_map[MAP_WIDTH][MAP_HEIGHT] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 5, 0, 2, 2, 0, 1},
    {1, 0, 5, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 3, 0, 1},
    {1, 0, 4, 4, 0, 3, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};


struct {
    f64 pos_x, pos_y;
    f64 dir_x, dir_y;
    f64 plane_x, plane_y;

    f64 mov_speed;
    f64 rot_speed;
} player;

// Do all graphics stuff
void render() {
    for(i32 x = 0; x < VGA_WIDTH; x++)
    {
        //calculate ray position and direction
        f64 camera_x = 2 * x / (f64)VGA_HEIGHT - 1; //x-coordinate in camera space
        f64 ray_dir_x = player.dir_x + player.plane_x * camera_x;
        f64 ray_dir_y = player.dir_y + player.plane_y * camera_x;

        //which box of the map we're in
        i32 map_x = (i32)(player.pos_x);
        i32 map_y = (i32)(player.pos_y);

        //length of ray from current position to next x or y-side
        f64 side_dist_x;
        f64 side_dist_y;

        //length of ray from one x or y-side to next x or y-side
        f64 delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1 / ray_dir_x);
        f64 delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1 / ray_dir_y);
        f64 perp_wall_dist;

        //what direction to step in x or y-direction (either +1 or -1)
        i8 step_x;
        i8 step_y;

        //calculate step and initial sideDist
        if (ray_dir_x < 0)
        {
            step_x = -1;
            side_dist_x = (player.pos_x - map_x) * delta_dist_x;
        }
        else
        {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - player.pos_x) * delta_dist_x;
        }
        if (ray_dir_y < 0)
        {
            step_y = -1;
            side_dist_y = (player.pos_y - map_y) * delta_dist_y;
        }
        else
        {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - player.pos_y) * delta_dist_y;
        }

        bool hit = false; // Was there a wall hit?
        u8 side; // Was a NS or a EW wall hit?

        // Perform DDA
        while (hit == 0)
        {
            // Jump to next map square, either in x-direction, or in y-direction
            if (side_dist_x < side_dist_y)
            {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            }
            else
            {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }

            // Check if ray has hit a wall
            if (world_map[map_x][map_y] > 0) hit = true;
        } 

        // Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
        if (side == 0) perp_wall_dist = (side_dist_x - delta_dist_x);
        else           perp_wall_dist = (side_dist_y - delta_dist_y);

        // Calculate height of line to draw on screen
        u16 line_height = (u16)(VGA_HEIGHT / perp_wall_dist);

        // Calculate lowest and highest pixel to fill in current stripe
        i32 draw_start = -line_height / 2 + VGA_HEIGHT / 2;
        if (draw_start < 0) draw_start = 0;

        i32 draw_end = line_height / 2 + VGA_HEIGHT / 2;
        if (draw_end >= VGA_HEIGHT) draw_end = VGA_HEIGHT - 1;

        //choose wall color
        u8 color = world_map[map_x][map_y];

        //draw the pixels of the stripe as a vertical line
        for (i32 i = 0; i < VGA_HEIGHT; i++) {
            u32 index = x + i * VGA_WIDTH;

            if (i < draw_start) { 
                vga_buffer[index] = 9;
            }
            else if (i > draw_end) {
                vga_buffer[index] = 6;
            }
            else {
                vga_buffer[index] = color;
            }
        }
    }
}

// Update game state
void update() {
    // Move forwards
    if (key_is_down(KEY_W))
    {
      player.pos_x += player.dir_x * player.mov_speed;
      player.pos_y += player.dir_y * player.mov_speed;
    }

    // Move backwards
    if (key_is_down(KEY_S))
    {
      player.pos_x -= player.dir_x * player.mov_speed;
      player.pos_y -= player.dir_y * player.mov_speed;
    }

    // Rotate to the right
    if (key_is_down(KEY_D))
    {
        // Rotate camera direction
        f64 old_dir_x = player.dir_x;
        player.dir_x = player.dir_x * cos(-player.rot_speed) - player.dir_y * sin(-player.rot_speed);
        player.dir_y = old_dir_x * sin(-player.rot_speed) + player.dir_y * cos(-player.rot_speed);

        // Rotate camera plane
        f64 oldPlaneX = player.plane_x;
        player.plane_x = player.plane_x * cos(-player.rot_speed) - player.plane_y * sin(-player.rot_speed);
        player.plane_y = oldPlaneX * sin(-player.rot_speed) + player.plane_y * cos(-player.rot_speed);
    }

    // Rotate to the left
    if (key_is_down(KEY_A))
    {
        // Rotate camera direction
        f64 old_dir_x = player.dir_x;
        player.dir_x = player.dir_x * cos(player.rot_speed) - player.dir_y * sin(player.rot_speed);
        player.dir_y = old_dir_x * sin(player.rot_speed) + player.dir_y * cos(player.rot_speed);

        // Rotate camera plane
        f64 oldPlaneX = player.plane_x;
        player.plane_x = player.plane_x * cos(player.rot_speed) - player.plane_y * sin(player.rot_speed);
        player.plane_y = oldPlaneX * sin(player.rot_speed) + player.plane_y * cos(player.rot_speed);
    }
}

void _start() {
    // Setup IDT
    init_idt();

    // Load palette onto VGA hardware
    configure_vga_palette(palette, TOTAL_COLORS);

    // Setup player data
    player.pos_x = 4;
    player.pos_y = 4;

    player.dir_x = -1;
    player.dir_y = 0;

    player.plane_x = 0;
    player.plane_y = 0.66;

    player.mov_speed = 0.05;
    player.rot_speed = 0.05;

    // Setup PIT to trigger [FPS] times per second
    init_timer(FPS);

    // Setup keyboard interrupts
    init_keyboard();

    // Keep track of frames
    u32 last_frame = 0;
    u32 since_last_frame = 0;

    while (true) {
        u32 ticks = timer_get();

        if (ticks > last_frame) {
            // Save the number of ticks since the last frame
            since_last_frame = ticks - last_frame;

            // Main game loop
            update();
            render();

            // Print the number of ticks since the last frame
            // If this is more than one, then the game is lagging
            vga_print_byte(since_last_frame, 1, 1);

            // Print an F in the top right corner every [FPS] frames
            // Should be about once a second
            // If it's not, then the PIT's timing is off
            if (last_frame % FPS == 0) { vga_print_character('F', 5, 7, VGA_WIDTH - 9, 1); }

            // Update frame count
            last_frame = ticks;

            // Swap buffers
            swap();
        }
    }
}