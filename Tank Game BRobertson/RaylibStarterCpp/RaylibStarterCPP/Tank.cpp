#include "Tank.h"
#include <iostream>

using namespace MathClasses;

// Constructor initialising tank properties
Tank::Tank(MathClasses::Vector3 position, Texture2D bodyTexture, Texture2D turretTexture, Texture2D bulletTexture)
    : position(position), bodyTexture(bodyTexture), turretTexture(turretTexture), bulletTexture(bulletTexture), bodyRotation(-180.0f), turretRotation(0.0f)
{
    bodyTransform = Matrix3::MakeIdentity();
    turretTransform = Matrix3::MakeIdentity();
    turretOffset = MathClasses::Vector3(0.0f, -bodyTexture.height / 3.0f, 0.0f);
}

// Update the tank's state based on player input
void Tank::Update(float deltaTime)
{
    // Rotate tank body
    if (IsKeyDown(KEY_A))
    {
        RotateBody(-60.0f * deltaTime);
    }
    if (IsKeyDown(KEY_D))
    {
        RotateBody(60.0f * deltaTime);
    }

    // Move tank
    if (IsKeyDown(KEY_W))
    {
        MoveBody(-100.0f * deltaTime);
    }
    if (IsKeyDown(KEY_S))
    {
        MoveBody(100.0f * deltaTime);
    }

    // Rotate turret
    if (IsKeyDown(KEY_Q))
    {
        RotateTurret(-60.0f * deltaTime);
    }
    if (IsKeyDown(KEY_E))
    {
        RotateTurret(60.0f * deltaTime);
    }

    // Fire bullet
    if (IsKeyPressed(KEY_SPACE))
    {
        FireBullet();
    }

    for (auto& bullet : bullets)
    {
        bullet.Update(deltaTime);
    }
}

// Draw the tank and its components
void Tank::Draw()
{
    Vector2 bodyPos = {position.x, position.y};

    // Draw tank body
    DrawTexturePro(bodyTexture, {0, 0, (float)bodyTexture.width, (float)bodyTexture.height},
                  {bodyPos.x, bodyPos.y, (float)bodyTexture.width, (float)bodyTexture.height},
                  {bodyTexture.width / 2.0f, bodyTexture.height / 2.0f}, bodyRotation, WHITE);

    // Calculate turret position with offset and apply body transformation
    MathClasses::Vector3 turretBottomOffset = MathClasses::Vector3(0.0f, turretTexture.height / 2.0f, 0.0f);

    Matrix3 turretTranslationToTank = Matrix3::MakeTranslation(turretOffset.x, turretOffset.y, 0.0f);

    Matrix3 turretPivotTranslation = Matrix3::MakeTranslation(-turretBottomOffset.x, -turretBottomOffset.y, 0.0f);

    Matrix3 turretRotationMatrix = Matrix3::MakeRotateZ(turretRotation * DEG2RAD);

    Matrix3 turretTranslationBack = Matrix3::MakeTranslation(turretBottomOffset.x, turretBottomOffset.y, 0.0f);

    Matrix3 turretTransform = turretTranslationToTank * turretTranslationBack * turretRotationMatrix * turretPivotTranslation;
    Matrix3 combinedTransform = bodyTransform * turretTransform;

    Vector2 turretPos = {combinedTransform.mm[2][0] + position.x, combinedTransform.mm[2][1] + position.y};

    // Draw turret
    DrawTexturePro(turretTexture, { 0, 0, (float)turretTexture.width, (float)turretTexture.height},
     {turretPos.x, turretPos.y, (float)turretTexture.width, (float)turretTexture.height},
     {turretTexture.width / 2.0f, turretTexture.height / 2.0f}, bodyRotation + turretRotation, WHITE);


    for (auto& bullet : bullets)
    {
        bullet.Draw();
    }
}

// Rotate the tank body using my Matrix3 function
void Tank::RotateBody(float angle)
{
    bodyRotation += angle;
    Matrix3 rotationMatrix = Matrix3::MakeRotateZ(angle * DEG2RAD);
    bodyTransform = rotationMatrix * bodyTransform;
}

// Move the tank body forward or backward using my Vector3
void Tank::MoveBody(float distance)
{
    MathClasses::Vector3 forward = bodyTransform.axis[1].Normalised();
    position = position + forward * distance;
}

// Rotate the turret using my Matrix3 function
void Tank::RotateTurret(float angle)
{
    turretRotation += angle;
    Matrix3 rotationMatrix = Matrix3::MakeRotateZ(angle * DEG2RAD);
    turretTransform = rotationMatrix * turretTransform;
}

// Fire a bullet from the end of the turret
void Tank::FireBullet()
{
    // Calculate the direction vector of the turret
    float adjustedRotation = (bodyRotation + turretRotation + 90.0f) * DEG2RAD;

    MathClasses::Vector3 bulletDirection = MathClasses::Vector3{ cosf(adjustedRotation), sinf(adjustedRotation), 0.0f };

    // Calculate bullet spawn position at the end of the turret
    float turretLength = static_cast<float>(turretTexture.height);
    MathClasses::Vector3 turretEndOffset = MathClasses::Vector3(0.0f, -turretLength, 0.0f); 
    Matrix3 turretTranslation = Matrix3::MakeTranslation(turretOffset.x, turretOffset.y, 0.0f); 
    Matrix3 turretRotationMatrix = Matrix3::MakeRotateZ(turretRotation * DEG2RAD);
    Matrix3 combinedTransform = bodyTransform * turretTranslation * turretRotationMatrix;
    MathClasses::Vector3 turretEnd = combinedTransform * turretEndOffset;
    MathClasses::Vector3 bulletPosition = position + turretEnd;

    // Create and store the bullet
    bullets.emplace_back(bulletPosition, bulletDirection, bulletTexture);
}

// Get the current position of the tank
MathClasses::Vector3 Tank::GetPosition() const
{
    return position;
}

// Get the transformation matrix of the turret
Matrix3 Tank::GetTurretTransform() const
{
    return turretTransform;
}

std::vector<Bullet>& Tank::GetBullets()
{
    return bullets;
}

// Expose current body rotation (degrees)
float Tank::GetBodyRotation() const
{
    return bodyRotation;
}


