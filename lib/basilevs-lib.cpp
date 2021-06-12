//
// Created by geoco on 21.04.2021.
//
#include <assets.h>
#include <basilevs-lib.h>
namespace basilevs {
    void Game::run(raylib::Window &window, raylib::AudioDevice &audio) {
        using namespace sml;
        using namespace state;
        GameDefinition game;
        sm<GameState> sm{game};
        sm.process_event(events::Init{});
        sm.process_event(events::Run{window, audio});
        sm.process_event(events::Stop{});
    }
}// namespace basilevs

std::vector<Texture2D> GameDefinition::load_textures_() {
    return assets::load_textures_level_1();
}

/*
void GameDefinition::load_sprites_() const {
    sprite_template_map = {
            {basilevs::EntityType::Player, Sprite(texture_map.at(basilevs::EntityType::Player), raylib::Vector2{64, 90}, 7)},
            {basilevs::EntityType::Enemy, Sprite(texture_map.at(basilevs::EntityType::Enemy), raylib::Vector2(70, 30), 1)},
            {basilevs::EntityType::BulletRound,Sprite(texture_map.at(basilevs::EntityType::BulletRound), raylib::Vector2(50, 50), 1)},
            {basilevs::EntityType::BulletPlayer, Sprite(texture_map.at(basilevs::EntityType::BulletPlayer), raylib::Vector2(50, 50), 1)}};
}
*/
void GameDefinition::load_sounds_() const {
    sound_map = {{basilevs::EntityType::BulletRound, LoadSound("assets/bullet.wav")}};
}

namespace emitter {
    enum class Type { ShootEverySecond,
                      ShootSpiral,
                      ShootNormal,
                      ShootCircular };

    /*static constexpr auto shoot_every_second = [](const float time, Emitter &emitter, World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {

                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2{0.0, 1.0f}, basilevs::bullet::bullet_fly_forward});
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += time;
        }
    };

    static constexpr auto shoot_spiral = [](const float time, Emitter &emitter, World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {
                //   for (int i = 0 ; i < 4; i++) {
                emitter.angle += 15.0f;
                if (emitter.angle > 360.0f) {
                    emitter.angle = 0.0f;
                }
                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);
                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 90), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);

                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 180), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);
                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 270), basilevs::bullet::bullet_fly_spiral});
                // PlaySound(emitter.sound);
                // }
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += time;
        }
    };

    static constexpr auto player_normal_shoot = [](const float time, Emitter &emitter, World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {

                world.player_bullets.add(NormalBullet{Vector2Add(emitter.position, Vector2{-5.0f, -1.0f}), Vector2{0.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(NormalBullet{Vector2Add(emitter.position, Vector2{5.0f, -1.0f}), Vector2{0.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(NormalBullet{emitter.position, Vector2{-1.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(NormalBullet{emitter.position, Vector2{1.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                // PlaySound(emitter.sound);
                emitter.last_shot = 0.0f;
            }
        }
        emitter.last_shot += time;
    };

    static constexpr auto shoot_circular = [](const float time, Emitter &emitter, World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {
                auto direction = Vector2{0.0f, 1.0f};
                for (int i = 1; i < 60; i++) {
                    //PlaySound(emitter.sound);
                    world.enemy_bullets.add(NormalBullet{emitter.position, direction, basilevs::bullet::bullet_fly_forward});
                    direction = Vector2Rotate(direction, 360 / i);
                }
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += sin(time);
        }
    };*/
}// namespace emitter

constexpr auto initialize_player = [](const std::vector<Texture2D> &textures) {
  auto player = Blueprint(behaviours::player::UpdateFunction(behaviours::player::kPlayerNormalBehaviour));
  auto &player_sprite = get<components::Sprite>(player);
  player_sprite.texture = assets::TextureId::Player;
  player_sprite.amount_frames = 7;
  auto texture_width = textures[static_cast<int>(player_sprite.texture)].width;
  auto texture_height = textures[static_cast<int>(player_sprite.texture)].height;
  player_sprite.texture_width = texture_width;
  player_sprite.texture_height = texture_height;
  player_sprite.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(player_sprite.amount_frames), static_cast<float>(texture_height)};

  return std::make_shared<decltype(player)>(player);
};

