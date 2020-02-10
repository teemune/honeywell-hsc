/*
  GNU GPLv3 license:
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
   
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
   
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Wire.h>
#include "hsc_ssc_i2c.h"

uint8_t ps_get_raw(const uint8_t slave_addr, struct cs_raw *raw)
{
    uint8_t i, val[4] = { 0, 0, 0, 0 };
    Wire.requestFrom(slave_addr, (uint8_t) 4);
    for (i = 0; i <= 3; i++) {
        delay(4);                        // sensor might be missing, do not block
        val[i] = Wire.read();            // by using Wire.available()
    }
    raw->status = (val[0] & 0xc0) >> 6;  // first 2 bits from first byte
    raw->bridge_data = ((val[0] & 0x3f) << 8) + val[1];
    raw->temperature_data = ((val[2] << 8) + (val[3] & 0xe0)) >> 5;
    if ( raw->temperature_data == 65535 ) return 4;
    return raw->status;
}

uint8_t ps_convert(const struct cs_raw raw, float *pressure, float *temperature,
                   const uint16_t output_min, const uint16_t output_max, const float pressure_min,
                   const float pressure_max)
{
    *pressure = 1.0 * (raw.bridge_data - output_min) * (pressure_max - pressure_min) / (output_max - output_min) + pressure_min;
    *temperature = (raw.temperature_data * 0.0977) - 50;
    return 0;
}
