#include "accel_data.h"

uint8_t ap_done;
uint8_t ofm_batch;
uint8_t ifm_batch;
uint8_t fm_div_cnt;

uint32_t sel;
uint32_t reg_conv_len_sel_relu_sel;
uint32_t reg_pool_config;
uint32_t reg_scale_shift;
uint32_t reg_zp_out_in;

uint32_t ifm_ddr_base_addr;
uint32_t ofm_ddr_base_addr;
uint32_t wgt_ddr_base_addr;
uint32_t bia_ddr_base_addr;
uint32_t lky_ddr_base_addr;

uint8_t state;

uint16_t fm_col;
uint16_t fm_row;
uint16_t fm_div;
uint16_t fm_res;
uint32_t fm_size;

uint8_t ofm_recv_half;
uint16_t fm_col_half;
uint16_t fm_row_half;
uint16_t fm_div_half;
uint16_t fm_res_half;
uint32_t fm_size_half;

uint32_t wgt_addr_send;
uint32_t wgt_addr_read;
uint32_t bia_addr_read;

uint32_t ifm_addr_send;
uint32_t ifm_addr_fmbase;
uint32_t ifm_addr_offset;
uint32_t ifm_send_len;

uint32_t ofm_addr_recv;
uint32_t ofm_addr_fmbase;
uint32_t ofm_addr_offset;
uint32_t ofm_recv_len;

uint32_t reg_static;
uint32_t reg_task;
uint8_t last_buf_sel;

uint8_t iter_ifm_pre;
uint8_t iter_ofm_pre;
uint8_t iter_div_pre;
uint8_t iter_ifm_post;
uint8_t iter_ofm_post;
uint8_t iter_div_post;

uint8_t ifm_send_task_enable;
uint8_t ofm_recv_task_enable;
uint8_t wgt_send_task_enable;
/*fm_col_list的6个参数分别对应特征图大小13 26 */
uint16_t fm_col_list[]={
	FM_COL_0,FM_COL_1,FM_COL_2,
	FM_COL_3,FM_COL_4,FM_COL_5
};
uint16_t fm_row_list[]={
	FM_ROW_0,FM_ROW_1,FM_ROW_2,
	FM_ROW_3,FM_ROW_4,FM_ROW_5
};

uint32_t *ifm_list		=&config_list[0*MAX_TASK];
uint32_t *ofm_list		=&config_list[1*MAX_TASK];
uint32_t *scale_list	=&config_list[2*MAX_TASK];
uint32_t *shift_list	=&config_list[3*MAX_TASK];
uint32_t *zp_in_list	=&config_list[4*MAX_TASK];
uint32_t *zp_out_list	=&config_list[5*MAX_TASK];
uint32_t *zp_act_list	=&config_list[6*MAX_TASK];
uint32_t *sel_in_list	=&config_list[7*MAX_TASK];
uint32_t *pool_list		=&config_list[8*MAX_TASK];
uint32_t *stride_list	=&config_list[9*MAX_TASK];

float sigmoid_table[256];
char cate[MAX_CLASS][MAX_CATE_LEN];
uint32_t config_list[MAX_LIST*MAX_TASK];