constexpr auto initialize_background = [](const std::vector<Texture2D> &textures) {
  auto background = Blueprint(behaviours::background::UpdateFunction(behaviours::background::level1_background_update));
  auto &background_sprite = get<components::Sprite>(background);
  auto texture = textures[static_cast<int>(assets::TextureId::Background_Level_1)];
  background_sprite.texture = assets::TextureId::Background_Level_1;
  background_sprite.amount_frames = 6;
  background_sprite.texture_width = texture.width;
  background_sprite.texture_height = texture.height;
  background_sprite.frame_rect = {0.0, static_cast<float>(texture.height - (texture.height / background_sprite.amount_frames)), static_cast<float>(texture.width), static_cast<float>((texture.height / background_sprite.amount_frames))};

  return std::make_shared<decltype(background)>(background);
};

void GameDefinition::initialize_world_() {

    world.background = initialize_background(textures_);
    world.player = initialize_player(textures_);

    /* auto player = BasilevsPlayer{};
    player.sprite = sprite_template_map.at(basilevs::EntityType::Player);
    player.emitter = Emitter{{0.0f, 0.0f}, 0.023f, emitter::player_normal_shoot};
    world = World{
            player,
            Background("assets/basilevs_bg_001.png", 6),
            Rectangle{-10.0f, -10.0f, config::kFrameWidth + 10.0f, config::kFrameHeight + 10.0f},
            BulletPool<NormalBullet>(1000L), BulletPool<NormalBullet>(500L)};

    auto radial_emitter = Emitter{sprite_template_map.at(basilevs::EntityType::Enemy).position, 1.5f, emitter::shoot_circular};
    auto spiral_emitter = Emitter{sprite_template_map.at(basilevs::EntityType::Enemy).position, 0.1f, emitter::shoot_spiral};

    world.enemy_spawns = std::list<Spawn>({
            {1.0f, Vector2{60.0, 10.0}, sprite_template_map.at(basilevs::EntityType::Enemy), basilevs::enemy::behavior_sinusoidal, radial_emitter},
            {3.0f, Vector2{60.0, 30.0}, sprite_template_map.at(basilevs::EntityType::Enemy), basilevs::enemy::behavior_sinusoidal, spiral_emitter},
    });

    world.enemies_on_screen = std::vector<Enemy>{};*/
}

void GameDefinition::initialize() {
    //load_sprites_();
    //load_sounds_();
    initialize_world_();
    SetTargetFPS(60);
    //SetTextureFilter(world.background.texture, FILTER_ANISOTROPIC_16X);
}

void GameDefinition::loop_(std::chrono::duration<double> duration) {
    world.background->update(duration.count(), world);
    world.player->update(duration.count(), world);
    render_();
}

void GameDefinition::run(raylib::Window &window, raylib::AudioDevice &audio) {
    std::chrono::duration<double> loop_duration = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
    while (!window.ShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        loop_(loop_duration);
        loop_duration = std::chrono::steady_clock::now() - now;
    }
}

static constexpr auto render_player = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
    auto player = world.player.get();
    auto sprite_component = std::get<components::Sprite>(player->components);
    auto texture = textures[static_cast<int>(sprite_component.texture)];

    DrawTextureRec(texture, sprite_component.frame_rect, {20, 20}, WHITE);
};

static constexpr auto render_background = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
    auto background = world.background.get();
    auto sprite_component = std::get<components::Sprite>(background->components);
    auto texture = textures[static_cast<int>(sprite_component.texture)];

    DrawTextureRec(texture, sprite_component.frame_rect, {0, 0}, WHITE);
};

static constexpr auto render_to_texture = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
    BeginTextureMode(render_target);
    ClearBackground(config::colors::kBackground);
    render_background(render_target, world, textures);
    render_player(render_target, world, textures);
    EndTextureMode();
};

static constexpr auto render_to_screen = [] (raylib::RenderTexture & render_target, const TWorld &world) {
    DrawTexturePro(render_target.texture, Rectangle{0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                   Rectangle{0.0f, 0.0f, static_cast<float>(config::kScreenWidth), static_cast<float>(config::kScreenHeight)}, Vector2{0, 0}, 0.0f, WHITE);
    //raylib::DrawText(std::to_string(static_cast<int>(world.timer)), config::kScreenWidth - 60, 20, 30, GREEN);
    raylib::DrawText(std::to_string(world.enemy_bullets.first_available_index), config::kScreenWidth - 60, 60, 30, ORANGE);
    DrawFPS(5, 5);
};

void GameDefinition::render_() {
    BeginDrawing();
    ClearBackground(config::colors::kBackground);
    render_to_texture(render_target_, world, textures_);
    render_to_screen(render_target_, world);
    EndDrawing();
}
