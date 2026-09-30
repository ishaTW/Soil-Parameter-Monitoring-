//void AsciiTohex(u8 *ptr, int len);
void tw_fileread(void);
void AsciiTohex(void);
void cat_all(void);
void new_file(char *);
//________itoa___
char ix_Retchar(unsigned int a);
char *ix_Itoa(signed long num);
void spped_capt(void);
//void take_pic_data(void);
void take_pic_cmd_fun(void);
void cam_tw_filewrite(char *);

void cam_uartdata(char *);
void cam_timer(void);
void clrcam1flags(void); 
void fun_cam_wrt_cnt(void);
void cam_mem_loc_read(void);
void Gen_CR(void);
void Gen_CW(void);
void fun_camdumpflag(void);
void Gen_CA(void);
void Gen_TC(void);
void Gen_TD1(void);
void Gen_TD2(void);
//void TEST_CAM_fun(void);


void fun_dump_cam2_wrt_cnt(void);
void cam_mem_loc_readextra(void);
void fun_cam2_read_cnt(void);
void fun_cam_read_cnt(void);
void fun_dump_cam_wrt_cnt(void);
void fun_cam2_wrt_cnt(void);
