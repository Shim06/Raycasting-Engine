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
    SDL_SetWindowMinimumSize(window, screen_width, screen_height);
    SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);
    SDL_SetRelativeMouseMode(SDL_TRUE);
  
#define texture_count 14
    texture.resize(texture_count);

    // Load wall textures
    texture[0] = IMG_Load("assets\\textures\\wall_ship_0.png");
    texture[1] = IMG_Load("assets\\textures\\wall_ship_1.png");
    texture[2] = IMG_Load("assets\\textures\\wall_ship_2.png");
    texture[3] = IMG_Load("assets\\textures\\wall_ship_3.png");
    texture[4] = IMG_Load("assets\\textures\\wall_ship_4.png");
    texture[5] = IMG_Load("assets\\textures\\wall_ship_5.png");
    texture[6] = IMG_Load("assets\\textures\\wall_ship_6.png");
    texture[7] = IMG_Load("assets\\textures\\wall_ship_7.png");
    texture[8] = IMG_Load("assets\\textures\\floor_desert.png");
    texture[9] = IMG_Load("assets\\textures\\ceiling.png");
    texture[10] = IMG_Load("assets\\textures\\well.png");
    texture[11] = IMG_Load("assets\\textures\\barrel.png");
    texture[12] = IMG_Load("assets\\textures\\obelisk.png");
    texture[13] = IMG_Load("assets\\textures\\altar.png");

    for (size_t i = 0; i < texture.size(); i++) 
    {
        if (texture[i] == nullptr) 
        {
            std::cout << ("Error: Failed to load texture %zu: %s\n", i, IMG_GetError());
            return -1;
        }
    }

    // Load sprite textures
    SDL_Texture* frame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
    if (!frame) {
        std::cout << "Unable to create texture! SDL_Error: " << SDL_GetError();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool done = false;
    player.pos.x = 1.2;
    player.pos.y = 1.2;

    // Main Loop
    while (!done)
    {
        if (handle_input(window) == -1) done = true;
        handle_player_movement();

        // Main Code
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Rendering
        raycastFloorAndCeiling();
        raycastWalls();
        raycastSprites();
        
        SDL_UpdateTexture(frame, nullptr, frame_buffer.data(), (screen_width) * sizeof(uint32_t));
        SDL_RenderCopy(renderer, frame, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        current_time = SDL_GetTicks();
        frame_time = (current_time - old_time) / 1000.0;
        old_time = current_time;
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

void raycastFloorAndCeiling()
{
    // Floor and Ceilling rendering
    for (int16_t y = 0; y < screen_height; y++)
    {
        vector2D ray0_direction;
        vector2D ray1_direction;
        ray0_direction.x = player.direction.x - plane.x;
        ray0_direction.y = player.direction.y - plane.y;
        ray1_direction.x = player.direction.x + plane.x;
        ray1_direction.y = player.direction.y + plane.y;

        float center_position = screen_height / 2;
        int ray_position = y - static_cast<int>(center_position);
        float row_distance = center_position / ray_position;

        float floor_step_x = row_distance * static_cast<float>(ray1_direction.x - ray0_direction.x) / (screen_width);
        float floor_step_y = row_distance * static_cast<float>(ray1_direction.y - ray0_direction.y) / (screen_width);

        float floor_x = static_cast<float>(player.pos.x) + row_distance * static_cast<float>(ray0_direction.x);
        float floor_y = static_cast<float>(player.pos.y) + row_distance * static_cast<float>(ray0_direction.y);

        for (uint16_t x = 0; x < screen_width; x++)
        {
            int cell_x = (int)floor_x;
            int cell_y = (int)floor_y;

            int texture_x = (int)(texture_width * (floor_x - cell_x)) & (texture_width - 1);
            int texture_y = (int)(texture_height * (floor_y - cell_y)) & (texture_height - 1);

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            const int floor_texture = 8;
            const int ceiling_texture = 9;

            // Floor rendering
            data = getPixel(texture[floor_texture], texture_x, texture_y);
            SDL_GetRGB(data, texture[floor_texture]->format, &rgb.r, &rgb.g, &rgb.b);
            rgb.r >>= 1; rgb.g >>= 1; rgb.b >>= 1;
            color = rgb.r << 24 | rgb.g << 16 | rgb.b << 8 | 255;
            frame_buffer[y * screen_width + x] = color;

            // Ceiling rendering
            data = getPixel(texture[ceiling_texture], texture_x, texture_y);
            SDL_GetRGB(data, texture[ceiling_texture]->format, &rgb.r, &rgb.g, &rgb.b);
            rgb.r >>= 1; rgb.g >>= 1; rgb.b >>= 1;
            color = rgb.r << 24 | rgb.g << 16 | rgb.b << 8 | 255;
            frame_buffer[((screen_height - 1 - y) * screen_width) + x] = color;
        }
    }
}

void raycastWalls()
{
    for (uint16_t x = 0; x < screen_width; x++)
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

            if (map[map_x][map_y] > 0) hit = 1;
        }

        if (side == 0) perp_wall_dist = (side_dist.x - delta_dist.x);
        else perp_wall_dist = (side_dist.y - delta_dist.y);
        // Used for sprite casting
        z_buffer[x] = perp_wall_dist;

        // Calculate the height of the line to draw on the screen
        int line_height = (int)(screen_height / perp_wall_dist);

        //calculate lowest and highest pixel to fill in current stripe
        int draw_start = -line_height / 2 + screen_height / 2;
        if (draw_start < 0) draw_start = 0;

        int draw_end = line_height / 2 + screen_height / 2;
        if (draw_end >= screen_height) draw_end = screen_height - 1;

        int texture_number = map[map_x][map_y] - 1;
        double wall_x;
        if (side == 0) wall_x = player.pos.y + perp_wall_dist * ray_direction.y;
        else           wall_x = player.pos.x + perp_wall_dist * ray_direction.x;
        wall_x -= floor(wall_x);

        int texture_x = int(wall_x * double(texture_width));
        if (side == 0 && ray_direction.x > 0) texture_x = texture_width - texture_x - 1;
        if (side == 1 && ray_direction.y < 0) texture_x = texture_width - texture_x - 1;

        double step = 1.0 * texture_height / line_height;
        double tex_pos = (draw_start - screen_height / 2 + line_height / 2) * step;
        for (int y = draw_start; y < draw_end; y++)
        {
            // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
            int texture_y = (int)tex_pos & (texture_height - 1);
            tex_pos += step;

            data = getPixel(texture[texture_number], texture_x, texture_y);
            SDL_GetRGB(data, texture[texture_number]->format, &rgb.r, &rgb.g, &rgb.b);
            if (side == 1) { rgb.r >>= 1; rgb.g >>= 1; rgb.b >>= 1; }
            color = rgb.r << 24 | rgb.g << 16 | rgb.b << 8 | 255;
            frame_buffer[y * screen_width + x] = color;
        }
    }
}

void raycastSprites()
{
    for (uint16_t i = 0; i < sprite_count; i++)
    {
        sprite_order[i] = i;
        sprite_distance[i] = (player.pos.x - sprites[i].pos.x) * (player.pos.x - sprites[i].pos.x)
            + (player.pos.y - sprites[i].pos.y) * (player.pos.y - sprites[i].pos.y);
    }
    sort_sprites(sprite_order, sprite_distance, sprite_count);

    for (uint16_t i = 0; i < sprite_count; i++)
    {
        double sprite_x = sprites[sprite_order[i]].pos.x - player.pos.x;
        double sprite_y = sprites[sprite_order[i]].pos.y - player.pos.y;

        //transform sprite with the inverse camera matrix
        // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
        // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
        // [ planeY   dirY ]                                          [ -planeY  planeX ]

        double inverse = 1.0 / (plane.x * player.direction.y - player.direction.x * plane.y);

        double transform_x = inverse * (player.direction.y * sprite_x - player.direction.x * sprite_y);
        double transform_y = inverse * (-plane.y * sprite_x + plane.x * sprite_y);

        int sprite_screen_x = int((screen_width / 2) * (1 + transform_x / transform_y));
        int sprite_height = abs(int(screen_height / transform_y));

        int draw_start_y = -sprite_height / 2 + screen_height / 2;
        int draw_end_y = sprite_height / 2 + screen_height / 2;
        if (draw_start_y < 0) draw_start_y = 0;
        if (draw_end_y > screen_height) draw_end_y = screen_height - 1;

        int sprite_width = abs(int(screen_height / transform_y));
        int draw_start_x = -sprite_width / 2 + sprite_screen_x;
        int draw_end_x = sprite_width / 2 + sprite_screen_x;
        if (draw_start_x < 0) draw_start_x = 0;
        if (draw_end_x > screen_width) draw_end_x = screen_width - 1;

        // Loop through every vertical stripe of the sprite
        for (uint16_t stripe = draw_start_x; stripe < draw_end_x; stripe++)
        {
            int texture_x = int(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) * texture_width / sprite_width) / 256;
            if (transform_y > 0 && stripe > 0 && stripe < screen_width && transform_y < z_buffer[stripe])
            {
                for (uint16_t y = draw_start_y; y < draw_end_y; y++)
                {
                    int d = (y) * 256 - screen_height * 128 + sprite_height * 128;
                    int texture_y = ((d * texture_height) / sprite_height) / 256;

                    data = getPixel(texture[sprites[sprite_order[i]].texture], texture_x, texture_y);
                    SDL_GetRGB(data, texture[sprites[sprite_order[i]].texture]->format, &rgb.r, &rgb.g, &rgb.b);
                    color = rgb.r << 24 | rgb.g << 16 | rgb.b << 8 | 255;
                    if (rgb.r != 0 && rgb.g != 0 && rgb.b != 0)
                        frame_buffer[y * screen_width + stripe] = color;
                }
            }
        }
    }
}

