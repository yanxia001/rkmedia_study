#ifndef _hxy_H_
#define _hxy_H_

void vi_set();
void vi_get_frame();
void vi_cd ();
void venc_set(IMAGE_TYPE_E ,int ,int );
void vi_to_venc();
void venc_reg();
void rga_venc();

void vi_to_rga_to_venc();
void vi_rga_bind_register_cb();
void main_init_rtsp();
void son_init_rtsp();
void venc_fengzhaung_reg();
void venc_main_reg();
void venc_son_reg();
void vi_todengzhuang_venc();

void venc_son_set(IMAGE_TYPE_E ,int ,int );
#endif


