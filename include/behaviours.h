//
// Created by geoco on 12.04.2021.
//

#ifndef BASILEVS_BEHAVIOURS_H
#define BASILEVS_BEHAVIOURS_H
namespace basilevs {
    namespace bullet {
        static constexpr auto bullet_fly_forward = [](float time, basilevs::NormalBullet &bullet, const basilevs::World &world) -> bool {
            bullet.position = Vector2Add(bullet.position, bullet.direction);
            bullet.timer += time;

            Vector2Normalize(bullet.direction);
            return true;
        };

        static constexpr auto bullet_fly_spiral = [](const float time, basilevs::NormalBullet &bullet, const basilevs::World &world) -> bool {
            bullet.timer += time;
            //if (bullet.timer > 0.1f) {
            bullet.rotation += 0.01f;
            if (bullet.rotation > 360) {
                bullet.rotation = 0;
            }
            bullet.timer = 0.0f;
            //}
            //bullet.position.x += bullet.direction.x ;//

            bullet.position = Vector2Add(bullet.position, bullet.direction);
            //bullet.direction = Vector2Rotate(bullet.direction, bullet.rotation);
            //bullet.direction = Vector2Normalize(bullet.direction);
            return true;
        };
    }// namespace bullet
    namespace emitter {
        static constexpr auto shoot_every_second = [](const float time, basilevs::Emitter &emitter, basilevs::World &world) {
            if (emitter.is_active) {
                if (emitter.last_shot > emitter.delay_between_shots) {

                    world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2{0.0, 1.0f}, basilevs::bullet::bullet_fly_forward});
                    emitter.last_shot = 0.0f;
                }
                emitter.last_shot += time;
            }
        };

        static constexpr auto shoot_spiral = [](const float time, basilevs::Emitter &emitter, basilevs::World &world) {
            if (emitter.is_active) {
                if (emitter.last_shot > emitter.delay_between_shots) {
                    //   for (int i = 0 ; i < 4; i++) {
                    emitter.angle += 15.0f;
                    if (emitter.angle > 360.0f) {
                        emitter.angle = 0.0f;
                    }
                    world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle), basilevs::bullet::bullet_fly_spiral});
                    PlaySound(emitter.sound);
                    world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 90), basilevs::bullet::bullet_fly_spiral});
                    PlaySound(emitter.sound);

                    world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 180), basilevs::bullet::bullet_fly_spiral});
                    PlaySound(emitter.sound);
                    world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 270), basilevs::bullet::bullet_fly_spiral});
                    PlaySound(emitter.sound);
                    // }
                    emitter.last_shot = 0.0f;
                }
                emitter.last_shot += time;
            }
        };

        static constexpr auto shoot_circular = [](const float time, basilevs::Emitter &emitter, basilevs::World &world) {
            if (emitter.is_active) {
                if (emitter.last_shot > emitter.delay_between_shots) {
                    auto direction = Vector2{0.0f, 1.0f};
                    for (int i = 1; i < 60; i++) {
                        PlaySound(emitter.sound);
                        world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, direction, basilevs::bullet::bullet_fly_forward});
                        direction = Vector2Rotate(direction, 360 / i);
                    }
                    emitter.last_shot = 0.0f;
                }
                emitter.last_shot += sin(time);
            }
        };
    }// namespace emitter
    namespace enemy {
        constexpr auto behavior_sinusoidal = [](basilevs::Enemy &enemy, double time) -> void {
            enemy.position.x += sin(time) * 0.7;
        };
    }
}// namespace basilevs
#endif//BASILEVS_BEHAVIOURS_H