void sort_sprites(int* order, double* dist, int amount)
{
    std::vector<std::pair<double, int>> sprites(amount);
    for (uint16_t i = 0; i < amount; i++)
    {
        sprites[i].first = dist[i];
        sprites[i].second = order[i];
    }
    std::sort(sprites.begin(), sprites.end());
    for (uint16_t i = 0; i < amount; i++)
    {
        dist[i] = sprites[amount - i - 1].first;
        order[i] = sprites[amount - i - 1].second;
    }
}

void handle_player_movement()
{
    int motion_x, motion_y;
    SDL_GetRelativeMouseState(&motion_x, &motion_y);
    double move_speed = player.speed * frame_time;
    if (player_controls.move_up)
    {
        double location_x = player.pos.x + (move_speed * player.direction.x);
        double location_y = player.pos.y + (move_speed * player.direction.y);
        if (map[(int)location_x][(int)player.pos.y] == false) player.pos.x = location_x;
        if (map[(int)player.pos.x][(int)location_y] == false) player.pos.y = location_y;
    }
    if (player_controls.move_down)
    {
        double location_x = player.pos.x - (move_speed * player.direction.x);
        double location_y = player.pos.y - (move_speed * player.direction.y);
        if (map[(int)location_x][(int)player.pos.y] == false) player.pos.x = location_x;
        if (map[(int)player.pos.x][(int)location_y] == false) player.pos.y = location_y;
    }
    if (player_controls.move_left)
    {
        double strafe_x = player.direction.y;
        double strafe_y = -player.direction.x;
        double location_x = player.pos.x - (move_speed * strafe_x);
        double location_y = player.pos.y - (move_speed * strafe_y);
        if (map[(int)location_x][(int)player.pos.y] == false) player.pos.x = location_x;
        if (map[(int)player.pos.x][(int)location_y] == false) player.pos.y = location_y;
    }
    if (player_controls.move_right)
    {
        double strafe_x = player.direction.y;
        double strafe_y = -player.direction.x;
        double location_x = player.pos.x + (move_speed * strafe_x);
        double location_y = player.pos.y + (move_speed * strafe_y);
        if (map[(int)location_x][(int)player.pos.y] == false) player.pos.x = location_x;
        if (map[(int)player.pos.x][(int)location_y] == false) player.pos.y = location_y;
    }

    if (motion_x != 0)
    {
        double rotation_speed = (motion_x * (player_controls.mouse_sensitivity / 800)) / 100;
        double old_direction_x = player.direction.x;
        player.direction.x = player.direction.x * std::cos(-rotation_speed) - player.direction.y * std::sin(-rotation_speed);
        player.direction.y = old_direction_x * std::sin(-rotation_speed) + player.direction.y * std::cos(-rotation_speed);

        double old_plane_x = plane.x;
        plane.x = plane.x * std::cos(-rotation_speed) - plane.y * std::sin(-rotation_speed);
        plane.y = old_plane_x * std::sin(-rotation_speed) + plane.y * std::cos(-rotation_speed);
    }
}

Uint32 getPixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16*)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32*)p;
        break;

    default:
        return 0;
    }
}
