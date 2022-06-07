#pragma once

#include <cstddef>
#include <cstdint>
#include "components/heartrate/Biquad.h"
#include "components/heartrate/Ptagc.h"
#include "CircularBuffer.h"

#define PPG_UPDATE_RATE 50
#define PPG_AVERAGING 10
#define PPG_DISCARD_OUTLIERS 2
static_assert(PPG_AVERAGING - 2 * PPG_DISCARD_OUTLIERS > 0);

namespace Pinetime {
  namespace Controllers {
    typedef CircularBuffer<int8_t, 200> HeartRateData;

    class Ppg {
      public:
        Ppg();
        int8_t Preprocess(float spl);
        float HeartRate();

        void SetOffset(uint16_t i);
        void Reset();

      private:
        HeartRateData data;
        CircularBuffer<float, PPG_AVERAGING> previous_heartrates;

        std::array<float, PPG_AVERAGING> hr_data;
        float last_heartrate = 0.0f;
        float offset;

        Biquad hpf;
        Ptagc agc;
        Biquad lpf;

        float ProcessHeartRate();

        int Compare(const HeartRateData& data, int shift, size_t count);
        int CompareShift(const HeartRateData& data, int shift, size_t count);
        int Trough(const HeartRateData& data, size_t size, uint8_t mn, uint8_t mx);
    };
  }
}
