#pragma once
#include "raylib.h"
#include "Vector3.h"
#include <vector>

// Manages tank track marks: stamping, fading and drawing.
class Tracks
{
public:
    Tracks(Texture2D trackTexture,
        float spacing = 12.0f,
        float lateralOffset = 22.0f,
        double fadeStart = 6.5,
        double fadeEnd = 12.5);

    // Set the size of each track stamp on screen (pixels).
    void SetStampSize(float width, float height);

    // Pass current tank center, body rotation (deg), and deltaTime (s).
    void UpdateFromMotion(const MathClasses::Vector3& currentPos, float bodyRotationDeg, float deltaTime);

    // Optional tunables for rotation stamping
    void SetRotationHold(double seconds) { _rotateHoldSec = seconds; }
    void SetMotionThresholds(float moveSpeedEps, float rotRateEps) { _moveSpeedEps = moveSpeedEps; _rotRateEps = rotRateEps; }

    // Draw all visible marks (handles fading alpha).
    void Draw() const;

    // If you still need manual control:
    void ResetPath(const MathClasses::Vector3& startPos);
    void Accumulate(const MathClasses::Vector3& currentPos);
    void AccumulateRotation(const MathClasses::Vector3& centerPos, float bodyRotationDeg);
    void Update();

private:
    struct TrackMark
    {
        MathClasses::Vector3 pos;  // world position (x,y)
        float rotationDeg;         // rotation in degrees
        double time;               // creation time
    };

    Texture2D _texture{};
    float _spacing;
    float _offset;
    double _fadeStart;
    double _fadeEnd;

    float _stampW;
    float _stampH;

    // Rotation stamping spacing and state
    float  _rotAngleSpacing = 6.0f; // degrees between rotation stamps
    float  _lastAngleDeg = 0.0f;
    bool   _hasLastAngle{ false };

    // Frame-to-frame motion sampling (for internal thresholds)
    MathClasses::Vector3 _prevFramePos{};
    float  _prevFrameAngleDeg{ 0.0f };
    bool   _hasPrevFrame{ false };

    // Debounce so quick A/D taps don't stamp rotation
    double _rotateHoldSec{ 0.30 };
    double _rotateHoldTimer{ 0.0 };

    // Thresholds
    float _moveSpeedEps{ 10.0f };  // px/sec
    float _rotRateEps{ 30.0f };    // deg/sec

    // Spaced path stamping state
    std::vector<TrackMark> _marks;
    MathClasses::Vector3 _lastPos{};  // last spaced-stamp position
    bool _hasLastPos{ false };
};