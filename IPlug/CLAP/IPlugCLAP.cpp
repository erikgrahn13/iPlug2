/*
 ==============================================================================
 
 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers. 
 
 See LICENSE.txt for  more info.
 
 ==============================================================================
*/

#include <cstdio>
#include "IPlugCLAP.h"
#include "IPlugPluginBase.h"
#include "plugin.hxx"

using namespace iplug;

IPlugCLAP::IPlugCLAP(const InstanceInfo& info, const Config& config)
  : IPlugAPIBase(config, kAPICLAP)
  , IPlugProcessor(config, kAPICLAP)
  , clap::Plugin(info.mDesc, info.mHost)
{
  Trace(TRACELOC, "%s", config.pluginName);
  
  CreateTimer();
}

//void IPlugCLAP::BeginInformHostOfParamChange(int idx)
//{
//}
//
//void IPlugCLAP::InformHostOfParamChange(int idx, double normalizedValue)
//{
//}
//
//void IPlugCLAP::EndInformHostOfParamChange(int idx)
//{
//}
//
//void IPlugCLAP::InformHostOfPresetChange()
//{
//}
//
//bool IPlugCLAP::EditorResize(int viewWidth, int viewHeight)
//{
//  bool resized = false;
//
//  if (HasUI())
//  {
//    if (viewWidth != GetEditorWidth() || viewHeight != GetEditorHeight())
//    {
//      SetEditorSize(viewWidth, viewHeight);
//
//    }
//  }
//
//  return resized;
//}
//
//void IPlugCLAP::SetLatency(int samples)
//{
//  IPlugProcessor::SetLatency(samples);
//}
//
bool IPlugCLAP::SendMidiMsg(const IMidiMsg& msg)
{
  return false;
}
//
//bool IPlugCLAP::SendSysEx(const ISysEx& msg)
//{
//  return false;
//}

// clap_plugin

bool IPlugCLAP::init() noexcept
{
  return true;
}

bool IPlugCLAP::activate(double sampleRate) noexcept
{
  SetSampleRate(sampleRate);
  OnActivate(true);
  OnReset();

  return true;
}

void IPlugCLAP::deactivate() noexcept
{
  OnActivate(false);
}

clap_process_status IPlugCLAP::process(const clap_process *process) noexcept
{
  return CLAP_PROCESS_CONTINUE;
}

// clap_plugin_state

bool IPlugCLAP::stateSave(clap_ostream *stream) noexcept
{
  IByteChunk chunk;
  
  if (!SerializeState(chunk))
    return false;
  
  return stream->write(stream, chunk.GetData(), chunk.Size()) == chunk.Size();
}

bool IPlugCLAP::stateLoad(clap_istream *stream) noexcept
{
  constexpr int blockSize = 8192;
  
  IByteChunk chunk;
  IByteChunk blockChunk;
  
  blockChunk.Resize(blockSize);
  
  while (stream->read(stream, blockChunk.GetData(), blockSize) == blockSize)
    chunk.PutChunk(&blockChunk);

  if (stream->read(stream, blockChunk.GetData(), blockSize) != 0)
    return false;
  
  return UnserializeState(chunk, 0) >= 0;
}

// clap_plugin_params

bool IPlugCLAP::paramsInfo(int32_t paramIndex, clap_param_info *info) const noexcept
{
  assert(MAX_PARAM_NAME_LEN <= CLAP_NAME_SIZE && "iPlug parameter name size exceeds CLAP maximum");
  assert(MAX_PARAM_GROUP_LEN <= CLAP_MODULE_SIZE && "iPlug group name size exceeds CLAP maximum");

  const IParam *pParam = GetParam(paramIndex);
  
  clap_param_info_flags flags = 0;

  if (pParam->GetStepped())
    flags |= CLAP_PARAM_IS_STEPPED;
  if (pParam->GetCanAutomate())
    flags |= CLAP_PARAM_IS_MODULATABLE;
  
  info->id = paramIndex;
  info->flags = flags;
  info->cookie = nullptr;

  strcpy(info->name, pParam->GetName());
  strcpy(info->module, pParam->GetGroup());

  // Values
  
  info->min_value = pParam->GetMin();
  info->min_value = pParam->GetMax();
  info->min_value = pParam->GetDefault();
  
  return true;
}

bool IPlugCLAP::paramsValue(clap_id paramId, double *value) noexcept
{
  const IParam *pParam = GetParam(paramId);
  *value = pParam->Value();
  return true;
}

bool IPlugCLAP::paramsValueToText(clap_id paramId, double value, char *display, uint32_t size) noexcept
{
  const IParam *pParam = GetParam(paramId);
  WDL_String str;
  
  pParam->GetDisplay(value, false, str);
  
  if (size < str.GetLength())
    return false;
    
  strcpy(display, str.Get());
  return true;
}

bool IPlugCLAP::paramsTextToValue(clap_id paramId, const char *display, double *value) noexcept
{
  const IParam *pParam = GetParam(paramId);
  *value = pParam->StringToValue(display);
  return true;
}

void IPlugCLAP::paramsFlush(const clap_event_list *input_parameter_changes, const clap_event_list *output_parameter_changes) noexcept
{
  // FIX
  // ??
}
