/** 
 * Quaternion.inl
 */

#include "Quaternion.h"

namespace gameplay
{

inline Quaternion Quaternion::operator*(const Quaternion& q)
{
    Quaternion result(*this);
    result.multiply(q);
    return result;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& q)
{
    multiply(q);
    return *this;
}

}