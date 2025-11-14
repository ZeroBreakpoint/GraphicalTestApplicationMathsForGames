#include "Tracks.h"
#include <algorithm>
#include <cmath>

using namespace MathClasses;
// Helper functions
static inline float RadToDeg(float r) { return r * (180.0f / 3.14159265358979323846f); }
static inline float DegToRad(float d) { return d * (3.14159265358979323846f / 180.0f); }
static inline float AngleDeltaDeg(float a, float b)
{
    float d = fmodf(a - b, 360.0f);
    if (d > 180.0f) d -= 360.0f;
    if (d < -180.0f) d += 360.0f;
    return d;
}
// Tracks overloaded constructor
Tracks::Tracks(Texture2D trackTexture, float spacing, float lateralOffset, double fadeStart, double fadeEnd)
    : _texture(trackTexture)
    , _spacing(spacing)
    , _offset(lateralOffset)
    , _fadeStart(fadeStart)
    , _fadeEnd(fadeEnd)
    , _stampW((float)trackTexture.width)
    , _stampH((float)trackTexture.height)
{
}
// Set the size of each track stamp on screen (pixels).
void Tracks::SetStampSize(float width, float height)
{
    _stampW = width;
    _stampH = height;
}
// Reset the path tracking to a specific start position
void Tracks::ResetPath(const MathClasses::Vector3& startPos)
{
    _lastPos = startPos;
    _hasLastPos = true;
    _lastAngleDeg = 0.0f;
    _hasLastAngle = false;

    _prevFramePos = startPos;
    _prevFrameAngleDeg = _lastAngleDeg;
    _hasPrevFrame = true;
    _rotateHoldTimer = 0.0;
}
// Accumulate track marks along the movement path
void Tracks::Accumulate(const MathClasses::Vector3& currentPos)
{
    if (_texture.id == 0) return;
    if (!_hasLastPos)
    {
        ResetPath(currentPos);
        return;
    }

    MathClasses::Vector3 delta = currentPos - _lastPos;
    float dist = delta.Magnitude();
    if (dist <= 0.0f) return;

    MathClasses::Vector3 forward = delta.Normalised();

    while (dist >= _spacing)
    {
        _lastPos = _lastPos + forward * _spacing;
        dist -= _spacing;

        float angleDeg = RadToDeg(std::atan2(forward.y, forward.x));
        double now = GetTime();

        // Single centered stamp (texture contains both treads)
        _marks.push_back(TrackMark{ _lastPos, angleDeg, now });
    }
}
// Accumulate track marks for in-place rotation
void Tracks::AccumulateRotation(const MathClasses::Vector3& centerPos, float bodyRotationDeg)
{
    if (_texture.id == 0) return;

    if (!_hasLastAngle)
    {
        _lastAngleDeg = bodyRotationDeg;
        _hasLastAngle = true;
        return;
    }

    float deltaDeg = AngleDeltaDeg(bodyRotationDeg, _lastAngleDeg);
    if (std::fabs(deltaDeg) < _rotAngleSpacing) return;

    float step = (deltaDeg > 0.0f) ? _rotAngleSpacing : -_rotAngleSpacing;
    double now = GetTime();

    // Single centered stamp per angle step (no left/right offset)
    while ((deltaDeg > 0.0f && AngleDeltaDeg(bodyRotationDeg, _lastAngleDeg) >= _rotAngleSpacing) ||
        (deltaDeg < 0.0f && AngleDeltaDeg(bodyRotationDeg, _lastAngleDeg) <= -_rotAngleSpacing))
    {
        _lastAngleDeg = _lastAngleDeg + step;
        _marks.push_back(TrackMark{ centerPos, _lastAngleDeg, now });
    }
}
// Update and remove old track marks based on fade timing
void Tracks::Update()
{
    if (_marks.empty()) return;
    double now = GetTime();
    _marks.erase(std::remove_if(_marks.begin(), _marks.end(), [&](const TrackMark& m)
        {
            return (now - m.time) >= _fadeEnd;
        }), _marks.end());
}

void Tracks::UpdateFromMotion(const MathClasses::Vector3& currentPos, float bodyRotationDeg, float deltaTime)
{
    if (deltaTime <= 0.0f) deltaTime = 0.0001f;

    // Always stamp movement path (spacing handles density)
    Accumulate(currentPos);

    // First-frame prev sampling
    if (!_hasPrevFrame)
    {
        _prevFramePos = currentPos;
        _prevFrameAngleDeg = bodyRotationDeg;
        _hasPrevFrame = true;
        Update();
        return;
    }

    // Compute motion/rotation rates
    float speedPxPerSec = (currentPos - _prevFramePos).Magnitude() / deltaTime;
    float rotRateDegPerSec = std::fabs(AngleDeltaDeg(bodyRotationDeg, _prevFrameAngleDeg)) / deltaTime;

    bool moving = speedPxPerSec > _moveSpeedEps;
    bool rotating = rotRateDegPerSec > _rotRateEps;

    // Only stamp rotation when rotating in-place for long enough
    if (!moving && rotating)
    {
        _rotateHoldTimer += deltaTime;
        if (_rotateHoldTimer >= _rotateHoldSec)
        {
            AccumulateRotation(currentPos, bodyRotationDeg);
        }
    }
    else
    {
        _rotateHoldTimer = 0.0;
        _hasLastAngle = false; // restart rotation spacing cleanly next time
    }

    Update();

    // Store for next frame
    _prevFramePos = currentPos;
    _prevFrameAngleDeg = bodyRotationDeg;
}

void Tracks::Draw() const
{
    if (_texture.id == 0) return;

    double now = GetTime();
    Rectangle src{ 0, 0, (float)_texture.width, (float)_texture.height };

    for (const TrackMark& m : _marks)
    {
        double age = now - m.time;
        float alpha = 1.0f;
        if (age >= _fadeStart)
        {
            float t = (float)((age - _fadeStart) / (_fadeEnd - _fadeStart));
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            alpha = 1.0f - t;
        }

        Rectangle dst{ m.pos.x, m.pos.y, _stampW, _stampH };
        Vector2 origin{ _stampW * 0.5f, _stampH * 0.5f };
        Color tint{ 255, 255, 255, (unsigned char)(255.0f * alpha) };

        // Align the vertical sprite with movement (tweak sign if needed)
        DrawTexturePro(_texture, src, dst, origin, m.rotationDeg - 90.0f, tint);
    }
}