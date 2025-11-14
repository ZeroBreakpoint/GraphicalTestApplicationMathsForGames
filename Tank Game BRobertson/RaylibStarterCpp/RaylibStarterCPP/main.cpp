#include <iostream>
#include "raylib.h"
#include <filesystem>
#include "Tank.h"
#include "Bullet.h"
#include "Tracks.h"
#include <vector>
#include <algorithm>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS

using namespace MathClasses;

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Tank Game - Bradley Robertson");

    // Loading in textures for the tank body, turret and bullet
    Texture2D bodyTexture = LoadTexture("../assets/images/body.png");
    Texture2D turretTexture = LoadTexture("../assets/images/turret.png");
    Texture2D bulletTexture = LoadTexture("../assets/images/bullet.png");

    // Loading in grass textures for the background
    Texture2D grassTexture1 = LoadTexture("../assets/images/grass1.png");
    Texture2D grassTexture2 = LoadTexture("../assets/images/grass2.png");

    // Crate and explosion textures
    Texture2D crateTexture = LoadTexture("../assets/images/cratewood.png");
    Texture2D explosionTexture = LoadTexture("../assets/images/explosion.png");

    // Track texture
    Texture2D trackTexture = LoadTexture("../assets/images/tracks.png");
    Tracks tracks(trackTexture, /*spacing*/12.0f, /*offset*/22.0f, /*fadeStart*/6.5, /*fadeEnd*/12.5);

    // Set stamp size using a scale factor of the texture size
    const float trackScale = 1.6f; // 60% of original
    tracks.SetStampSize(trackTexture.width * trackScale, trackTexture.height * trackScale);

    // Initialise the starting location of the tank
    const Vector2 tankStart = { (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    Tank tank(MathClasses::Vector3{ tankStart.x, tankStart.y, 0.0f }, bodyTexture, turretTexture, bulletTexture);
    tracks.ResetPath(tank.GetPosition());

	// Spawn in crates at random non-overlapping positions
    const int crateCount = 6;
    const Vector2 spawnCrateSize = { 40, 40 };
	const float minSpawnDistance = 220.0f; // min distance to spawn crates from the tank starting position
    const float minSpawnDistSqr = minSpawnDistance * minSpawnDistance;
    const int maxSpawnAttempts = 300;

    std::vector<Vector2> cratePositions;
    std::vector<Vector2> crateSizes;
    std::vector<bool>    crateAlive;
    std::vector<double>  crateExplosionStart;

    cratePositions.reserve(crateCount);
    crateSizes.reserve(crateCount);
    crateAlive.reserve(crateCount);
    crateExplosionStart.reserve(crateCount);

    int attempts = 0;
    while ((int)cratePositions.size() < crateCount && attempts < maxSpawnAttempts)
    {
        attempts++;

        float px = (float)GetRandomValue(0, screenWidth - (int)spawnCrateSize.x);
        float py = (float)GetRandomValue(0, screenHeight - (int)spawnCrateSize.y);
        Vector2 pos{ px, py };

        // Reject if too close to tank start (compare centers)
        Vector2 center{ pos.x + spawnCrateSize.x * 0.5f, pos.y + spawnCrateSize.y * 0.5f };
        float dx = center.x - tankStart.x;
        float dy = center.y - tankStart.y;
        if (dx * dx + dy * dy < minSpawnDistSqr) continue;

        // Avoid overlapping previous crates
        bool overlaps = false;
        for (size_t i = 0; i < cratePositions.size(); ++i)
        {
            Rectangle a{ pos.x, pos.y, spawnCrateSize.x, spawnCrateSize.y };
            Rectangle b{ cratePositions[i].x, cratePositions[i].y, crateSizes[i].x, crateSizes[i].y };
            if (CheckCollisionRecs(a, b)) { overlaps = true; break; }
        }
        if (overlaps) continue;

        cratePositions.push_back(pos);
        crateSizes.push_back(spawnCrateSize);
        crateAlive.push_back(true);
        crateExplosionStart.push_back(0.0);
    }

    const double explosionDuration = 0.5;

    SetTargetFPS(120);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        tank.Update(deltaTime);

        // Call to update tracks based on tank movement and rotation
        tracks.UpdateFromMotion(tank.GetPosition(), tank.GetBodyRotation(), deltaTime);

		// Tank and crate collision outcomes
        {
            Vector2 tankCenter{ tank.GetPosition().x, tank.GetPosition().y };
            float tankRadius = (float)std::min(bodyTexture.width, bodyTexture.height) * 0.33f; // tweak to fit sprite

            for (size_t i = 0; i < cratePositions.size(); ++i)
            {
                if (!crateAlive[i]) continue;
                Rectangle rec{ cratePositions[i].x, cratePositions[i].y, crateSizes[i].x, crateSizes[i].y };
                if (CheckCollisionCircleRec(tankCenter, tankRadius, rec))
                {
                    crateAlive[i] = false;
                    crateExplosionStart[i] = GetTime();
                }
            }
        }

        // Destroy bullets that are out of bounds or have collided with any crate
        tank.GetBullets().erase(std::remove_if(tank.GetBullets().begin(), tank.GetBullets().end(),
            [&](const Bullet& bullet)
            {
                if (bullet.IsOutOfBounds()) return true;

                // Bullet vs crates: same effect as tank collision
                for (size_t i = 0; i < cratePositions.size(); ++i)
                {
                    if (!crateAlive[i]) continue;
                    if (bullet.BoxCollision(cratePositions[i], crateSizes[i]))
                    {
                        crateAlive[i] = false;
                        crateExplosionStart[i] = GetTime();
                        return true; // remove this bullet
                    }
                }
                return false;
            }), tank.GetBullets().end());

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw tiled grass background (alternating between grass1/grass2)
        if (grassTexture1.id != 0 && grassTexture2.id != 0)
        {
            const int tileW = grassTexture1.width;
            const int tileH = grassTexture1.height;
            const int cols = (screenWidth + tileW - 1) / tileW;
            const int rows = (screenHeight + tileH - 1) / tileH;

            for (int row = 0; row < rows; ++row)
            {
                for (int col = 0; col < cols; ++col)
                {
                    const Texture2D& tex = (col % 2 == 0) ? grassTexture1 : grassTexture2;
                    DrawTexture(tex, col * tileW, row * tileH, WHITE);
                }
            }
        }
        else
        {
            // Output if paths/working directory are wrong
            DrawText("Grass textures failed to load", 20, 20, 20, RED);
        }

        // Draw tracks above grass
        tracks.Draw();

		// Logic to draw crates and their explosions
        for (size_t i = 0; i < cratePositions.size(); ++i)
        {
            const Vector2& pos = cratePositions[i];
            const Vector2& size = crateSizes[i];

            if (crateAlive[i])
            {
                if (crateTexture.id != 0)
                {
                    Rectangle src{ 0, 0, (float)crateTexture.width, (float)crateTexture.height };
                    Rectangle dst{ pos.x, pos.y, size.x, size.y };
                    Vector2 origin{ 0, 0 };
                    DrawTexturePro(crateTexture, src, dst, origin, 0.0f, WHITE);
                }
            }
            else
            {
                double t = GetTime() - crateExplosionStart[i];
                if (t < explosionDuration && explosionTexture.id != 0)
                {
                    Rectangle src{ 0, 0, (float)explosionTexture.width, (float)explosionTexture.height };

					// Crate explosion draw size (2x size)
                    const float exW = size.x * 2.0f;
                    const float exH = size.y * 2.0f;
                    const float cx = pos.x + size.x * 0.5f;
                    const float cy = pos.y + size.y * 0.5f;
                    Rectangle dst{ cx - exW * 0.5f, cy - exH * 0.5f, exW, exH };

                    Vector2 origin{ 0, 0 };
                    DrawTexturePro(explosionTexture, src, dst, origin, 0.0f, WHITE);
                }
            }
        }

        // Prune finished explosions to keep arrays compact
        for (size_t i = 0; i < cratePositions.size(); ++i)
        {
            if (!crateAlive[i] && (GetTime() - crateExplosionStart[i]) >= explosionDuration)
            {
                size_t last = cratePositions.size() - 1;
                cratePositions[i] = cratePositions[last];
                crateSizes[i] = crateSizes[last];
                crateAlive[i] = crateAlive[last];
                crateExplosionStart[i] = crateExplosionStart[last];

                cratePositions.pop_back();
                crateSizes.pop_back();
                crateAlive.pop_back();
                crateExplosionStart.pop_back();
                --i; // re-check swapped element
            }
        }

        tank.Draw();

        EndDrawing();
    }

    // Unloading the textures
    UnloadTexture(bodyTexture);
    UnloadTexture(turretTexture);
    UnloadTexture(bulletTexture);
    UnloadTexture(grassTexture1);
    UnloadTexture(grassTexture2);
    UnloadTexture(crateTexture);
    UnloadTexture(explosionTexture);
    UnloadTexture(trackTexture);

    CloseWindow();

    system("pause");

    return 0;
}