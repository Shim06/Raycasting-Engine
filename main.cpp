#include "main.h"

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);
    SDL_Window* window = SDL_CreateWindow("Raycasting Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          screen_width, screen_height, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 1;
    }
    SDL_SetWindowMinimumSize(window, 640, 480);
    SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);

    //player_speed = player_speed / (map_scale * cell_size);
    bool done = false;
    player.pos.x = 2;
    player.pos.y = 2;
    while (!done)
    {
        old_time = SDL_GetTicks();
        if (handle_input(window) == -1) done = true;

        // Main Code
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawMap(renderer);
        handle_player_movement();
        drawPlayer(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        int end_x = int(player.global_pos.x + (player.direction.x * 15));
        int end_y = int(player.global_pos.y + (player.direction.y * 15));
        SDL_RenderDrawPoint(renderer, end_x, end_y);
        SDL_RenderDrawLine(renderer, player.global_pos.x, player.global_pos.y, end_x, end_y);

        for (int16_t x = 512; x < screen_width; x++)
        {
            double camera_x = 2 * x / double(screen_width) - 1;
            vector2D ray_direction;
            ray_direction.x = player.direction.x + plane.x * camera_x;
            ray_direction.y = player.direction.y + plane.y * camera_x;

            //which box of the map the player is in
            int map_x = int(player.pos.x);
            int map_y = int(player.pos.y);

            //length of ray from current position to next x or y-side
            vector2D side_dist;

            vector2D delta_dist;
            delta_dist.x = std::abs(1 / ray_direction.x);
            delta_dist.y = std::abs(1 / ray_direction.y);
            double perp_wall_dist;

            //what direction to step in x or y-direction (either +1 or -1)
            int step_x;
            int step_y;

            int hit = 0; //was there a wall hit?
            int side; //was a NS or a EW wall hit?

            if (ray_direction.x < 0)
            {

            }
            else
            {

            }
        }
        SDL_RenderPresent(renderer);

        current_time = SDL_GetTicks();
        frame_time = (current_time - old_time) / 1000.0;
        //std::cout << "FPS: " << 1.0 / frame_time << "\n";
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}



int handle_input(SDL_Window*& window)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            return -1;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            return -1;

        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_a:
                player_controls.move_left = true;
                break;

            case SDLK_d:
                player_controls.move_right = true;
                break;

            case SDLK_w:
                player_controls.move_up = true;
                break;

            case SDLK_s:
                player_controls.move_down = true;
                break;

            default: break;
            }
        }

        if (event.type == SDL_KEYUP)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_a:
                player_controls.move_left = false;
                break;

            case SDLK_d:
                player_controls.move_right = false;
                break;

            case SDLK_w:
                player_controls.move_up = false;
                break;

            case SDLK_s:
                player_controls.move_down = false;
                break;

            default: break;
            }
        }
    }
    return 0;
}

void drawMap(SDL_Renderer *& renderer)
{
    uint16_t x, y, pos_x, pos_y;
    for (y = 0; y < map_height; y++)
    {
        for (x = 0; x < map_width; x++)
        {
            if (map[y][x])
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            pos_x = int(x * cell_size * map_scale_x);
            pos_y = int(y * cell_size * map_scale_y);
            SDL_Rect rect;
            rect.x = pos_x + 1;
            rect.y = pos_y + 1;
            rect.w = int(cell_size * map_scale_x) - 1;
            rect.h = int(cell_size * map_scale_y) - 1;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void drawPlayer(SDL_Renderer*& renderer)
{
    static uint8_t player_width = cell_size * map_scale_x * 0.5;
    static uint8_t player_height = cell_size * map_scale_y * 0.5;
    player.global_pos.x = ((player.pos.x * cell_size * map_scale_x) + player_width / 2);
    player.global_pos.y = ((player.pos.y * cell_size * map_scale_y) + player_height / 2);
    SDL_Rect rect;
    rect.x = static_cast<int>(player.pos.x * cell_size * map_scale_x);
    rect.y = static_cast<int>(player.pos.y * cell_size * map_scale_y);
    rect.w = player_width;
    rect.h = player_height;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void handle_player_movement()
{
    double move_speed = player_speed * frame_time;
    double rotation_speed = player_rotation_speed * frame_time;
    if (player_controls.move_up)
    {
        double location_x = player.pos.x + (move_speed * player.direction.x);
        double location_y = player.pos.y + (move_speed * player.direction.y);
        std::cout << "x: " << (int)location_x << " " << "y: " << (int)location_y << "\n";
        if (map[(int)player.pos.y][(int)location_x] == false) player.pos.x = location_x;
        if (map[(int)location_y][(int)player.pos.x] == false) player.pos.y = location_y;
    }
    if (player_controls.move_down)
    {
        std::cout << (player.global_pos.x / (cell_size * map_scale_x)) << "\n";
        double location_x = player.pos.x - (move_speed * player.direction.x);
        double location_y = player.pos.y - (move_speed * player.direction.y);
        if (map[(int)player.pos.y][(int)location_x] == false) player.pos.x = location_x;
        if (map[(int)location_y][(int)player.pos.x] == false) player.pos.y = location_y;
    }


    if (player_controls.move_left)
    {
        double old_direction_x = player.direction.x;
        player.direction.x = player.direction.x * std::cos(-rotation_speed) - player.direction.y * std::sin(-rotation_speed);
        player.direction.y = old_direction_x * std::sin(-rotation_speed) + player.direction.y * std::cos(-rotation_speed);

        double old_plane_x = plane.x;
        plane.x = plane.x * std::cos(-rotation_speed) - plane.y * std::sin(-rotation_speed);
        plane.y = plane.x * std::sin(-rotation_speed) + plane.y * std::cos(-rotation_speed);
    }
    if (player_controls.move_right)
    {
        double old_direction_x = player.direction.x;
        player.direction.x = player.direction.x * std::cos(rotation_speed) - player.direction.y * std::sin(rotation_speed);
        player.direction.y = old_direction_x * std::sin(rotation_speed) + player.direction.y * std::cos(rotation_speed);

        double old_plane_x = plane.x;
        plane.x = plane.x * std::cos(rotation_speed) - plane.y * std::sin(rotation_speed);
        plane.y = old_plane_x * std::sin(rotation_speed) + plane.y * std::cos(rotation_speed);
    }
}