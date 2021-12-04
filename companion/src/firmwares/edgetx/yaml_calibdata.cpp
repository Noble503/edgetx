/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "yaml_calibdata.h"

YamlCalibData::YamlCalibData() { memset(calib, 0, sizeof(calib)); }

YamlCalibData::YamlCalibData(const int* calibMid, const int* calibSpanNeg,
                             const int* calibSpanPos)
{
  for (int i = 0; i < CPN_MAX_ANALOGS; i++) {
    calib[i].mid = calibMid[i];
    calib[i].spanNeg = calibSpanNeg[i];
    calib[i].spanPos = calibSpanPos[i];
  }
}

void YamlCalibData::copy(int* calibMid, int* calibSpanNeg,
                         int* calibSpanPos) const
{
  for (int i = 0; i < CPN_MAX_ANALOGS; i++) {
    calibMid[i] = calib[i].mid;
    calibSpanNeg[i] = calib[i].spanNeg;
    calibSpanPos[i] = calib[i].spanPos;
  }
}

namespace YAML
{

Node convert<CalibData>::encode(const CalibData& rhs)
{
  Node node;
  node["mid"] = rhs.mid;
  node["spanNeg"] = rhs.spanNeg;
  node["spanPos"] = rhs.spanPos;
  return node;
}

bool convert<CalibData>::decode(const Node& node, CalibData& rhs)
{
  if (!node.IsMap()) return false;
  node["mid"] >> rhs.mid;
  node["spanNeg"] >> rhs.spanNeg;
  node["spanPos"] >> rhs.spanPos;
  return true;
}

Node convert<YamlCalibData>::encode(const YamlCalibData& rhs)
{
  Node node;
  const YamlLookupTable* calibIdxLut = getCurrentFirmware()->getAnalogIndexNamesLookupTable();
  // TODO: we need something better here!
  for (const auto& kv : *calibIdxLut) {
    node[kv.second] = rhs.calib[kv.first];
  }
  return node;
}

bool convert<YamlCalibData>::decode(const Node& node, YamlCalibData& rhs)
{
  if (!node.IsMap()) return false;
  const YamlLookupTable* calibIdxLut = getCurrentFirmware()->getAnalogIndexNamesLookupTable();
  //for_each(calibIdxLut->begin(), calibIdxLut->end(), [=](const Int2StringMapping& row) {
  //    qDebug() << "index:" << row.first << "value:" << row.second.c_str();
  //  });
  int idx = 0;
  for (const auto& kv : node) {
    kv.first >> *calibIdxLut >> idx;
    kv.second >> rhs.calib[idx];
  }
  return true;
}

}  // namespace YAML