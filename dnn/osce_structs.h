/* Copyright (c) 2023 Amazon
   Written by Jan Buethe */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef OSCE_STRUCTS_H
#define OSCE_STRUCTS_H

#include "opus_types.h"
#include "osce_config.h"
#ifndef DISABLE_LACE
#include "lace_data.h"
#endif
#ifndef DISABLE_TINYLACE
#include "tinylace_data.h"
#ifdef DISABLE_LACE
typedef TINYLACELayers LACELayers;
#define LACE_AF1_FILTER_GAIN_A TINYLACE_AF1_FILTER_GAIN_A
#define LACE_AF1_FILTER_GAIN_B TINYLACE_AF1_FILTER_GAIN_B
#define LACE_AF1_IN_CHANNELS TINYLACE_AF1_IN_CHANNELS
#define LACE_AF1_KERNEL_SIZE TINYLACE_AF1_KERNEL_SIZE
#define LACE_AF1_LEFT_PADDING TINYLACE_AF1_LEFT_PADDING
#define LACE_AF1_OUT_CHANNELS TINYLACE_AF1_OUT_CHANNELS
#define LACE_AF1_SHAPE_GAIN TINYLACE_AF1_SHAPE_GAIN
#define LACE_CF1_FILTER_GAIN_A TINYLACE_CF1_FILTER_GAIN_A
#define LACE_CF1_FILTER_GAIN_B TINYLACE_CF1_FILTER_GAIN_B
#define LACE_CF1_KERNEL_SIZE TINYLACE_CF1_KERNEL_SIZE
#define LACE_CF1_LEFT_PADDING TINYLACE_CF1_LEFT_PADDING
#define LACE_CF1_LOG_GAIN_LIMIT TINYLACE_CF1_LOG_GAIN_LIMIT
#define LACE_CF2_FILTER_GAIN_A TINYLACE_CF2_FILTER_GAIN_A
#define LACE_CF2_FILTER_GAIN_B TINYLACE_CF2_FILTER_GAIN_B
#define LACE_CF2_KERNEL_SIZE TINYLACE_CF2_KERNEL_SIZE
#define LACE_CF2_LEFT_PADDING TINYLACE_CF2_LEFT_PADDING
#define LACE_CF2_LOG_GAIN_LIMIT TINYLACE_CF2_LOG_GAIN_LIMIT
#define LACE_COND_DIM TINYLACE_COND_DIM
#define LACE_FNET_CONV2_STATE_SIZE TINYLACE_FNET_CONV2_STATE_SIZE
#define LACE_FRAME_SIZE TINYLACE_FRAME_SIZE
#define LACE_HIDDEN_FEATURE_DIM TINYLACE_HIDDEN_FEATURE_DIM
#define LACE_NUMBITS_EMBEDDING_DIM TINYLACE_NUMBITS_EMBEDDING_DIM
#define LACE_NUMBITS_RANGE_HIGH TINYLACE_NUMBITS_RANGE_HIGH
#define LACE_NUMBITS_RANGE_LOW TINYLACE_NUMBITS_RANGE_LOW
#define LACE_NUM_FEATURES TINYLACE_NUM_FEATURES
#define LACE_OVERLAP_SIZE TINYLACE_OVERLAP_SIZE
#define LACE_PITCH_EMBEDDING_DIM TINYLACE_PITCH_EMBEDDING_DIM
#define LACE_PREEMPH TINYLACE_PREEMPH
#endif
#endif
#ifndef DISABLE_NOLACE
#include "nolace_data.h"
#endif
#ifndef DISABLE_BBWENET
#include "bbwenet_data.h"
#include "resampler_structs.h"
#endif
#include "nndsp.h"
#include "nnet.h"

/* feature calculation */

typedef struct {
    float               numbits_smooth;
    int                 pitch_hangover_count;
    int                 last_lag;
    int                 last_type;
    float               signal_history[OSCE_FEATURES_MAX_HISTORY];
    int                 reset;
} OSCEFeatureState;

