#ifndef AOM_DSP_RTCD_H_
#define AOM_DSP_RTCD_H_

#ifdef RTCD_C
#define RTCD_EXTERN
#else
#define RTCD_EXTERN extern
#endif

/*
 * DSP
 */

#include "aom/aom_integer.h"
#include "aom_dsp/aom_dsp_common.h"
#include "av1/common/enums.h"

#ifdef __cplusplus
extern "C" {
#endif

void aom_blend_a64_d32_mask_c(int32_t* dst,
                              uint32_t dst_stride,
                              const int32_t* src0,
                              uint32_t src0_stride,
                              const int32_t* src1,
                              uint32_t src1_stride,
                              const uint8_t* mask,
                              uint32_t mask_stride,
                              int h,
                              int w,
                              int suby,
                              int subx);
#define aom_blend_a64_d32_mask aom_blend_a64_d32_mask_c

void aom_blend_a64_hmask_c(uint8_t* dst,
                           uint32_t dst_stride,
                           const uint8_t* src0,
                           uint32_t src0_stride,
                           const uint8_t* src1,
                           uint32_t src1_stride,
                           const uint8_t* mask,
                           int h,
                           int w);
#define aom_blend_a64_hmask aom_blend_a64_hmask_c

void aom_blend_a64_mask_c(uint8_t* dst,
                          uint32_t dst_stride,
                          const uint8_t* src0,
                          uint32_t src0_stride,
                          const uint8_t* src1,
                          uint32_t src1_stride,
                          const uint8_t* mask,
                          uint32_t mask_stride,
                          int h,
                          int w,
                          int suby,
                          int subx);
#define aom_blend_a64_mask aom_blend_a64_mask_c

void aom_blend_a64_vmask_c(uint8_t* dst,
                           uint32_t dst_stride,
                           const uint8_t* src0,
                           uint32_t src0_stride,
                           const uint8_t* src1,
                           uint32_t src1_stride,
                           const uint8_t* mask,
                           int h,
                           int w);
#define aom_blend_a64_vmask aom_blend_a64_vmask_c

void aom_convolve8_c(const uint8_t* src,
                     ptrdiff_t src_stride,
                     uint8_t* dst,
                     ptrdiff_t dst_stride,
                     const int16_t* filter_x,
                     int x_step_q4,
                     const int16_t* filter_y,
                     int y_step_q4,
                     int w,
                     int h);
#define aom_convolve8 aom_convolve8_c

void aom_convolve8_avg_c(const uint8_t* src,
                         ptrdiff_t src_stride,
                         uint8_t* dst,
                         ptrdiff_t dst_stride,
                         const int16_t* filter_x,
                         int x_step_q4,
                         const int16_t* filter_y,
                         int y_step_q4,
                         int w,
                         int h);
#define aom_convolve8_avg aom_convolve8_avg_c

void aom_convolve8_avg_horiz_c(const uint8_t* src,
                               ptrdiff_t src_stride,
                               uint8_t* dst,
                               ptrdiff_t dst_stride,
                               const int16_t* filter_x,
                               int x_step_q4,
                               const int16_t* filter_y,
                               int y_step_q4,
                               int w,
                               int h);
#define aom_convolve8_avg_horiz aom_convolve8_avg_horiz_c

void aom_convolve8_avg_horiz_scale_c(const uint8_t* src,
                                     ptrdiff_t src_stride,
                                     uint8_t* dst,
                                     ptrdiff_t dst_stride,
                                     const int16_t* filter_x,
                                     int subpel_x,
                                     int x_step_q4,
                                     const int16_t* filter_y,
                                     int subpel_y,
                                     int y_step_q4,
                                     int w,
                                     int h);
#define aom_convolve8_avg_horiz_scale aom_convolve8_avg_horiz_scale_c

void aom_convolve8_avg_scale_c(const uint8_t* src,
                               ptrdiff_t src_stride,
                               uint8_t* dst,
                               ptrdiff_t dst_stride,
                               const int16_t* filter_x,
                               int subpel_x,
                               int x_step_q4,
                               const int16_t* filter_y,
                               int subpel_y,
                               int y_step_q4,
                               int w,
                               int h);
#define aom_convolve8_avg_scale aom_convolve8_avg_scale_c

void aom_convolve8_avg_vert_c(const uint8_t* src,
                              ptrdiff_t src_stride,
                              uint8_t* dst,
                              ptrdiff_t dst_stride,
                              const int16_t* filter_x,
                              int x_step_q4,
                              const int16_t* filter_y,
                              int y_step_q4,
                              int w,
                              int h);
#define aom_convolve8_avg_vert aom_convolve8_avg_vert_c

void aom_convolve8_avg_vert_scale_c(const uint8_t* src,
                                    ptrdiff_t src_stride,
                                    uint8_t* dst,
                                    ptrdiff_t dst_stride,
                                    const int16_t* filter_x,
                                    int subpel_x,
                                    int x_step_q4,
                                    const int16_t* filter_y,
                                    int subpel_y,
                                    int y_step_q4,
                                    int w,
                                    int h);
#define aom_convolve8_avg_vert_scale aom_convolve8_avg_vert_scale_c

void aom_convolve8_horiz_c(const uint8_t* src,
                           ptrdiff_t src_stride,
                           uint8_t* dst,
                           ptrdiff_t dst_stride,
                           const int16_t* filter_x,
                           int x_step_q4,
                           const int16_t* filter_y,
                           int y_step_q4,
                           int w,
                           int h);
#define aom_convolve8_horiz aom_convolve8_horiz_c

void aom_convolve8_horiz_scale_c(const uint8_t* src,
                                 ptrdiff_t src_stride,
                                 uint8_t* dst,
                                 ptrdiff_t dst_stride,
                                 const int16_t* filter_x,
                                 int subpel_x,
                                 int x_step_q4,
                                 const int16_t* filter_y,
                                 int subpel_y,
                                 int y_step_q4,
                                 int w,
                                 int h);
#define aom_convolve8_horiz_scale aom_convolve8_horiz_scale_c

void aom_convolve8_scale_c(const uint8_t* src,
                           ptrdiff_t src_stride,
                           uint8_t* dst,
                           ptrdiff_t dst_stride,
                           const int16_t* filter_x,
                           int subpel_x,
                           int x_step_q4,
                           const int16_t* filter_y,
                           int subpel_y,
                           int y_step_q4,
                           int w,
                           int h);
#define aom_convolve8_scale aom_convolve8_scale_c

void aom_convolve8_vert_c(const uint8_t* src,
                          ptrdiff_t src_stride,
                          uint8_t* dst,
                          ptrdiff_t dst_stride,
                          const int16_t* filter_x,
                          int x_step_q4,
                          const int16_t* filter_y,
                          int y_step_q4,
                          int w,
                          int h);
#define aom_convolve8_vert aom_convolve8_vert_c

void aom_convolve8_vert_scale_c(const uint8_t* src,
                                ptrdiff_t src_stride,
                                uint8_t* dst,
                                ptrdiff_t dst_stride,
                                const int16_t* filter_x,
                                int subpel_x,
                                int x_step_q4,
                                const int16_t* filter_y,
                                int subpel_y,
                                int y_step_q4,
                                int w,
                                int h);
#define aom_convolve8_vert_scale aom_convolve8_vert_scale_c

void aom_convolve_avg_c(const uint8_t* src,
                        ptrdiff_t src_stride,
                        uint8_t* dst,
                        ptrdiff_t dst_stride,
                        const int16_t* filter_x,
                        int x_step_q4,
                        const int16_t* filter_y,
                        int y_step_q4,
                        int w,
                        int h);
#define aom_convolve_avg aom_convolve_avg_c

void aom_convolve_copy_c(const uint8_t* src,
                         ptrdiff_t src_stride,
                         uint8_t* dst,
                         ptrdiff_t dst_stride,
                         const int16_t* filter_x,
                         int x_step_q4,
                         const int16_t* filter_y,
                         int y_step_q4,
                         int w,
                         int h);
#define aom_convolve_copy aom_convolve_copy_c

void aom_d117_predictor_16x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d117_predictor_16x16 aom_d117_predictor_16x16_c

void aom_d117_predictor_16x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d117_predictor_16x32 aom_d117_predictor_16x32_c

void aom_d117_predictor_16x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d117_predictor_16x8 aom_d117_predictor_16x8_c

void aom_d117_predictor_2x2_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d117_predictor_2x2 aom_d117_predictor_2x2_c

void aom_d117_predictor_32x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d117_predictor_32x16 aom_d117_predictor_32x16_c

void aom_d117_predictor_32x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d117_predictor_32x32 aom_d117_predictor_32x32_c

void aom_d117_predictor_4x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d117_predictor_4x4 aom_d117_predictor_4x4_c

void aom_d117_predictor_4x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d117_predictor_4x8 aom_d117_predictor_4x8_c

void aom_d117_predictor_8x16_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d117_predictor_8x16 aom_d117_predictor_8x16_c

void aom_d117_predictor_8x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d117_predictor_8x4 aom_d117_predictor_8x4_c

void aom_d117_predictor_8x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d117_predictor_8x8 aom_d117_predictor_8x8_c

void aom_d135_predictor_16x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d135_predictor_16x16 aom_d135_predictor_16x16_c

void aom_d135_predictor_16x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d135_predictor_16x32 aom_d135_predictor_16x32_c

void aom_d135_predictor_16x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d135_predictor_16x8 aom_d135_predictor_16x8_c

void aom_d135_predictor_2x2_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d135_predictor_2x2 aom_d135_predictor_2x2_c

void aom_d135_predictor_32x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d135_predictor_32x16 aom_d135_predictor_32x16_c

void aom_d135_predictor_32x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d135_predictor_32x32 aom_d135_predictor_32x32_c

void aom_d135_predictor_4x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d135_predictor_4x4 aom_d135_predictor_4x4_c

void aom_d135_predictor_4x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d135_predictor_4x8 aom_d135_predictor_4x8_c

void aom_d135_predictor_8x16_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d135_predictor_8x16 aom_d135_predictor_8x16_c

void aom_d135_predictor_8x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d135_predictor_8x4 aom_d135_predictor_8x4_c

void aom_d135_predictor_8x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d135_predictor_8x8 aom_d135_predictor_8x8_c

void aom_d153_predictor_16x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d153_predictor_16x16 aom_d153_predictor_16x16_c

void aom_d153_predictor_16x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d153_predictor_16x32 aom_d153_predictor_16x32_c

void aom_d153_predictor_16x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d153_predictor_16x8 aom_d153_predictor_16x8_c

void aom_d153_predictor_2x2_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d153_predictor_2x2 aom_d153_predictor_2x2_c

void aom_d153_predictor_32x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d153_predictor_32x16 aom_d153_predictor_32x16_c

void aom_d153_predictor_32x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d153_predictor_32x32 aom_d153_predictor_32x32_c

void aom_d153_predictor_4x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d153_predictor_4x4 aom_d153_predictor_4x4_c

void aom_d153_predictor_4x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d153_predictor_4x8 aom_d153_predictor_4x8_c

void aom_d153_predictor_8x16_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d153_predictor_8x16 aom_d153_predictor_8x16_c

void aom_d153_predictor_8x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d153_predictor_8x4 aom_d153_predictor_8x4_c

void aom_d153_predictor_8x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d153_predictor_8x8 aom_d153_predictor_8x8_c

void aom_d207e_predictor_16x16_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_d207e_predictor_16x16 aom_d207e_predictor_16x16_c

void aom_d207e_predictor_16x32_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_d207e_predictor_16x32 aom_d207e_predictor_16x32_c

void aom_d207e_predictor_16x8_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d207e_predictor_16x8 aom_d207e_predictor_16x8_c

void aom_d207e_predictor_2x2_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d207e_predictor_2x2 aom_d207e_predictor_2x2_c

void aom_d207e_predictor_32x16_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_d207e_predictor_32x16 aom_d207e_predictor_32x16_c

void aom_d207e_predictor_32x32_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_d207e_predictor_32x32 aom_d207e_predictor_32x32_c

void aom_d207e_predictor_4x4_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d207e_predictor_4x4 aom_d207e_predictor_4x4_c

void aom_d207e_predictor_4x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d207e_predictor_4x8 aom_d207e_predictor_4x8_c

void aom_d207e_predictor_8x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d207e_predictor_8x16 aom_d207e_predictor_8x16_c

void aom_d207e_predictor_8x4_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d207e_predictor_8x4 aom_d207e_predictor_8x4_c

void aom_d207e_predictor_8x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d207e_predictor_8x8 aom_d207e_predictor_8x8_c

void aom_d45e_predictor_16x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d45e_predictor_16x16 aom_d45e_predictor_16x16_c

void aom_d45e_predictor_16x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d45e_predictor_16x32 aom_d45e_predictor_16x32_c

void aom_d45e_predictor_16x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d45e_predictor_16x8 aom_d45e_predictor_16x8_c

void aom_d45e_predictor_2x2_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d45e_predictor_2x2 aom_d45e_predictor_2x2_c

void aom_d45e_predictor_32x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d45e_predictor_32x16 aom_d45e_predictor_32x16_c

void aom_d45e_predictor_32x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d45e_predictor_32x32 aom_d45e_predictor_32x32_c

void aom_d45e_predictor_4x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d45e_predictor_4x4 aom_d45e_predictor_4x4_c

void aom_d45e_predictor_4x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d45e_predictor_4x8 aom_d45e_predictor_4x8_c

void aom_d45e_predictor_8x16_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d45e_predictor_8x16 aom_d45e_predictor_8x16_c

void aom_d45e_predictor_8x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d45e_predictor_8x4 aom_d45e_predictor_8x4_c

void aom_d45e_predictor_8x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d45e_predictor_8x8 aom_d45e_predictor_8x8_c

void aom_d63e_predictor_16x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d63e_predictor_16x16 aom_d63e_predictor_16x16_c

void aom_d63e_predictor_16x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d63e_predictor_16x32 aom_d63e_predictor_16x32_c

void aom_d63e_predictor_16x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d63e_predictor_16x8 aom_d63e_predictor_16x8_c

void aom_d63e_predictor_2x2_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d63e_predictor_2x2 aom_d63e_predictor_2x2_c

void aom_d63e_predictor_32x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d63e_predictor_32x16 aom_d63e_predictor_32x16_c

void aom_d63e_predictor_32x32_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_d63e_predictor_32x32 aom_d63e_predictor_32x32_c

void aom_d63e_predictor_4x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d63e_predictor_4x4 aom_d63e_predictor_4x4_c

void aom_d63e_predictor_4x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d63e_predictor_4x8 aom_d63e_predictor_4x8_c

void aom_d63e_predictor_8x16_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_d63e_predictor_8x16 aom_d63e_predictor_8x16_c

void aom_d63e_predictor_8x4_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d63e_predictor_8x4 aom_d63e_predictor_8x4_c

void aom_d63e_predictor_8x8_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_d63e_predictor_8x8 aom_d63e_predictor_8x8_c

void aom_dc_128_predictor_16x16_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_128_predictor_16x16 aom_dc_128_predictor_16x16_c

void aom_dc_128_predictor_16x32_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_128_predictor_16x32 aom_dc_128_predictor_16x32_c

void aom_dc_128_predictor_16x8_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_128_predictor_16x8 aom_dc_128_predictor_16x8_c

void aom_dc_128_predictor_2x2_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_128_predictor_2x2 aom_dc_128_predictor_2x2_c

void aom_dc_128_predictor_32x16_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_128_predictor_32x16 aom_dc_128_predictor_32x16_c

void aom_dc_128_predictor_32x32_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_128_predictor_32x32 aom_dc_128_predictor_32x32_c

void aom_dc_128_predictor_4x4_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_128_predictor_4x4 aom_dc_128_predictor_4x4_c

void aom_dc_128_predictor_4x8_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_128_predictor_4x8 aom_dc_128_predictor_4x8_c

void aom_dc_128_predictor_8x16_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_128_predictor_8x16 aom_dc_128_predictor_8x16_c

void aom_dc_128_predictor_8x4_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_128_predictor_8x4 aom_dc_128_predictor_8x4_c

void aom_dc_128_predictor_8x8_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_128_predictor_8x8 aom_dc_128_predictor_8x8_c

void aom_dc_left_predictor_16x16_c(uint8_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint8_t* above,
                                   const uint8_t* left);
#define aom_dc_left_predictor_16x16 aom_dc_left_predictor_16x16_c

void aom_dc_left_predictor_16x32_c(uint8_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint8_t* above,
                                   const uint8_t* left);
#define aom_dc_left_predictor_16x32 aom_dc_left_predictor_16x32_c

void aom_dc_left_predictor_16x8_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_left_predictor_16x8 aom_dc_left_predictor_16x8_c

void aom_dc_left_predictor_2x2_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_left_predictor_2x2 aom_dc_left_predictor_2x2_c

void aom_dc_left_predictor_32x16_c(uint8_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint8_t* above,
                                   const uint8_t* left);
#define aom_dc_left_predictor_32x16 aom_dc_left_predictor_32x16_c

void aom_dc_left_predictor_32x32_c(uint8_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint8_t* above,
                                   const uint8_t* left);
#define aom_dc_left_predictor_32x32 aom_dc_left_predictor_32x32_c

void aom_dc_left_predictor_4x4_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_left_predictor_4x4 aom_dc_left_predictor_4x4_c

void aom_dc_left_predictor_4x8_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_left_predictor_4x8 aom_dc_left_predictor_4x8_c

void aom_dc_left_predictor_8x16_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_left_predictor_8x16 aom_dc_left_predictor_8x16_c

void aom_dc_left_predictor_8x4_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_left_predictor_8x4 aom_dc_left_predictor_8x4_c

void aom_dc_left_predictor_8x8_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_left_predictor_8x8 aom_dc_left_predictor_8x8_c

void aom_dc_predictor_16x16_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_dc_predictor_16x16 aom_dc_predictor_16x16_c

void aom_dc_predictor_16x32_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_dc_predictor_16x32 aom_dc_predictor_16x32_c

void aom_dc_predictor_16x8_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_dc_predictor_16x8 aom_dc_predictor_16x8_c

void aom_dc_predictor_2x2_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_dc_predictor_2x2 aom_dc_predictor_2x2_c

void aom_dc_predictor_32x16_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_dc_predictor_32x16 aom_dc_predictor_32x16_c

void aom_dc_predictor_32x32_c(uint8_t* dst,
                              ptrdiff_t y_stride,
                              const uint8_t* above,
                              const uint8_t* left);
#define aom_dc_predictor_32x32 aom_dc_predictor_32x32_c

void aom_dc_predictor_4x4_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_dc_predictor_4x4 aom_dc_predictor_4x4_c

void aom_dc_predictor_4x8_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_dc_predictor_4x8 aom_dc_predictor_4x8_c

void aom_dc_predictor_8x16_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_dc_predictor_8x16 aom_dc_predictor_8x16_c

void aom_dc_predictor_8x4_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_dc_predictor_8x4 aom_dc_predictor_8x4_c

void aom_dc_predictor_8x8_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_dc_predictor_8x8 aom_dc_predictor_8x8_c

void aom_dc_top_predictor_16x16_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_top_predictor_16x16 aom_dc_top_predictor_16x16_c

void aom_dc_top_predictor_16x32_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_top_predictor_16x32 aom_dc_top_predictor_16x32_c

void aom_dc_top_predictor_16x8_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_top_predictor_16x8 aom_dc_top_predictor_16x8_c

void aom_dc_top_predictor_2x2_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_top_predictor_2x2 aom_dc_top_predictor_2x2_c

void aom_dc_top_predictor_32x16_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_top_predictor_32x16 aom_dc_top_predictor_32x16_c

void aom_dc_top_predictor_32x32_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_dc_top_predictor_32x32 aom_dc_top_predictor_32x32_c

void aom_dc_top_predictor_4x4_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_top_predictor_4x4 aom_dc_top_predictor_4x4_c

void aom_dc_top_predictor_4x8_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_top_predictor_4x8 aom_dc_top_predictor_4x8_c

void aom_dc_top_predictor_8x16_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_dc_top_predictor_8x16 aom_dc_top_predictor_8x16_c

void aom_dc_top_predictor_8x4_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_top_predictor_8x4 aom_dc_top_predictor_8x4_c

void aom_dc_top_predictor_8x8_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_dc_top_predictor_8x8 aom_dc_top_predictor_8x8_c

void aom_h_predictor_16x16_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_h_predictor_16x16 aom_h_predictor_16x16_c

void aom_h_predictor_16x32_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_h_predictor_16x32 aom_h_predictor_16x32_c

void aom_h_predictor_16x8_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_h_predictor_16x8 aom_h_predictor_16x8_c

void aom_h_predictor_2x2_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_h_predictor_2x2 aom_h_predictor_2x2_c

void aom_h_predictor_32x16_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_h_predictor_32x16 aom_h_predictor_32x16_c

void aom_h_predictor_32x32_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_h_predictor_32x32 aom_h_predictor_32x32_c

void aom_h_predictor_4x4_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_h_predictor_4x4 aom_h_predictor_4x4_c

void aom_h_predictor_4x8_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_h_predictor_4x8 aom_h_predictor_4x8_c

void aom_h_predictor_8x16_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_h_predictor_8x16 aom_h_predictor_8x16_c

void aom_h_predictor_8x4_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_h_predictor_8x4 aom_h_predictor_8x4_c

void aom_h_predictor_8x8_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_h_predictor_8x8 aom_h_predictor_8x8_c

void aom_highbd_blend_a64_hmask_c(uint8_t* dst,
                                  uint32_t dst_stride,
                                  const uint8_t* src0,
                                  uint32_t src0_stride,
                                  const uint8_t* src1,
                                  uint32_t src1_stride,
                                  const uint8_t* mask,
                                  int h,
                                  int w,
                                  int bd);
#define aom_highbd_blend_a64_hmask aom_highbd_blend_a64_hmask_c

void aom_highbd_blend_a64_mask_c(uint8_t* dst,
                                 uint32_t dst_stride,
                                 const uint8_t* src0,
                                 uint32_t src0_stride,
                                 const uint8_t* src1,
                                 uint32_t src1_stride,
                                 const uint8_t* mask,
                                 uint32_t mask_stride,
                                 int h,
                                 int w,
                                 int suby,
                                 int subx,
                                 int bd);
#define aom_highbd_blend_a64_mask aom_highbd_blend_a64_mask_c

void aom_highbd_blend_a64_vmask_c(uint8_t* dst,
                                  uint32_t dst_stride,
                                  const uint8_t* src0,
                                  uint32_t src0_stride,
                                  const uint8_t* src1,
                                  uint32_t src1_stride,
                                  const uint8_t* mask,
                                  int h,
                                  int w,
                                  int bd);
#define aom_highbd_blend_a64_vmask aom_highbd_blend_a64_vmask_c

void aom_highbd_convolve8_c(const uint8_t* src,
                            ptrdiff_t src_stride,
                            uint8_t* dst,
                            ptrdiff_t dst_stride,
                            const int16_t* filter_x,
                            int x_step_q4,
                            const int16_t* filter_y,
                            int y_step_q4,
                            int w,
                            int h,
                            int bps);
#define aom_highbd_convolve8 aom_highbd_convolve8_c

void aom_highbd_convolve8_avg_c(const uint8_t* src,
                                ptrdiff_t src_stride,
                                uint8_t* dst,
                                ptrdiff_t dst_stride,
                                const int16_t* filter_x,
                                int x_step_q4,
                                const int16_t* filter_y,
                                int y_step_q4,
                                int w,
                                int h,
                                int bps);
#define aom_highbd_convolve8_avg aom_highbd_convolve8_avg_c

void aom_highbd_convolve8_avg_horiz_c(const uint8_t* src,
                                      ptrdiff_t src_stride,
                                      uint8_t* dst,
                                      ptrdiff_t dst_stride,
                                      const int16_t* filter_x,
                                      int x_step_q4,
                                      const int16_t* filter_y,
                                      int y_step_q4,
                                      int w,
                                      int h,
                                      int bps);
#define aom_highbd_convolve8_avg_horiz aom_highbd_convolve8_avg_horiz_c

void aom_highbd_convolve8_avg_vert_c(const uint8_t* src,
                                     ptrdiff_t src_stride,
                                     uint8_t* dst,
                                     ptrdiff_t dst_stride,
                                     const int16_t* filter_x,
                                     int x_step_q4,
                                     const int16_t* filter_y,
                                     int y_step_q4,
                                     int w,
                                     int h,
                                     int bps);
#define aom_highbd_convolve8_avg_vert aom_highbd_convolve8_avg_vert_c

void aom_highbd_convolve8_horiz_c(const uint8_t* src,
                                  ptrdiff_t src_stride,
                                  uint8_t* dst,
                                  ptrdiff_t dst_stride,
                                  const int16_t* filter_x,
                                  int x_step_q4,
                                  const int16_t* filter_y,
                                  int y_step_q4,
                                  int w,
                                  int h,
                                  int bps);
#define aom_highbd_convolve8_horiz aom_highbd_convolve8_horiz_c

void aom_highbd_convolve8_vert_c(const uint8_t* src,
                                 ptrdiff_t src_stride,
                                 uint8_t* dst,
                                 ptrdiff_t dst_stride,
                                 const int16_t* filter_x,
                                 int x_step_q4,
                                 const int16_t* filter_y,
                                 int y_step_q4,
                                 int w,
                                 int h,
                                 int bps);
#define aom_highbd_convolve8_vert aom_highbd_convolve8_vert_c

void aom_highbd_convolve_avg_c(const uint8_t* src,
                               ptrdiff_t src_stride,
                               uint8_t* dst,
                               ptrdiff_t dst_stride,
                               const int16_t* filter_x,
                               int x_step_q4,
                               const int16_t* filter_y,
                               int y_step_q4,
                               int w,
                               int h,
                               int bps);
#define aom_highbd_convolve_avg aom_highbd_convolve_avg_c

void aom_highbd_convolve_copy_c(const uint8_t* src,
                                ptrdiff_t src_stride,
                                uint8_t* dst,
                                ptrdiff_t dst_stride,
                                const int16_t* filter_x,
                                int x_step_q4,
                                const int16_t* filter_y,
                                int y_step_q4,
                                int w,
                                int h,
                                int bps);
#define aom_highbd_convolve_copy aom_highbd_convolve_copy_c

void aom_highbd_d117_predictor_16x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d117_predictor_16x16 aom_highbd_d117_predictor_16x16_c

void aom_highbd_d117_predictor_16x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d117_predictor_16x32 aom_highbd_d117_predictor_16x32_c

void aom_highbd_d117_predictor_16x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d117_predictor_16x8 aom_highbd_d117_predictor_16x8_c

void aom_highbd_d117_predictor_2x2_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d117_predictor_2x2 aom_highbd_d117_predictor_2x2_c

void aom_highbd_d117_predictor_32x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d117_predictor_32x16 aom_highbd_d117_predictor_32x16_c

void aom_highbd_d117_predictor_32x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d117_predictor_32x32 aom_highbd_d117_predictor_32x32_c

void aom_highbd_d117_predictor_4x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d117_predictor_4x4 aom_highbd_d117_predictor_4x4_c

void aom_highbd_d117_predictor_4x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d117_predictor_4x8 aom_highbd_d117_predictor_4x8_c

void aom_highbd_d117_predictor_8x16_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d117_predictor_8x16 aom_highbd_d117_predictor_8x16_c

void aom_highbd_d117_predictor_8x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d117_predictor_8x4 aom_highbd_d117_predictor_8x4_c

void aom_highbd_d117_predictor_8x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d117_predictor_8x8 aom_highbd_d117_predictor_8x8_c

void aom_highbd_d135_predictor_16x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d135_predictor_16x16 aom_highbd_d135_predictor_16x16_c

void aom_highbd_d135_predictor_16x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d135_predictor_16x32 aom_highbd_d135_predictor_16x32_c

void aom_highbd_d135_predictor_16x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d135_predictor_16x8 aom_highbd_d135_predictor_16x8_c

void aom_highbd_d135_predictor_2x2_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d135_predictor_2x2 aom_highbd_d135_predictor_2x2_c

void aom_highbd_d135_predictor_32x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d135_predictor_32x16 aom_highbd_d135_predictor_32x16_c

void aom_highbd_d135_predictor_32x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d135_predictor_32x32 aom_highbd_d135_predictor_32x32_c

void aom_highbd_d135_predictor_4x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d135_predictor_4x4 aom_highbd_d135_predictor_4x4_c

void aom_highbd_d135_predictor_4x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d135_predictor_4x8 aom_highbd_d135_predictor_4x8_c

void aom_highbd_d135_predictor_8x16_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d135_predictor_8x16 aom_highbd_d135_predictor_8x16_c

void aom_highbd_d135_predictor_8x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d135_predictor_8x4 aom_highbd_d135_predictor_8x4_c

void aom_highbd_d135_predictor_8x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d135_predictor_8x8 aom_highbd_d135_predictor_8x8_c

void aom_highbd_d153_predictor_16x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d153_predictor_16x16 aom_highbd_d153_predictor_16x16_c

void aom_highbd_d153_predictor_16x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d153_predictor_16x32 aom_highbd_d153_predictor_16x32_c

void aom_highbd_d153_predictor_16x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d153_predictor_16x8 aom_highbd_d153_predictor_16x8_c

void aom_highbd_d153_predictor_2x2_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d153_predictor_2x2 aom_highbd_d153_predictor_2x2_c

void aom_highbd_d153_predictor_32x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d153_predictor_32x16 aom_highbd_d153_predictor_32x16_c

void aom_highbd_d153_predictor_32x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d153_predictor_32x32 aom_highbd_d153_predictor_32x32_c

void aom_highbd_d153_predictor_4x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d153_predictor_4x4 aom_highbd_d153_predictor_4x4_c

void aom_highbd_d153_predictor_4x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d153_predictor_4x8 aom_highbd_d153_predictor_4x8_c

void aom_highbd_d153_predictor_8x16_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d153_predictor_8x16 aom_highbd_d153_predictor_8x16_c

void aom_highbd_d153_predictor_8x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d153_predictor_8x4 aom_highbd_d153_predictor_8x4_c

void aom_highbd_d153_predictor_8x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d153_predictor_8x8 aom_highbd_d153_predictor_8x8_c

void aom_highbd_d207e_predictor_16x16_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_d207e_predictor_16x16 aom_highbd_d207e_predictor_16x16_c

void aom_highbd_d207e_predictor_16x32_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_d207e_predictor_16x32 aom_highbd_d207e_predictor_16x32_c

void aom_highbd_d207e_predictor_16x8_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d207e_predictor_16x8 aom_highbd_d207e_predictor_16x8_c

void aom_highbd_d207e_predictor_2x2_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d207e_predictor_2x2 aom_highbd_d207e_predictor_2x2_c

void aom_highbd_d207e_predictor_32x16_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_d207e_predictor_32x16 aom_highbd_d207e_predictor_32x16_c

void aom_highbd_d207e_predictor_32x32_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_d207e_predictor_32x32 aom_highbd_d207e_predictor_32x32_c

void aom_highbd_d207e_predictor_4x4_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d207e_predictor_4x4 aom_highbd_d207e_predictor_4x4_c

void aom_highbd_d207e_predictor_4x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d207e_predictor_4x8 aom_highbd_d207e_predictor_4x8_c

void aom_highbd_d207e_predictor_8x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d207e_predictor_8x16 aom_highbd_d207e_predictor_8x16_c

void aom_highbd_d207e_predictor_8x4_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d207e_predictor_8x4 aom_highbd_d207e_predictor_8x4_c

void aom_highbd_d207e_predictor_8x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d207e_predictor_8x8 aom_highbd_d207e_predictor_8x8_c

void aom_highbd_d45e_predictor_16x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d45e_predictor_16x16 aom_highbd_d45e_predictor_16x16_c

void aom_highbd_d45e_predictor_16x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d45e_predictor_16x32 aom_highbd_d45e_predictor_16x32_c

void aom_highbd_d45e_predictor_16x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d45e_predictor_16x8 aom_highbd_d45e_predictor_16x8_c

void aom_highbd_d45e_predictor_2x2_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d45e_predictor_2x2 aom_highbd_d45e_predictor_2x2_c

void aom_highbd_d45e_predictor_32x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d45e_predictor_32x16 aom_highbd_d45e_predictor_32x16_c

void aom_highbd_d45e_predictor_32x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d45e_predictor_32x32 aom_highbd_d45e_predictor_32x32_c

void aom_highbd_d45e_predictor_4x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d45e_predictor_4x4 aom_highbd_d45e_predictor_4x4_c

void aom_highbd_d45e_predictor_4x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d45e_predictor_4x8 aom_highbd_d45e_predictor_4x8_c

void aom_highbd_d45e_predictor_8x16_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d45e_predictor_8x16 aom_highbd_d45e_predictor_8x16_c

void aom_highbd_d45e_predictor_8x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d45e_predictor_8x4 aom_highbd_d45e_predictor_8x4_c

void aom_highbd_d45e_predictor_8x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d45e_predictor_8x8 aom_highbd_d45e_predictor_8x8_c

void aom_highbd_d63e_predictor_16x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d63e_predictor_16x16 aom_highbd_d63e_predictor_16x16_c

void aom_highbd_d63e_predictor_16x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d63e_predictor_16x32 aom_highbd_d63e_predictor_16x32_c

void aom_highbd_d63e_predictor_16x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d63e_predictor_16x8 aom_highbd_d63e_predictor_16x8_c

void aom_highbd_d63e_predictor_2x2_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d63e_predictor_2x2 aom_highbd_d63e_predictor_2x2_c

void aom_highbd_d63e_predictor_32x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d63e_predictor_32x16 aom_highbd_d63e_predictor_32x16_c

void aom_highbd_d63e_predictor_32x32_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_d63e_predictor_32x32 aom_highbd_d63e_predictor_32x32_c

void aom_highbd_d63e_predictor_4x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d63e_predictor_4x4 aom_highbd_d63e_predictor_4x4_c

void aom_highbd_d63e_predictor_4x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d63e_predictor_4x8 aom_highbd_d63e_predictor_4x8_c

void aom_highbd_d63e_predictor_8x16_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_d63e_predictor_8x16 aom_highbd_d63e_predictor_8x16_c

void aom_highbd_d63e_predictor_8x4_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d63e_predictor_8x4 aom_highbd_d63e_predictor_8x4_c

void aom_highbd_d63e_predictor_8x8_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_d63e_predictor_8x8 aom_highbd_d63e_predictor_8x8_c

void aom_highbd_dc_128_predictor_16x16_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_128_predictor_16x16 aom_highbd_dc_128_predictor_16x16_c

void aom_highbd_dc_128_predictor_16x32_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_128_predictor_16x32 aom_highbd_dc_128_predictor_16x32_c

void aom_highbd_dc_128_predictor_16x8_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_128_predictor_16x8 aom_highbd_dc_128_predictor_16x8_c

void aom_highbd_dc_128_predictor_2x2_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_128_predictor_2x2 aom_highbd_dc_128_predictor_2x2_c

void aom_highbd_dc_128_predictor_32x16_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_128_predictor_32x16 aom_highbd_dc_128_predictor_32x16_c

void aom_highbd_dc_128_predictor_32x32_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_128_predictor_32x32 aom_highbd_dc_128_predictor_32x32_c

void aom_highbd_dc_128_predictor_4x4_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_128_predictor_4x4 aom_highbd_dc_128_predictor_4x4_c

void aom_highbd_dc_128_predictor_4x8_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_128_predictor_4x8 aom_highbd_dc_128_predictor_4x8_c

void aom_highbd_dc_128_predictor_8x16_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_128_predictor_8x16 aom_highbd_dc_128_predictor_8x16_c

void aom_highbd_dc_128_predictor_8x4_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_128_predictor_8x4 aom_highbd_dc_128_predictor_8x4_c

void aom_highbd_dc_128_predictor_8x8_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_128_predictor_8x8 aom_highbd_dc_128_predictor_8x8_c

void aom_highbd_dc_left_predictor_16x16_c(uint16_t* dst,
                                          ptrdiff_t y_stride,
                                          const uint16_t* above,
                                          const uint16_t* left,
                                          int bd);
#define aom_highbd_dc_left_predictor_16x16 aom_highbd_dc_left_predictor_16x16_c

void aom_highbd_dc_left_predictor_16x32_c(uint16_t* dst,
                                          ptrdiff_t y_stride,
                                          const uint16_t* above,
                                          const uint16_t* left,
                                          int bd);
#define aom_highbd_dc_left_predictor_16x32 aom_highbd_dc_left_predictor_16x32_c

void aom_highbd_dc_left_predictor_16x8_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_left_predictor_16x8 aom_highbd_dc_left_predictor_16x8_c

void aom_highbd_dc_left_predictor_2x2_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_left_predictor_2x2 aom_highbd_dc_left_predictor_2x2_c

void aom_highbd_dc_left_predictor_32x16_c(uint16_t* dst,
                                          ptrdiff_t y_stride,
                                          const uint16_t* above,
                                          const uint16_t* left,
                                          int bd);
#define aom_highbd_dc_left_predictor_32x16 aom_highbd_dc_left_predictor_32x16_c

void aom_highbd_dc_left_predictor_32x32_c(uint16_t* dst,
                                          ptrdiff_t y_stride,
                                          const uint16_t* above,
                                          const uint16_t* left,
                                          int bd);
#define aom_highbd_dc_left_predictor_32x32 aom_highbd_dc_left_predictor_32x32_c

void aom_highbd_dc_left_predictor_4x4_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_left_predictor_4x4 aom_highbd_dc_left_predictor_4x4_c

void aom_highbd_dc_left_predictor_4x8_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_left_predictor_4x8 aom_highbd_dc_left_predictor_4x8_c

void aom_highbd_dc_left_predictor_8x16_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_left_predictor_8x16 aom_highbd_dc_left_predictor_8x16_c

void aom_highbd_dc_left_predictor_8x4_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_left_predictor_8x4 aom_highbd_dc_left_predictor_8x4_c

void aom_highbd_dc_left_predictor_8x8_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_left_predictor_8x8 aom_highbd_dc_left_predictor_8x8_c

void aom_highbd_dc_predictor_16x16_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_dc_predictor_16x16 aom_highbd_dc_predictor_16x16_c

void aom_highbd_dc_predictor_16x32_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_dc_predictor_16x32 aom_highbd_dc_predictor_16x32_c

void aom_highbd_dc_predictor_16x8_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_dc_predictor_16x8 aom_highbd_dc_predictor_16x8_c

void aom_highbd_dc_predictor_2x2_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_dc_predictor_2x2 aom_highbd_dc_predictor_2x2_c

void aom_highbd_dc_predictor_32x16_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_dc_predictor_32x16 aom_highbd_dc_predictor_32x16_c

void aom_highbd_dc_predictor_32x32_c(uint16_t* dst,
                                     ptrdiff_t y_stride,
                                     const uint16_t* above,
                                     const uint16_t* left,
                                     int bd);
#define aom_highbd_dc_predictor_32x32 aom_highbd_dc_predictor_32x32_c

void aom_highbd_dc_predictor_4x4_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_dc_predictor_4x4 aom_highbd_dc_predictor_4x4_c

void aom_highbd_dc_predictor_4x8_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_dc_predictor_4x8 aom_highbd_dc_predictor_4x8_c

void aom_highbd_dc_predictor_8x16_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_dc_predictor_8x16 aom_highbd_dc_predictor_8x16_c

void aom_highbd_dc_predictor_8x4_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_dc_predictor_8x4 aom_highbd_dc_predictor_8x4_c

void aom_highbd_dc_predictor_8x8_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_dc_predictor_8x8 aom_highbd_dc_predictor_8x8_c

void aom_highbd_dc_top_predictor_16x16_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_top_predictor_16x16 aom_highbd_dc_top_predictor_16x16_c

void aom_highbd_dc_top_predictor_16x32_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_top_predictor_16x32 aom_highbd_dc_top_predictor_16x32_c

void aom_highbd_dc_top_predictor_16x8_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_top_predictor_16x8 aom_highbd_dc_top_predictor_16x8_c

void aom_highbd_dc_top_predictor_2x2_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_top_predictor_2x2 aom_highbd_dc_top_predictor_2x2_c

void aom_highbd_dc_top_predictor_32x16_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_top_predictor_32x16 aom_highbd_dc_top_predictor_32x16_c

void aom_highbd_dc_top_predictor_32x32_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_dc_top_predictor_32x32 aom_highbd_dc_top_predictor_32x32_c

void aom_highbd_dc_top_predictor_4x4_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_top_predictor_4x4 aom_highbd_dc_top_predictor_4x4_c

void aom_highbd_dc_top_predictor_4x8_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_top_predictor_4x8 aom_highbd_dc_top_predictor_4x8_c

void aom_highbd_dc_top_predictor_8x16_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_dc_top_predictor_8x16 aom_highbd_dc_top_predictor_8x16_c

void aom_highbd_dc_top_predictor_8x4_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_top_predictor_8x4 aom_highbd_dc_top_predictor_8x4_c

void aom_highbd_dc_top_predictor_8x8_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_dc_top_predictor_8x8 aom_highbd_dc_top_predictor_8x8_c

void aom_highbd_h_predictor_16x16_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_h_predictor_16x16 aom_highbd_h_predictor_16x16_c

void aom_highbd_h_predictor_16x32_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_h_predictor_16x32 aom_highbd_h_predictor_16x32_c

void aom_highbd_h_predictor_16x8_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_h_predictor_16x8 aom_highbd_h_predictor_16x8_c

void aom_highbd_h_predictor_2x2_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_h_predictor_2x2 aom_highbd_h_predictor_2x2_c

void aom_highbd_h_predictor_32x16_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_h_predictor_32x16 aom_highbd_h_predictor_32x16_c

void aom_highbd_h_predictor_32x32_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_h_predictor_32x32 aom_highbd_h_predictor_32x32_c

void aom_highbd_h_predictor_4x4_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_h_predictor_4x4 aom_highbd_h_predictor_4x4_c

void aom_highbd_h_predictor_4x8_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_h_predictor_4x8 aom_highbd_h_predictor_4x8_c

void aom_highbd_h_predictor_8x16_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_h_predictor_8x16 aom_highbd_h_predictor_8x16_c

void aom_highbd_h_predictor_8x4_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_h_predictor_8x4 aom_highbd_h_predictor_8x4_c

void aom_highbd_h_predictor_8x8_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_h_predictor_8x8 aom_highbd_h_predictor_8x8_c

void aom_highbd_iwht4x4_16_add_c(const tran_low_t* input,
                                 uint8_t* dest,
                                 int dest_stride,
                                 int bd);
#define aom_highbd_iwht4x4_16_add aom_highbd_iwht4x4_16_add_c

void aom_highbd_iwht4x4_1_add_c(const tran_low_t* input,
                                uint8_t* dest,
                                int dest_stride,
                                int bd);
#define aom_highbd_iwht4x4_1_add aom_highbd_iwht4x4_1_add_c

void aom_highbd_lpf_horizontal_4_c(uint16_t* s,
                                   int pitch,
                                   const uint8_t* blimit,
                                   const uint8_t* limit,
                                   const uint8_t* thresh,
                                   int bd);
#define aom_highbd_lpf_horizontal_4 aom_highbd_lpf_horizontal_4_c

void aom_highbd_lpf_horizontal_4_dual_c(uint16_t* s,
                                        int pitch,
                                        const uint8_t* blimit0,
                                        const uint8_t* limit0,
                                        const uint8_t* thresh0,
                                        const uint8_t* blimit1,
                                        const uint8_t* limit1,
                                        const uint8_t* thresh1,
                                        int bd);
#define aom_highbd_lpf_horizontal_4_dual aom_highbd_lpf_horizontal_4_dual_c

void aom_highbd_lpf_horizontal_8_c(uint16_t* s,
                                   int pitch,
                                   const uint8_t* blimit,
                                   const uint8_t* limit,
                                   const uint8_t* thresh,
                                   int bd);
#define aom_highbd_lpf_horizontal_8 aom_highbd_lpf_horizontal_8_c

void aom_highbd_lpf_horizontal_8_dual_c(uint16_t* s,
                                        int pitch,
                                        const uint8_t* blimit0,
                                        const uint8_t* limit0,
                                        const uint8_t* thresh0,
                                        const uint8_t* blimit1,
                                        const uint8_t* limit1,
                                        const uint8_t* thresh1,
                                        int bd);
#define aom_highbd_lpf_horizontal_8_dual aom_highbd_lpf_horizontal_8_dual_c

void aom_highbd_lpf_horizontal_edge_16_c(uint16_t* s,
                                         int pitch,
                                         const uint8_t* blimit,
                                         const uint8_t* limit,
                                         const uint8_t* thresh,
                                         int bd);
#define aom_highbd_lpf_horizontal_edge_16 aom_highbd_lpf_horizontal_edge_16_c

void aom_highbd_lpf_horizontal_edge_8_c(uint16_t* s,
                                        int pitch,
                                        const uint8_t* blimit,
                                        const uint8_t* limit,
                                        const uint8_t* thresh,
                                        int bd);
#define aom_highbd_lpf_horizontal_edge_8 aom_highbd_lpf_horizontal_edge_8_c

void aom_highbd_lpf_vertical_16_c(uint16_t* s,
                                  int pitch,
                                  const uint8_t* blimit,
                                  const uint8_t* limit,
                                  const uint8_t* thresh,
                                  int bd);
#define aom_highbd_lpf_vertical_16 aom_highbd_lpf_vertical_16_c

void aom_highbd_lpf_vertical_16_dual_c(uint16_t* s,
                                       int pitch,
                                       const uint8_t* blimit,
                                       const uint8_t* limit,
                                       const uint8_t* thresh,
                                       int bd);
#define aom_highbd_lpf_vertical_16_dual aom_highbd_lpf_vertical_16_dual_c

void aom_highbd_lpf_vertical_4_c(uint16_t* s,
                                 int pitch,
                                 const uint8_t* blimit,
                                 const uint8_t* limit,
                                 const uint8_t* thresh,
                                 int bd);
#define aom_highbd_lpf_vertical_4 aom_highbd_lpf_vertical_4_c

void aom_highbd_lpf_vertical_4_dual_c(uint16_t* s,
                                      int pitch,
                                      const uint8_t* blimit0,
                                      const uint8_t* limit0,
                                      const uint8_t* thresh0,
                                      const uint8_t* blimit1,
                                      const uint8_t* limit1,
                                      const uint8_t* thresh1,
                                      int bd);
#define aom_highbd_lpf_vertical_4_dual aom_highbd_lpf_vertical_4_dual_c

void aom_highbd_lpf_vertical_8_c(uint16_t* s,
                                 int pitch,
                                 const uint8_t* blimit,
                                 const uint8_t* limit,
                                 const uint8_t* thresh,
                                 int bd);
#define aom_highbd_lpf_vertical_8 aom_highbd_lpf_vertical_8_c

void aom_highbd_lpf_vertical_8_dual_c(uint16_t* s,
                                      int pitch,
                                      const uint8_t* blimit0,
                                      const uint8_t* limit0,
                                      const uint8_t* thresh0,
                                      const uint8_t* blimit1,
                                      const uint8_t* limit1,
                                      const uint8_t* thresh1,
                                      int bd);
#define aom_highbd_lpf_vertical_8_dual aom_highbd_lpf_vertical_8_dual_c

void aom_highbd_paeth_predictor_16x16_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_paeth_predictor_16x16 aom_highbd_paeth_predictor_16x16_c

void aom_highbd_paeth_predictor_16x32_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_paeth_predictor_16x32 aom_highbd_paeth_predictor_16x32_c

void aom_highbd_paeth_predictor_16x8_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_paeth_predictor_16x8 aom_highbd_paeth_predictor_16x8_c

void aom_highbd_paeth_predictor_2x2_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_paeth_predictor_2x2 aom_highbd_paeth_predictor_2x2_c

void aom_highbd_paeth_predictor_32x16_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_paeth_predictor_32x16 aom_highbd_paeth_predictor_32x16_c

void aom_highbd_paeth_predictor_32x32_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_paeth_predictor_32x32 aom_highbd_paeth_predictor_32x32_c

void aom_highbd_paeth_predictor_4x4_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_paeth_predictor_4x4 aom_highbd_paeth_predictor_4x4_c

void aom_highbd_paeth_predictor_4x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_paeth_predictor_4x8 aom_highbd_paeth_predictor_4x8_c

void aom_highbd_paeth_predictor_8x16_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_paeth_predictor_8x16 aom_highbd_paeth_predictor_8x16_c

void aom_highbd_paeth_predictor_8x4_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_paeth_predictor_8x4 aom_highbd_paeth_predictor_8x4_c

void aom_highbd_paeth_predictor_8x8_c(uint16_t* dst,
                                      ptrdiff_t y_stride,
                                      const uint16_t* above,
                                      const uint16_t* left,
                                      int bd);
#define aom_highbd_paeth_predictor_8x8 aom_highbd_paeth_predictor_8x8_c

void aom_highbd_smooth_h_predictor_16x16_c(uint16_t* dst,
                                           ptrdiff_t y_stride,
                                           const uint16_t* above,
                                           const uint16_t* left,
                                           int bd);
#define aom_highbd_smooth_h_predictor_16x16 \
  aom_highbd_smooth_h_predictor_16x16_c

void aom_highbd_smooth_h_predictor_16x32_c(uint16_t* dst,
                                           ptrdiff_t y_stride,
                                           const uint16_t* above,
                                           const uint16_t* left,
                                           int bd);
#define aom_highbd_smooth_h_predictor_16x32 \
  aom_highbd_smooth_h_predictor_16x32_c

void aom_highbd_smooth_h_predictor_16x8_c(uint16_t* dst,
                                          ptrdiff_t y_stride,
                                          const uint16_t* above,
                                          const uint16_t* left,
                                          int bd);
#define aom_highbd_smooth_h_predictor_16x8 aom_highbd_smooth_h_predictor_16x8_c

void aom_highbd_smooth_h_predictor_2x2_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_h_predictor_2x2 aom_highbd_smooth_h_predictor_2x2_c

void aom_highbd_smooth_h_predictor_32x16_c(uint16_t* dst,
                                           ptrdiff_t y_stride,
                                           const uint16_t* above,
                                           const uint16_t* left,
                                           int bd);
#define aom_highbd_smooth_h_predictor_32x16 \
  aom_highbd_smooth_h_predictor_32x16_c

void aom_highbd_smooth_h_predictor_32x32_c(uint16_t* dst,
                                           ptrdiff_t y_stride,
                                           const uint16_t* above,
                                           const uint16_t* left,
                                           int bd);
#define aom_highbd_smooth_h_predictor_32x32 \
  aom_highbd_smooth_h_predictor_32x32_c

void aom_highbd_smooth_h_predictor_4x4_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_h_predictor_4x4 aom_highbd_smooth_h_predictor_4x4_c

void aom_highbd_smooth_h_predictor_4x8_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_h_predictor_4x8 aom_highbd_smooth_h_predictor_4x8_c

void aom_highbd_smooth_h_predictor_8x16_c(uint16_t* dst,
                                          ptrdiff_t y_stride,
                                          const uint16_t* above,
                                          const uint16_t* left,
                                          int bd);
#define aom_highbd_smooth_h_predictor_8x16 aom_highbd_smooth_h_predictor_8x16_c

void aom_highbd_smooth_h_predictor_8x4_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_h_predictor_8x4 aom_highbd_smooth_h_predictor_8x4_c

void aom_highbd_smooth_h_predictor_8x8_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_h_predictor_8x8 aom_highbd_smooth_h_predictor_8x8_c

void aom_highbd_smooth_predictor_16x16_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_predictor_16x16 aom_highbd_smooth_predictor_16x16_c

void aom_highbd_smooth_predictor_16x32_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_predictor_16x32 aom_highbd_smooth_predictor_16x32_c

void aom_highbd_smooth_predictor_16x8_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_smooth_predictor_16x8 aom_highbd_smooth_predictor_16x8_c

void aom_highbd_smooth_predictor_2x2_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_smooth_predictor_2x2 aom_highbd_smooth_predictor_2x2_c

void aom_highbd_smooth_predictor_32x16_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_predictor_32x16 aom_highbd_smooth_predictor_32x16_c

void aom_highbd_smooth_predictor_32x32_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_predictor_32x32 aom_highbd_smooth_predictor_32x32_c

void aom_highbd_smooth_predictor_4x4_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_smooth_predictor_4x4 aom_highbd_smooth_predictor_4x4_c

void aom_highbd_smooth_predictor_4x8_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_smooth_predictor_4x8 aom_highbd_smooth_predictor_4x8_c

void aom_highbd_smooth_predictor_8x16_c(uint16_t* dst,
                                        ptrdiff_t y_stride,
                                        const uint16_t* above,
                                        const uint16_t* left,
                                        int bd);
#define aom_highbd_smooth_predictor_8x16 aom_highbd_smooth_predictor_8x16_c

void aom_highbd_smooth_predictor_8x4_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_smooth_predictor_8x4 aom_highbd_smooth_predictor_8x4_c

void aom_highbd_smooth_predictor_8x8_c(uint16_t* dst,
                                       ptrdiff_t y_stride,
                                       const uint16_t* above,
                                       const uint16_t* left,
                                       int bd);
#define aom_highbd_smooth_predictor_8x8 aom_highbd_smooth_predictor_8x8_c

void aom_highbd_smooth_v_predictor_16x16_c(uint16_t* dst,
                                           ptrdiff_t y_stride,
                                           const uint16_t* above,
                                           const uint16_t* left,
                                           int bd);
#define aom_highbd_smooth_v_predictor_16x16 \
  aom_highbd_smooth_v_predictor_16x16_c

void aom_highbd_smooth_v_predictor_16x32_c(uint16_t* dst,
                                           ptrdiff_t y_stride,
                                           const uint16_t* above,
                                           const uint16_t* left,
                                           int bd);
#define aom_highbd_smooth_v_predictor_16x32 \
  aom_highbd_smooth_v_predictor_16x32_c

void aom_highbd_smooth_v_predictor_16x8_c(uint16_t* dst,
                                          ptrdiff_t y_stride,
                                          const uint16_t* above,
                                          const uint16_t* left,
                                          int bd);
#define aom_highbd_smooth_v_predictor_16x8 aom_highbd_smooth_v_predictor_16x8_c

void aom_highbd_smooth_v_predictor_2x2_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_v_predictor_2x2 aom_highbd_smooth_v_predictor_2x2_c

void aom_highbd_smooth_v_predictor_32x16_c(uint16_t* dst,
                                           ptrdiff_t y_stride,
                                           const uint16_t* above,
                                           const uint16_t* left,
                                           int bd);
#define aom_highbd_smooth_v_predictor_32x16 \
  aom_highbd_smooth_v_predictor_32x16_c

void aom_highbd_smooth_v_predictor_32x32_c(uint16_t* dst,
                                           ptrdiff_t y_stride,
                                           const uint16_t* above,
                                           const uint16_t* left,
                                           int bd);
#define aom_highbd_smooth_v_predictor_32x32 \
  aom_highbd_smooth_v_predictor_32x32_c

void aom_highbd_smooth_v_predictor_4x4_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_v_predictor_4x4 aom_highbd_smooth_v_predictor_4x4_c

void aom_highbd_smooth_v_predictor_4x8_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_v_predictor_4x8 aom_highbd_smooth_v_predictor_4x8_c

void aom_highbd_smooth_v_predictor_8x16_c(uint16_t* dst,
                                          ptrdiff_t y_stride,
                                          const uint16_t* above,
                                          const uint16_t* left,
                                          int bd);
#define aom_highbd_smooth_v_predictor_8x16 aom_highbd_smooth_v_predictor_8x16_c

void aom_highbd_smooth_v_predictor_8x4_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_v_predictor_8x4 aom_highbd_smooth_v_predictor_8x4_c

void aom_highbd_smooth_v_predictor_8x8_c(uint16_t* dst,
                                         ptrdiff_t y_stride,
                                         const uint16_t* above,
                                         const uint16_t* left,
                                         int bd);
#define aom_highbd_smooth_v_predictor_8x8 aom_highbd_smooth_v_predictor_8x8_c

void aom_highbd_v_predictor_16x16_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_v_predictor_16x16 aom_highbd_v_predictor_16x16_c

void aom_highbd_v_predictor_16x32_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_v_predictor_16x32 aom_highbd_v_predictor_16x32_c

void aom_highbd_v_predictor_16x8_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_v_predictor_16x8 aom_highbd_v_predictor_16x8_c

void aom_highbd_v_predictor_2x2_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_v_predictor_2x2 aom_highbd_v_predictor_2x2_c

void aom_highbd_v_predictor_32x16_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_v_predictor_32x16 aom_highbd_v_predictor_32x16_c

void aom_highbd_v_predictor_32x32_c(uint16_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint16_t* above,
                                    const uint16_t* left,
                                    int bd);
#define aom_highbd_v_predictor_32x32 aom_highbd_v_predictor_32x32_c

void aom_highbd_v_predictor_4x4_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_v_predictor_4x4 aom_highbd_v_predictor_4x4_c

void aom_highbd_v_predictor_4x8_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_v_predictor_4x8 aom_highbd_v_predictor_4x8_c

void aom_highbd_v_predictor_8x16_c(uint16_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint16_t* above,
                                   const uint16_t* left,
                                   int bd);
#define aom_highbd_v_predictor_8x16 aom_highbd_v_predictor_8x16_c

void aom_highbd_v_predictor_8x4_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_v_predictor_8x4 aom_highbd_v_predictor_8x4_c

void aom_highbd_v_predictor_8x8_c(uint16_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint16_t* above,
                                  const uint16_t* left,
                                  int bd);
#define aom_highbd_v_predictor_8x8 aom_highbd_v_predictor_8x8_c

void aom_idct16x16_10_add_c(const tran_low_t* input,
                            uint8_t* dest,
                            int dest_stride);
#define aom_idct16x16_10_add aom_idct16x16_10_add_c

void aom_idct16x16_1_add_c(const tran_low_t* input,
                           uint8_t* dest,
                           int dest_stride);
#define aom_idct16x16_1_add aom_idct16x16_1_add_c

void aom_idct16x16_256_add_c(const tran_low_t* input,
                             uint8_t* dest,
                             int dest_stride);
#define aom_idct16x16_256_add aom_idct16x16_256_add_c

void aom_idct16x16_38_add_c(const tran_low_t* input,
                            uint8_t* dest,
                            int dest_stride);
#define aom_idct16x16_38_add aom_idct16x16_38_add_c

void aom_idct32x32_1024_add_c(const tran_low_t* input,
                              uint8_t* dest,
                              int dest_stride);
#define aom_idct32x32_1024_add aom_idct32x32_1024_add_c

void aom_idct32x32_135_add_c(const tran_low_t* input,
                             uint8_t* dest,
                             int dest_stride);
#define aom_idct32x32_135_add aom_idct32x32_135_add_c

void aom_idct32x32_1_add_c(const tran_low_t* input,
                           uint8_t* dest,
                           int dest_stride);
#define aom_idct32x32_1_add aom_idct32x32_1_add_c

void aom_idct32x32_34_add_c(const tran_low_t* input,
                            uint8_t* dest,
                            int dest_stride);
#define aom_idct32x32_34_add aom_idct32x32_34_add_c

void aom_idct4x4_16_add_c(const tran_low_t* input,
                          uint8_t* dest,
                          int dest_stride);
#define aom_idct4x4_16_add aom_idct4x4_16_add_c

void aom_idct4x4_1_add_c(const tran_low_t* input,
                         uint8_t* dest,
                         int dest_stride);
#define aom_idct4x4_1_add aom_idct4x4_1_add_c

void aom_idct8x8_12_add_c(const tran_low_t* input,
                          uint8_t* dest,
                          int dest_stride);
#define aom_idct8x8_12_add aom_idct8x8_12_add_c

void aom_idct8x8_1_add_c(const tran_low_t* input,
                         uint8_t* dest,
                         int dest_stride);
#define aom_idct8x8_1_add aom_idct8x8_1_add_c

void aom_idct8x8_64_add_c(const tran_low_t* input,
                          uint8_t* dest,
                          int dest_stride);
#define aom_idct8x8_64_add aom_idct8x8_64_add_c

void aom_iwht4x4_16_add_c(const tran_low_t* input,
                          uint8_t* dest,
                          int dest_stride);
#define aom_iwht4x4_16_add aom_iwht4x4_16_add_c

void aom_iwht4x4_1_add_c(const tran_low_t* input,
                         uint8_t* dest,
                         int dest_stride);
#define aom_iwht4x4_1_add aom_iwht4x4_1_add_c

void aom_lpf_horizontal_4_c(uint8_t* s,
                            int pitch,
                            const uint8_t* blimit,
                            const uint8_t* limit,
                            const uint8_t* thresh);
#define aom_lpf_horizontal_4 aom_lpf_horizontal_4_c

void aom_lpf_horizontal_4_dual_c(uint8_t* s,
                                 int pitch,
                                 const uint8_t* blimit0,
                                 const uint8_t* limit0,
                                 const uint8_t* thresh0,
                                 const uint8_t* blimit1,
                                 const uint8_t* limit1,
                                 const uint8_t* thresh1);
#define aom_lpf_horizontal_4_dual aom_lpf_horizontal_4_dual_c

void aom_lpf_horizontal_8_c(uint8_t* s,
                            int pitch,
                            const uint8_t* blimit,
                            const uint8_t* limit,
                            const uint8_t* thresh);
#define aom_lpf_horizontal_8 aom_lpf_horizontal_8_c

void aom_lpf_horizontal_8_dual_c(uint8_t* s,
                                 int pitch,
                                 const uint8_t* blimit0,
                                 const uint8_t* limit0,
                                 const uint8_t* thresh0,
                                 const uint8_t* blimit1,
                                 const uint8_t* limit1,
                                 const uint8_t* thresh1);
#define aom_lpf_horizontal_8_dual aom_lpf_horizontal_8_dual_c

void aom_lpf_horizontal_edge_16_c(uint8_t* s,
                                  int pitch,
                                  const uint8_t* blimit,
                                  const uint8_t* limit,
                                  const uint8_t* thresh);
#define aom_lpf_horizontal_edge_16 aom_lpf_horizontal_edge_16_c

void aom_lpf_horizontal_edge_8_c(uint8_t* s,
                                 int pitch,
                                 const uint8_t* blimit,
                                 const uint8_t* limit,
                                 const uint8_t* thresh);
#define aom_lpf_horizontal_edge_8 aom_lpf_horizontal_edge_8_c

void aom_lpf_vertical_16_c(uint8_t* s,
                           int pitch,
                           const uint8_t* blimit,
                           const uint8_t* limit,
                           const uint8_t* thresh);
#define aom_lpf_vertical_16 aom_lpf_vertical_16_c

void aom_lpf_vertical_16_dual_c(uint8_t* s,
                                int pitch,
                                const uint8_t* blimit,
                                const uint8_t* limit,
                                const uint8_t* thresh);
#define aom_lpf_vertical_16_dual aom_lpf_vertical_16_dual_c

void aom_lpf_vertical_4_c(uint8_t* s,
                          int pitch,
                          const uint8_t* blimit,
                          const uint8_t* limit,
                          const uint8_t* thresh);
#define aom_lpf_vertical_4 aom_lpf_vertical_4_c

void aom_lpf_vertical_4_dual_c(uint8_t* s,
                               int pitch,
                               const uint8_t* blimit0,
                               const uint8_t* limit0,
                               const uint8_t* thresh0,
                               const uint8_t* blimit1,
                               const uint8_t* limit1,
                               const uint8_t* thresh1);
#define aom_lpf_vertical_4_dual aom_lpf_vertical_4_dual_c

void aom_lpf_vertical_8_c(uint8_t* s,
                          int pitch,
                          const uint8_t* blimit,
                          const uint8_t* limit,
                          const uint8_t* thresh);
#define aom_lpf_vertical_8 aom_lpf_vertical_8_c

void aom_lpf_vertical_8_dual_c(uint8_t* s,
                               int pitch,
                               const uint8_t* blimit0,
                               const uint8_t* limit0,
                               const uint8_t* thresh0,
                               const uint8_t* blimit1,
                               const uint8_t* limit1,
                               const uint8_t* thresh1);
#define aom_lpf_vertical_8_dual aom_lpf_vertical_8_dual_c

void aom_paeth_predictor_16x16_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_paeth_predictor_16x16 aom_paeth_predictor_16x16_c

void aom_paeth_predictor_16x32_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_paeth_predictor_16x32 aom_paeth_predictor_16x32_c

void aom_paeth_predictor_16x8_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_paeth_predictor_16x8 aom_paeth_predictor_16x8_c

void aom_paeth_predictor_2x2_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_paeth_predictor_2x2 aom_paeth_predictor_2x2_c

void aom_paeth_predictor_32x16_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_paeth_predictor_32x16 aom_paeth_predictor_32x16_c

void aom_paeth_predictor_32x32_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_paeth_predictor_32x32 aom_paeth_predictor_32x32_c

void aom_paeth_predictor_4x4_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_paeth_predictor_4x4 aom_paeth_predictor_4x4_c

void aom_paeth_predictor_4x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_paeth_predictor_4x8 aom_paeth_predictor_4x8_c

void aom_paeth_predictor_8x16_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_paeth_predictor_8x16 aom_paeth_predictor_8x16_c

void aom_paeth_predictor_8x4_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_paeth_predictor_8x4 aom_paeth_predictor_8x4_c

void aom_paeth_predictor_8x8_c(uint8_t* dst,
                               ptrdiff_t y_stride,
                               const uint8_t* above,
                               const uint8_t* left);
#define aom_paeth_predictor_8x8 aom_paeth_predictor_8x8_c

void aom_scaled_2d_c(const uint8_t* src,
                     ptrdiff_t src_stride,
                     uint8_t* dst,
                     ptrdiff_t dst_stride,
                     const int16_t* filter_x,
                     int x_step_q4,
                     const int16_t* filter_y,
                     int y_step_q4,
                     int w,
                     int h);
#define aom_scaled_2d aom_scaled_2d_c

void aom_scaled_avg_2d_c(const uint8_t* src,
                         ptrdiff_t src_stride,
                         uint8_t* dst,
                         ptrdiff_t dst_stride,
                         const int16_t* filter_x,
                         int x_step_q4,
                         const int16_t* filter_y,
                         int y_step_q4,
                         int w,
                         int h);
#define aom_scaled_avg_2d aom_scaled_avg_2d_c

void aom_scaled_avg_horiz_c(const uint8_t* src,
                            ptrdiff_t src_stride,
                            uint8_t* dst,
                            ptrdiff_t dst_stride,
                            const int16_t* filter_x,
                            int x_step_q4,
                            const int16_t* filter_y,
                            int y_step_q4,
                            int w,
                            int h);
#define aom_scaled_avg_horiz aom_scaled_avg_horiz_c

void aom_scaled_avg_vert_c(const uint8_t* src,
                           ptrdiff_t src_stride,
                           uint8_t* dst,
                           ptrdiff_t dst_stride,
                           const int16_t* filter_x,
                           int x_step_q4,
                           const int16_t* filter_y,
                           int y_step_q4,
                           int w,
                           int h);
#define aom_scaled_avg_vert aom_scaled_avg_vert_c

void aom_scaled_horiz_c(const uint8_t* src,
                        ptrdiff_t src_stride,
                        uint8_t* dst,
                        ptrdiff_t dst_stride,
                        const int16_t* filter_x,
                        int x_step_q4,
                        const int16_t* filter_y,
                        int y_step_q4,
                        int w,
                        int h);
#define aom_scaled_horiz aom_scaled_horiz_c

void aom_scaled_vert_c(const uint8_t* src,
                       ptrdiff_t src_stride,
                       uint8_t* dst,
                       ptrdiff_t dst_stride,
                       const int16_t* filter_x,
                       int x_step_q4,
                       const int16_t* filter_y,
                       int y_step_q4,
                       int w,
                       int h);
#define aom_scaled_vert aom_scaled_vert_c

void aom_smooth_h_predictor_16x16_c(uint8_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint8_t* above,
                                    const uint8_t* left);
#define aom_smooth_h_predictor_16x16 aom_smooth_h_predictor_16x16_c

void aom_smooth_h_predictor_16x32_c(uint8_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint8_t* above,
                                    const uint8_t* left);
#define aom_smooth_h_predictor_16x32 aom_smooth_h_predictor_16x32_c

void aom_smooth_h_predictor_16x8_c(uint8_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint8_t* above,
                                   const uint8_t* left);
#define aom_smooth_h_predictor_16x8 aom_smooth_h_predictor_16x8_c

void aom_smooth_h_predictor_2x2_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_h_predictor_2x2 aom_smooth_h_predictor_2x2_c

void aom_smooth_h_predictor_32x16_c(uint8_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint8_t* above,
                                    const uint8_t* left);
#define aom_smooth_h_predictor_32x16 aom_smooth_h_predictor_32x16_c

void aom_smooth_h_predictor_32x32_c(uint8_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint8_t* above,
                                    const uint8_t* left);
#define aom_smooth_h_predictor_32x32 aom_smooth_h_predictor_32x32_c

void aom_smooth_h_predictor_4x4_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_h_predictor_4x4 aom_smooth_h_predictor_4x4_c

void aom_smooth_h_predictor_4x8_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_h_predictor_4x8 aom_smooth_h_predictor_4x8_c

void aom_smooth_h_predictor_8x16_c(uint8_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint8_t* above,
                                   const uint8_t* left);
#define aom_smooth_h_predictor_8x16 aom_smooth_h_predictor_8x16_c

void aom_smooth_h_predictor_8x4_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_h_predictor_8x4 aom_smooth_h_predictor_8x4_c

void aom_smooth_h_predictor_8x8_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_h_predictor_8x8 aom_smooth_h_predictor_8x8_c

void aom_smooth_predictor_16x16_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_predictor_16x16 aom_smooth_predictor_16x16_c

void aom_smooth_predictor_16x32_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_predictor_16x32 aom_smooth_predictor_16x32_c

void aom_smooth_predictor_16x8_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_smooth_predictor_16x8 aom_smooth_predictor_16x8_c

void aom_smooth_predictor_2x2_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_smooth_predictor_2x2 aom_smooth_predictor_2x2_c

void aom_smooth_predictor_32x16_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_predictor_32x16 aom_smooth_predictor_32x16_c

void aom_smooth_predictor_32x32_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_predictor_32x32 aom_smooth_predictor_32x32_c

void aom_smooth_predictor_4x4_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_smooth_predictor_4x4 aom_smooth_predictor_4x4_c

void aom_smooth_predictor_4x8_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_smooth_predictor_4x8 aom_smooth_predictor_4x8_c

void aom_smooth_predictor_8x16_c(uint8_t* dst,
                                 ptrdiff_t y_stride,
                                 const uint8_t* above,
                                 const uint8_t* left);
#define aom_smooth_predictor_8x16 aom_smooth_predictor_8x16_c

void aom_smooth_predictor_8x4_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_smooth_predictor_8x4 aom_smooth_predictor_8x4_c

void aom_smooth_predictor_8x8_c(uint8_t* dst,
                                ptrdiff_t y_stride,
                                const uint8_t* above,
                                const uint8_t* left);
#define aom_smooth_predictor_8x8 aom_smooth_predictor_8x8_c

void aom_smooth_v_predictor_16x16_c(uint8_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint8_t* above,
                                    const uint8_t* left);
#define aom_smooth_v_predictor_16x16 aom_smooth_v_predictor_16x16_c

void aom_smooth_v_predictor_16x32_c(uint8_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint8_t* above,
                                    const uint8_t* left);
#define aom_smooth_v_predictor_16x32 aom_smooth_v_predictor_16x32_c

void aom_smooth_v_predictor_16x8_c(uint8_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint8_t* above,
                                   const uint8_t* left);
#define aom_smooth_v_predictor_16x8 aom_smooth_v_predictor_16x8_c

void aom_smooth_v_predictor_2x2_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_v_predictor_2x2 aom_smooth_v_predictor_2x2_c

void aom_smooth_v_predictor_32x16_c(uint8_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint8_t* above,
                                    const uint8_t* left);
#define aom_smooth_v_predictor_32x16 aom_smooth_v_predictor_32x16_c

void aom_smooth_v_predictor_32x32_c(uint8_t* dst,
                                    ptrdiff_t y_stride,
                                    const uint8_t* above,
                                    const uint8_t* left);
#define aom_smooth_v_predictor_32x32 aom_smooth_v_predictor_32x32_c

void aom_smooth_v_predictor_4x4_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_v_predictor_4x4 aom_smooth_v_predictor_4x4_c

void aom_smooth_v_predictor_4x8_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_v_predictor_4x8 aom_smooth_v_predictor_4x8_c

void aom_smooth_v_predictor_8x16_c(uint8_t* dst,
                                   ptrdiff_t y_stride,
                                   const uint8_t* above,
                                   const uint8_t* left);
#define aom_smooth_v_predictor_8x16 aom_smooth_v_predictor_8x16_c

void aom_smooth_v_predictor_8x4_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_v_predictor_8x4 aom_smooth_v_predictor_8x4_c

void aom_smooth_v_predictor_8x8_c(uint8_t* dst,
                                  ptrdiff_t y_stride,
                                  const uint8_t* above,
                                  const uint8_t* left);
#define aom_smooth_v_predictor_8x8 aom_smooth_v_predictor_8x8_c

void aom_v_predictor_16x16_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_v_predictor_16x16 aom_v_predictor_16x16_c

void aom_v_predictor_16x32_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_v_predictor_16x32 aom_v_predictor_16x32_c

void aom_v_predictor_16x8_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_v_predictor_16x8 aom_v_predictor_16x8_c

void aom_v_predictor_2x2_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_v_predictor_2x2 aom_v_predictor_2x2_c

void aom_v_predictor_32x16_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_v_predictor_32x16 aom_v_predictor_32x16_c

void aom_v_predictor_32x32_c(uint8_t* dst,
                             ptrdiff_t y_stride,
                             const uint8_t* above,
                             const uint8_t* left);
#define aom_v_predictor_32x32 aom_v_predictor_32x32_c

void aom_v_predictor_4x4_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_v_predictor_4x4 aom_v_predictor_4x4_c

void aom_v_predictor_4x8_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_v_predictor_4x8 aom_v_predictor_4x8_c

void aom_v_predictor_8x16_c(uint8_t* dst,
                            ptrdiff_t y_stride,
                            const uint8_t* above,
                            const uint8_t* left);
#define aom_v_predictor_8x16 aom_v_predictor_8x16_c

void aom_v_predictor_8x4_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_v_predictor_8x4 aom_v_predictor_8x4_c

void aom_v_predictor_8x8_c(uint8_t* dst,
                           ptrdiff_t y_stride,
                           const uint8_t* above,
                           const uint8_t* left);
#define aom_v_predictor_8x8 aom_v_predictor_8x8_c

void aom_dsp_rtcd(void);

#include "aom_config.h"

#ifdef RTCD_C
static void setup_rtcd_internal(void) {}
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
