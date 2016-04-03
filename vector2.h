//
// Created by awbrenn on 3/11/16.
//

#ifndef SPH_PARTICLE_VECTOR2_H
#define SPH_PARTICLE_VECTOR2_H


class vector2 {
public:
    float x, y;
    vector2(float x = 0.0f, float y =0.0f);
    vector2(const vector2& src);
    const vector2& operator=(const vector2& rhs);
    ~vector2();

    vector2 operator+(const vector2& rhs) const;
    vector2 operator-(const vector2& rhs) const;
    vector2 operator*(const vector2& rhs) const;
    vector2 operator/(const vector2& rhs) const;
//    vector2 operator/(const float &constant);

    const vector2& operator+=(const vector2& rhs);
    const vector2& operator-=(const vector2& rhs);
    const vector2& operator*=(const vector2& rhs);
    const vector2& operator/=(const vector2& rhs);

    float length() const;
    float dot(const vector2 &rhs);
    vector2 scale(const float factor);
    vector2 unit() const;
};


#endif //SPH_PARTICLE_vector2_H
