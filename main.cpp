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
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }
    SDL_SetWindowMinimumSize(window, 640, 480);
    SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    std::vector<SDL_Texture*> texture;
    texture.resize(1);
    texture[0] = IMG_LoadTexture(renderer, "assets\\textures\\wall_ship_0.png");


    //player_speed = player_speed / (map_scale * cell_size);
    bool done = false;
    player.pos.x = 1;
    player.pos.y = 1;
    while (!done)
    {
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

        for (int16_t x = screen_width / 2; x < screen_width; x++)
        {
            double camera_width = x - (screen_width / 2);
            double camera_x = 2 * camera_width / double(screen_width / 2) - 1;
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
                step_x = -1;
                side_dist.x = (player.pos.x - map_x) * delta_dist.x;
            }
            else
            {
                step_x = 1;
                side_dist.x = (map_x + 1.0 - player.pos.x) * delta_dist.x;
            }
            if (ray_direction.y < 0)
            {
                step_y = -1;
                side_dist.y = (player.pos.y - map_y) * delta_dist.y;
            }
            else
            {
                step_y = 1;
                side_dist.y = (map_y + 1.0 - player.pos.y) * delta_dist.y;
            }

            while (hit == 0)
            {
                if (side_dist.x < side_dist.y)
                {
                    side_dist.x += delta_dist.x;
                    map_x += step_x;
                    side = 0;
                }
                else
                {
                    side_dist.y += delta_dist.y;
                    map_y += step_y;
                    side = 1;
                }

                if (map[map_y][map_x] > 0) hit = 1;
            }

            if (side == 0) perp_wall_dist = (side_dist.x - delta_dist.x);
            else perp_wall_dist = (side_dist.y - delta_dist.y);

            // Calculate the height of the line to draw on the screen
            int line_height = (int)(screen_height / perp_wall_dist);

            color blue = { 0, 0, 255, 255 };
            if (side == 1) { blue.b /= 1.7; }
            switch (map[map_y][map_x])
            {
            case 1:
                SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, 255);
                break;

            case 2:
                SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, 255);
                break;

            case 3:
                SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, 255);
                break;

            case 4:
                SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, 255);
                break;

            case 5:
                SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, 255);
                break;

            case 6:
                SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, 255);
                break;

            case 7:
                SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, 255);
                break;

            case 8:
                SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, 255);
                break;
            }

            //calculate lowest and highest pixel to fill in current stripe
            int draw_start = -line_height / 2 + screen_height / 2;
            if (draw_start < 0) draw_start = 0;

            int draw_end = line_height / 2 + screen_height / 2;
            if (draw_end >= screen_height) draw_end = screen_height - 1;

            SDL_RenderDrawLine(renderer, x, draw_start, x, draw_end);
        }

        SDL_Rect destRect = { 100, 100, 64, 64 };
        SDL_RenderCopy(renderer, texture[0], nullptr, &destRect);
        SDL_RenderPresent(renderer);

        current_time = SDL_GetTicks();
        frame_time = (current_time - old_time) / 1000.0;
        old_time = current_time;
        std::cout << "FPS: " << 1.0 / frame_time << "\n";
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

            case SDLK_ESCAPE:
                return -1;
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
    if (map_scale_x <= 0) map_scale_x = 0.5;
    if (map_scale_y <= 0) map_scale_y = 0.1;
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
    int motion_x, motion_y;
    SDL_GetRelativeMouseState(&motion_x, &motion_y);
    double move_speed = player_speed * frame_time;
    if (player_controls.move_up)
    {
        double location_x = player.pos.x + (move_speed * player.direction.x);
        double location_y = player.pos.y + (move_speed * player.direction.y);
        if (map[(int)player.pos.y][(int)location_x] == false) player.pos.x = location_x;
        if (map[(int)location_y][(int)player.pos.x] == false) player.pos.y = location_y;
    }
    if (player_controls.move_down)
    {
        double location_x = player.pos.x - (move_speed * player.direction.x);
        double location_y = player.pos.y - (move_speed * player.direction.y);
        if (map[(int)player.pos.y][(int)location_x] == false) player.pos.x = location_x;
        if (map[(int)location_y][(int)player.pos.x] == false) player.pos.y = location_y;
    }
    if (player_controls.move_left)
    {
        double strafe_x = player.direction.y;
        double strafe_y = -player.direction.x;
        double location_x = player.pos.x - (move_speed * strafe_x);
        double location_y = player.pos.y - (move_speed * strafe_y);
        if (map[(int)player.pos.y][(int)location_x] == false) player.pos.x = location_x;
        if (map[(int)location_y][(int)player.pos.x] == false) player.pos.y = location_y;
    }
    if (player_controls.move_right)
    {
        double strafe_x = player.direction.y;
        double strafe_y = -player.direction.x;
        double location_x = player.pos.x + (move_speed * strafe_x);
        double location_y = player.pos.y + (move_speed * strafe_y);
        if (map[(int)player.pos.y][(int)location_x] == false) player.pos.x = location_x;
        if (map[(int)location_y][(int)player.pos.x] == false) player.pos.y = location_y;
    }

    if (motion_x != 0)
    {
        double rotation_speed = (motion_x * player_controls.mouse_sensitivity) * frame_time;
        double old_direction_x = player.direction.x;
        player.direction.x = player.direction.x * std::cos(-rotation_speed) - player.direction.y * std::sin(-rotation_speed);
        player.direction.y = old_direction_x * std::sin(-rotation_speed) + player.direction.y * std::cos(-rotation_speed);

        double old_plane_x = plane.x;
        plane.x = plane.x * std::cos(-rotation_speed) - plane.y * std::sin(-rotation_speed);
        plane.y = old_plane_x * std::sin(-rotation_speed) + plane.y * std::cos(-rotation_speed);
    }
}