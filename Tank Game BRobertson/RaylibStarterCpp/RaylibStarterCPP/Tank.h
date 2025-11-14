#pragma once
#include "raylib.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Bullet.h"
#include <vector>

using namespace MathClasses;

// Tank class controls movement, rotation, firing and drawing of a tank
class Tank
{
public:
    Tank(MathClasses::Vector3 position, Texture2D bodyTexture, Texture2D turretTexture, Texture2D bulletTexture);
    void Update(float deltaTime); // Handles player input and updates bullets
    void Draw(); // Renders the tank and its bullets
    void RotateBody(float angle); // Rotates the tank's body
    void MoveBody(float distance); // Moves the tank along its facing direction
    void RotateTurret(float angle); // Rotates the turret independently of the body
    void FireBullet(); // Spawns a new bullet from the turret's tip
    MathClasses::Vector3 GetPosition() const;
    Matrix3 GetTurretTransform() const;
    std::vector<Bullet>& GetBullets(); // Returns a reference to the bullets vector
    // Expose current body rotation (degrees) so tracks can react to in-place turns
    float GetBodyRotation() const;

private:
    MathClasses::Vector3 position; // Tank's world position
    float bodyRotation; // Angle in degrees for tank body
    float turretRotation; // Angle in degrees for turret
    Texture2D bodyTexture, turretTexture, bulletTexture; // Textures used
    Matrix3 bodyTransform, turretTransform; // Local transformation matrices
    MathClasses::Vector3 turretOffset; // Offset from tank centre to turret base
    std::vector<Bullet> bullets; // Collection of active bullets
};

