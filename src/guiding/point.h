/*
 *  point.h
 *  LGuider Guiding
 *
 *  Created by Bret McKee
 *  Copyright (c) 2012 Bret McKee
 *  All rights reserved.
 *
 *  This source code is distributed under the following "BSD" license
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *    Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *    Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *    Neither the name of Bret McKee, Dad Dog Development,
 *     Craig Stark, Stark Labs nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

class LGuider_Point {
    bool m_valid;

public:
    double X;
    double Y;

    LGuider_Point(double x, double y) { SetXY(x, y); }

    LGuider_Point(const LGuider_Point& p)
        : m_valid(p.m_valid), X(p.X), Y(p.Y) {}

    LGuider_Point() { Invalidate(); }

    bool IsValid() const { return m_valid; }

    void Invalidate() { m_valid = false; }

    void SetXY(double x, double y) {
        X = x;
        Y = y;
        m_valid = true;
    }

    double dX(const LGuider_Point& p) const {
        assert(m_valid && p.m_valid);

        double dRet = X - p.X;
        return dRet;
    }

    double dX(const LGuider_Point* pPoint) const { return this->dX(*pPoint); }

    double dY(const LGuider_Point& p) const {
        assert(m_valid && p.m_valid);

        double dRet = Y - p.Y;
        return dRet;
    }

    double dY(const LGuider_Point* pPoint) const { return this->dY(*pPoint); }

    double Distance(const LGuider_Point& p) const {
        double dX = this->dX(p);
        double dY = this->dY(p);
        double dRet = hypot(dX, dY);
        return dRet;
    }

    double Distance(const LGuider_Point* pPoint) const {
        return Distance(*pPoint);
    }

    double Distance() const { return hypot(X, Y); }

    double Angle(const LGuider_Point& p) const {
        double dX = this->dX(p);
        double dY = this->dY(p);
        double dRet = 0.0;

        // man pages vary on whether atan2 deals well with dx == 0 && dy == 0,
        // so I handle it explictly
        if (dX != 0 || dY != 0) {
            dRet = atan2(dY, dX);
        }

        return dRet;
    }

    double Angle() const {
        LGuider_Point origin(0, 0);

        return Angle(origin);
    }

    double Angle(const LGuider_Point* pPoint) const { return Angle(*pPoint); }

    LGuider_Point operator+(const LGuider_Point& addend) const {
        assert(m_valid && addend.m_valid);
        return LGuider_Point(this->X + addend.X, this->Y + addend.Y);
    }

    LGuider_Point& operator+=(const LGuider_Point& addend) {
        assert(m_valid && addend.m_valid);
        this->X += addend.X;
        this->Y += addend.Y;

        return *this;
    }

    LGuider_Point operator-(const LGuider_Point& subtrahend) const {
        assert(m_valid && subtrahend.m_valid);
        return LGuider_Point(this->X - subtrahend.X, this->Y - subtrahend.Y);
    }

    LGuider_Point operator-=(const LGuider_Point& subtrahend) {
        assert(m_valid && subtrahend.m_valid);
        this->X -= subtrahend.X;
        this->Y -= subtrahend.Y;

        return *this;
    }

    LGuider_Point operator/(double divisor) const {
        assert(m_valid);
        return LGuider_Point(this->X / divisor, this->Y / divisor);
    }

    LGuider_Point& operator/=(double divisor) {
        assert(m_valid);
        this->X /= divisor;
        this->Y /= divisor;

        return *this;
    }

    LGuider_Point operator*(double multiplicand) const {
        assert(m_valid);
        return LGuider_Point(this->X * multiplicand, this->Y * multiplicand);
    }

    LGuider_Point& operator*=(double multiplicand) {
        assert(m_valid);
        this->X *= multiplicand;
        this->Y *= multiplicand;

        return *this;
    }
};

class ShiftPoint : public LGuider_Point {
    typedef LGuider_Point super;

    LGuider_Point m_rate;  // rate of change (per second)
    double m_x0;           // initial x position
    double m_y0;           // initial y position
    wxLongLong_t m_t0;     // initial time (seconds)

public:
    ShiftPoint() {}

    void SetShiftRate(double xrate, double yrate) {
        m_rate.SetXY(xrate, yrate);
        BeginShift();
    }

    void BeginShift() {
        if (IsValid()) {
            m_x0 = X;
            m_y0 = Y;
            m_t0 = ::wxGetUTCTimeMillis().GetValue();
        }
    }

    void DisableShift() { m_rate.Invalidate(); }

    void UpdateShift() {
        if (IsValid() && m_rate.IsValid()) {
            double dt =
                (double)(::wxGetUTCTimeMillis().GetValue() - m_t0) / 1000.;
            X = m_x0 + m_rate.X * dt;
            Y = m_y0 + m_rate.Y * dt;
        }
    }

    void SetXY(double x, double y) {
        super::SetXY(x, y);
        BeginShift();
    }

    const LGuider_Point& ShiftRate() const { return m_rate; }
};

#endif /* POINT_H_INCLUDED */
