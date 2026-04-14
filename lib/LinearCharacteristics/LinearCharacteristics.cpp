#include "LinearCharacteristics.h"

using namespace std;

LinearCharacteristics::LinearCharacteristics(float gain, float offset)
{
    m_gain = gain;
    m_offset = offset;
    m_ulim = 999999.0f;  // a large number
    m_llim = -999999.0f; // a large neg. number
}

LinearCharacteristics::LinearCharacteristics(float xmin, float xmax, float ymin, float ymax)
{
    // calculate gain and offset from min/max values
    // y = gain * (x - offset)
    // mapping: xmin -> ymin, xmax -> ymax
    m_gain = (ymax - ymin) / (xmax - xmin);
    m_offset = xmin - ymin / m_gain;
    m_ulim = 999999.0f;  // a large number
    m_llim = -999999.0f; // a large neg. number
}

LinearCharacteristics::~LinearCharacteristics() {}

float LinearCharacteristics::evaluate(float x)
{
    // calculate result as y(x) = gain * (x - offset)
    float y = m_gain * (x - m_offset);
    
    // apply saturation limits
    if (y > m_ulim) {
        y = m_ulim;
    } else if (y < m_llim) {
        y = m_llim;
    }
    
    return y;
}

void LinearCharacteristics::set_limits(float ll, float ul)
{
    m_llim = ll;
    m_ulim = ul;
}