typedef struct {
    float signal_history[OSCE_BWE_HALF_WINDOW_SIZE];
    float last_spec[2 * OSCE_BWE_MAX_INSTAFREQ_BIN + 2];
} OSCEBWEFeatureState;

#ifndef DISABLE_BBWENET
/* BBWENet */
typedef struct {
    float upsamp_buffer[2][3];
    float interpol_buffer[8];
} resamp_state;

typedef struct {
    float feature_net_conv1_state[BBWENET_FNET_CONV1_STATE_SIZE];
    float feature_net_conv2_state[BBWENET_FNET_CONV2_STATE_SIZE];
    float feature_net_gru_state[BBWENET_FNET_GRU_STATE_SIZE];
    opus_int16 outbut_buffer[OSCE_BWE_OUTPUT_DELAY];
    AdaConvState af1_state;
    AdaConvState af2_state;
    AdaConvState af3_state;
    AdaShapeState tdshape1_state;
    AdaShapeState tdshape2_state;
    resamp_state resampler_state[3];
} BBWENetState;

typedef struct {
    BBWENETLayers layers;
    float window16[BBWENET_AF1_OVERLAP_SIZE];
    float window32[BBWENET_AF2_OVERLAP_SIZE];
    float window48[BBWENET_AF3_OVERLAP_SIZE];
} BBWENet;
#endif


#if !defined(DISABLE_LACE) || !defined(DISABLE_TINYLACE)
/* LACE */
typedef struct {
    float feature_net_conv2_state[LACE_FNET_CONV2_STATE_SIZE];
    float feature_net_gru_state[LACE_COND_DIM];
    AdaCombState cf1_state;
    AdaCombState cf2_state;
    AdaConvState af1_state;
    float preemph_mem;
    float deemph_mem;
} LACEState;

typedef struct
{
    LACELayers layers;
    float numbits_scale[LACE_NUMBITS_EMBEDDING_DIM];
    float window[LACE_OVERLAP_SIZE];
} LACE;

#endif


#ifndef DISABLE_NOLACE
/* NoLACE */
typedef struct {
    float feature_net_conv2_state[NOLACE_FNET_CONV2_STATE_SIZE];
    float feature_net_gru_state[NOLACE_COND_DIM];
    float post_cf1_state[NOLACE_COND_DIM];
    float post_cf2_state[NOLACE_COND_DIM];
    float post_af1_state[NOLACE_COND_DIM];
    float post_af2_state[NOLACE_COND_DIM];
    float post_af3_state[NOLACE_COND_DIM];
    AdaCombState cf1_state;
    AdaCombState cf2_state;
    AdaConvState af1_state;
    AdaConvState af2_state;
    AdaConvState af3_state;
    AdaConvState af4_state;
    AdaShapeState tdshape1_state;
    AdaShapeState tdshape2_state;
    AdaShapeState tdshape3_state;
    float preemph_mem;
    float deemph_mem;
} NoLACEState;

typedef struct {
    NOLACELayers layers;
    float window[NOLACE_OVERLAP_SIZE];
} NoLACE;

#endif /* #ifndef DISABLE_NOLACE */

/* OSCEModel */
typedef struct {
   int loaded;
#ifndef DISABLE_TINYLACE
    int tinylace_loaded;
    LACE tinylace;
#endif
#ifndef DISABLE_LACE
    LACE lace;
#endif
#ifndef DISABLE_NOLACE
    NoLACE nolace;
#endif
#ifndef DISABLE_BBWENET
    BBWENet bbwenet;
#endif
} OSCEModel;

typedef union {
#if !defined(DISABLE_LACE) || !defined(DISABLE_TINYLACE)
    LACEState lace;
#endif
#ifndef DISABLE_NOLACE
    NoLACEState nolace;
#endif
} OSCEState;

typedef struct {
    #ifndef DISABLE_BBWENET
    BBWENetState bbwenet;
    #endif
} OSCEBWEState;

#endif
