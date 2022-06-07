/*
  SPDX-License-Identifier: LGPL-3.0-or-later
  Original work Copyright (C) 2020 Daniel Thompson
  C++ port Copyright (C) 2021 Jean-François Milants
*/

#include "components/heartrate/Ppg.h"
#include <vector>
#include <algorithm>
#include <nrf_log.h>
using namespace Pinetime::Controllers;

/** Original implementation from wasp-os : https://github.com/daniel-thompson/wasp-os/blob/master/wasp/ppg.py */

Ppg::Ppg()
  : hpf {0.87033078, -1.74066156, 0.87033078, -1.72377617, 0.75754694},
    agc {20, 0.971, 2},
    lpf {0.11595249, 0.23190498, 0.11595249, -0.72168143, 0.18549138} {
}

int8_t Ppg::Preprocess(float spl) {
  spl -= offset;
  spl = hpf.Step(spl);
  spl = agc.Step(spl);
  spl = lpf.Step(spl);

  auto spl_int = static_cast<int8_t>(spl);

  data.Append(spl_int);
  return spl_int;
}

float Ppg::HeartRate() {
  if (!data.IsFull()){
    return PPG_UPDATE_RATE;
  }

  NRF_LOG_INFO("PREPROCESS, offset = %d", offset);

  const auto new_heartrate = ProcessHeartRate();
  if (new_heartrate > 40){
    previous_heartrates.Append(new_heartrate);
  } else {
    return last_heartrate;
  }

  if (data.index() % PPG_UPDATE_RATE == 0){
    if (previous_heartrates.IsFull()){
      std::copy(
        previous_heartrates.GetData().begin(),
        previous_heartrates.GetData().end(),
        hr_data.begin());

      std::sort(hr_data.begin(), hr_data.end());

      last_heartrate = 0;
      for (size_t i = PPG_DISCARD_OUTLIERS; i < hr_data.size() - PPG_DISCARD_OUTLIERS; ++i){
        last_heartrate += hr_data[i];
      }
      last_heartrate /= hr_data.size() - 2 * PPG_DISCARD_OUTLIERS;
    } else {
      last_heartrate = 0;
    }
  }
  return last_heartrate;
}

float Ppg::ProcessHeartRate() {
  const size_t SIZE = data.size();
  auto t0 = Trough(data, SIZE, 7, 48);
  if (t0 < 0)
    return 0;

  float t1 = t0 * 2;
  t1 = Trough(data, SIZE, t1 - 5, t1 + 5);
  if (t1 < 0)
    return 0;

  float t2 = static_cast<int>(t1 * 3) / 2;
  t2 = Trough(data, SIZE, t2 - 5, t2 + 5);
  if (t2 < 0)
    return 0;

  float t3 = static_cast<int>(t2 * 4) / 3;
  t3 = Trough(data, SIZE, t3 - 4, t3 + 4);
  if (t3 < 0)
    return static_cast<int>(60 * 24 * 3) / static_cast<int>(t2);

  return static_cast<int>(60 * 24 * 4) / static_cast<int>(t3);
}

void Ppg::SetOffset(uint16_t offset) {
  this->offset = offset;
  Reset();
}

void Ppg::Reset() {
  data.Reset();
  previous_heartrates.Reset();
}

int Ppg::Compare(const HeartRateData& data, int shift, size_t count) {
  int e = 0;
  for (size_t i = 0; i < count; i++) {
    auto d = data.Get(i + shift) - data.Get(i);
    e += d * d;
  }
  return e;
}

int Ppg::CompareShift(const HeartRateData& data, int shift, size_t count) {
  return Compare(data, shift, count - shift);
}

int Ppg::Trough(const HeartRateData& data, size_t size, uint8_t mn, uint8_t mx) {
  auto z2 = CompareShift(data, mn - 2, size);
  auto z1 = CompareShift(data, mn - 1, size);
  for (int i = mn; i < mx + 1; i++) {
    auto z = CompareShift(data, i, size);
    if (z2 > z1 && z1 < z)
      return i;
    z2 = z1;
    z1 = z;
  }
  return -1;
}
