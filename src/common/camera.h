//
// Created by henry on 3/5/24.
//

#ifndef HABAN_CAMERA_H
#define HABAN_CAMERA_H

#include <hagame/graphics/camera.h>

class Camera : public hg::graphics::OrthographicCamera {
public:

    void setPosition(hg::Vec3 pos) {
        transform.position = pos;
    }

    void lerp(hg::Vec3 to, double duration) {
        if (!m_lerping) {
            m_lerping = true;
            m_lerpA = transform.position;
            m_lerpB = to;
            m_duration = duration;
            m_t = 0;
        }
    }

    void update(double dt) {
        if (m_lerping) {
            m_t = std::clamp<double>(m_t + dt * (1. / m_duration), 0, 1);
            setPosition(m_lerpA + (m_lerpB - m_lerpA) * m_t);
            if (m_t >= 1) {
                m_lerping = false;
            }
        }
    }

    bool lerping() const { return m_lerping; }

private:

    hg::Vec3 m_lerpA, m_lerpB;
    bool m_lerping = false;
    double m_duration = false;
    double m_t = 0;

};

#endif //HABAN_CAMERA_H