uint8_t FM_BUF_0[MAX_FM]	__attribute((aligned (64)));
uint8_t FM_BUF_1[MAX_FM]	__attribute((aligned (64)));
uint8_t WEIGHT0[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT1[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT2[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT3[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT4[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT5[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT6[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT7[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT8[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t WEIGHT9[MAX_WEIGHT]	__attribute((aligned (64)));
uint8_t BIAS0[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS1[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS2[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS3[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS4[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS5[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS6[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS7[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS8[MAX_BIAS]		__attribute((aligned (64)));
uint8_t BIAS9[MAX_BIAS]		__attribute((aligned (64)));
uint8_t LEAKY0[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY1[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY2[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY3[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY4[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY5[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY6[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY7[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY8[MAX_LEAKY]	__attribute((aligned (64)));
uint8_t LEAKY9[MAX_LEAKY]	__attribute((aligned (64)));

uint8_t *ifm_ddr_base_addr_in_list[MAX_TASK]={
	ifmInBuf[1],FM_BUF_1,FM_BUF_0,FM_BUF_1,FM_BUF_0,
	//FM_BUF_0,FM_BUF_1,FM_BUF_0,FM_BUF_1,FM_BUF_0,
	FM_BUF_1,FM_BUF_0,FM_BUF_1,FM_BUF_0,FM_BUF_1
};
uint8_t *ofm_ddr_base_addr_in_list[MAX_TASK]={
	FM_BUF_1,FM_BUF_0,FM_BUF_1,FM_BUF_0,FM_BUF_1,
	FM_BUF_0,FM_BUF_1,FM_BUF_0,FM_BUF_1,FM_BUF_0
};
uint8_t *wgt_ddr_base_addr_in_list[MAX_TASK]={
	WEIGHT0,WEIGHT1,WEIGHT2,WEIGHT3,WEIGHT4,
	WEIGHT5,WEIGHT6,WEIGHT7,WEIGHT8,WEIGHT9
};
uint8_t *bia_ddr_base_addr_in_list[MAX_TASK]={
	BIAS0,BIAS1,BIAS2,BIAS3,BIAS4,
	BIAS5,BIAS6,BIAS7,BIAS8,BIAS9
};
uint8_t *lky_ddr_base_addr_in_list[MAX_TASK]={
	LEAKY0,LEAKY1,LEAKY2,LEAKY3,LEAKY4,
	LEAKY5,LEAKY6,LEAKY7,LEAKY8,LEAKY9
};

const float anchor[3][2]={
	{81,82},{135,169},{344,319}
};
const uint8_t color1[MAX_CLASS]={255,0,0};
const uint8_t color2[MAX_CLASS]={0,0,255};
const uint8_t color3[MAX_CLASS]={0,255,0};

uint32_t xleft;
uint32_t xright;
uint32_t ytop;
uint32_t ydown;
float prob;
uint8_t category;
uint8_t CLASS;

uint8_t chr1;
uint8_t chr2;
uint8_t chr3;
uint8_t chr4;
uint8_t chr5;
uint8_t chr6;

FIL fil;
FATFS fatfs;
FRESULT response;
uint32_t wr_tot;//指示写入或读了多少个数据（结果）

XTime tEnd,tCur;
XTime tEnd_t,tCur_t;
uint32_t tUsed;

void Load_IFM(uint8_t *ifmbuf,int fmcol,int fm_row,int fmch){ //输入指针，行，列，通道
	xil_printf("Begin Load_IFM!\n");
	f_open(&fil,"IFMT.bin",FA_OPEN_EXISTING|FA_READ);
	f_read(&fil,ifmbuf,fmcol*fm_row*fmch,&wr_tot);//文件系统指针；内存地址；读取长度；实际读取长度
	f_close(&fil);
	Xil_DCacheFlushRange((INTPTR)ifmbuf,fmcol*fm_row*fmch);
	xil_printf("Finish Load_IFM!\n");
	return;
}
void Save_OFM(uint8_t *ofmbuf,int fmcol,int fm_row,int fmch){
	xil_printf("Begin Save_OFM!\n");
	Xil_DCacheInvalidateRange((INTPTR)ofmbuf,fmcol*fm_row*fmch);
	f_open(&fil,"OFMS.bin",FA_CREATE_ALWAYS|FA_WRITE);
	f_write(&fil,ofmbuf,fmcol*fm_row*fmch,&wr_tot);
	f_close(&fil);
	xil_printf("Finish Save_OFM!\n");
	return;
}
void Load_Para(char c,int class_num){

	xil_printf("-------------- Begin Load_Para! -------------- \n");
	CLASS=class_num;
	char filename[]="1:/000.bin";
	filename[3]=c;



	/* ------------------- config -------------------- */
	filename[4]='C';
	filename[5]='G';
	xil_printf("filename =  %s\n",filename);
	//xil_printf("filename[3] =  %c\n",filename[3]);
//	xil_printf("filename[4] =  %c\n",filename[4]);
	//xil_printf("filename[5] =  %c\n",filename[5]);
	response = f_open(&fil,filename,FA_OPEN_EXISTING|FA_READ);
	if(response !=FR_OK)
	{
		xil_printf("open fail!!!!!!!!!!!!!\n");
		xil_printf("response = %d \n",response);
		return;

	}
	response = f_read(&fil,config_list,4*MAX_LIST*MAX_TASK,&wr_tot);
if(response !=FR_OK)
{
	xil_printf("read fail!!!!!!!!!!!!!\n");
	xil_printf("res = %d \n",response);
	return;
}
	f_close(&fil);
//	xil_printf("config_list:%x \n config_list[0]:%x \n  config_list[1]: %x \n  config_list[2]:%x \n",config_list,config_list[0],config_list[1],config_list[2]);
	//xil_printf("  wr_tot= %x \n  &wr_tot= %x \n",wr_tot,&wr_tot);

	/* ------------------- weights ------------------- */
	filename[4]='W';
	for(int i=0;i<MAX_TASK;i++){
		filename[5]='0'+i;
		f_open(&fil,filename,FA_OPEN_EXISTING|FA_READ);
		f_read(&fil,wgt_ddr_base_addr_in_list[i],ifm_list[i]*ofm_list[i]*9,&wr_tot);
		//xil_printf("wgt_ddr_base_addr_in_list[i]:%x wr_tot=%x\n",wgt_ddr_base_addr_in_list[i],wr_tot);
		//xil_printf("ofm_list[%d]:%x \n",i,ofm_list[i]);
		f_close(&fil);

	}
	/* ------------------- bias ---------------------- */
	filename[4]='B';
	for(int i=0;i<MAX_TASK;i++){
		filename[5]='0'+i;
		f_open(&fil,filename,FA_OPEN_EXISTING|FA_READ);
		f_read(&fil,bia_ddr_base_addr_in_list[i],ofm_list[i]*8,&wr_tot);//文件指针；读取基地址；需要读取的数据量；存储地址
		f_close(&fil);
	}
	/* ------------------- leaky --------------------- */
	filename[4]='R';
	for(int i=0;i<MAX_TASK;i++){
		filename[5]='0'+i;
		f_open(&fil,filename,FA_OPEN_EXISTING|FA_READ);
		f_read(&fil,lky_ddr_base_addr_in_list[i],256*8,&wr_tot);
		f_close(&fil);
	}
	for(int i=0;i<MAX_TASK;i++){
		Xil_DCacheFlushRange((INTPTR)wgt_ddr_base_addr_in_list[i],ifm_list[i]*ofm_list[i]*9);
		Xil_DCacheFlushRange((INTPTR)bia_ddr_base_addr_in_list[i],ofm_list[i]*8);
		Xil_DCacheFlushRange((INTPTR)lky_ddr_base_addr_in_list[i],256*8);
	}
	/* ------------------- sigmoid ------------------- */
	filename[4]='S';
	filename[5]='G';
	f_open(&fil,filename,FA_OPEN_EXISTING|FA_READ);
	f_read(&fil,sigmoid_table,4*256,&wr_tot);
	f_close(&fil);
	/* ------------------- class name ---------------- */
	filename[4]='C';
	filename[5]='S';
	//filename[5]='M';
	f_open(&fil,filename,FA_OPEN_EXISTING|FA_READ);
	f_read(&fil,cate,MAX_CLASS*MAX_CATE_LEN,&wr_tot);
	f_close(&fil);
	xil_printf("cate:%s \n",cate);
	xil_printf("-------------- Finish Load_Para! ------------- \n");
	return;
}
