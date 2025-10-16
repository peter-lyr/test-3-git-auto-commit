#include "include.h"

#if FUNC_MUSIC_EN
extern u8 fname_buf[100];

//#define MUSIC_EXT_NUM 6                         //一共搜索3个扩展名
//const char tbl_music_ext[MUSIC_EXT_NUM][4] = {"mp3", "wav", "wma", "ape", "fla", "sbc"};

//转换为小写字符
AT(.text.fs.scan)
unsigned char char_tolower(unsigned char c)
{
	if (c >= 'A' && c <= 'Z') {
		c -= 'A'-'a';
	}
	return c;
}

//将字符串转换为小写
AT(.text.fs.scan)
void str_tolower(char *str, uint len)
{
    uint i;
    for (i = 0; i < len; i++) {
        str[i] = char_tolower(str[i]);
    }
}

AT(.text.fs.scan)
static bool file_is_music(char *ext_name)
{
//    u8 i;
//    for (i=0; i != MUSIC_EXT_NUM; i++) {
//        if (0 == memcmp(ext_name, (char *)(tbl_music_ext[i]), 3)) {
//            return true;
//        }
//    }
//    return false;

    if (0 == memcmp(ext_name, "mp3", 3)) {
        return true;
    }
#if MUSIC_WAV_SUPPORT
    if (0 == memcmp(ext_name, "wav", 3)) {
        return true;
    }
#endif // MUSIC_WAV_SUPPORT

#if MUSIC_WMA_SUPPORT
    if (0 == memcmp(ext_name, "wma", 3)) {
        return true;
    }
#endif // MUSIC_WMA_SUPPORT

#if (MUSIC_ESBC_SUPPORT || MUSIC_LSBC_SUPPORT || MUSIC_SBC_SUPPORT)
    if (0 == memcmp(ext_name, "sbc", 3)) {
        return true;
    }
#endif //MUSIC_ESBC_SUPPORT

    return false;
}

///返回值： 0->不匹配的文件类型， 1->匹配的文件类型
AT(.text.fs.scan)
u8 music_file_filter(void)
{
    char extension[3];
    fs_get_fname_extension(extension);
    str_tolower(extension, 3);
    if (file_is_music(extension)) {
#if REC_FAST_PLAY
        bsp_update_final_rec_file_num();
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///返回值： 0->不匹配的目录类型， 1->匹配的目录类型
AT(.text.fs.scan)
u8 music_dir_filter(void)
{
#if REC_FAST_PLAY
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
        if (msc_cb.rec_scan & BIT(0)) {
            msc_cb.rec_scan |= BIT(1);
        }
    } else {
        msc_cb.rec_scan &= ~BIT(1);
    }
#endif // REC_FAST_PLAY

    return 1;
}

#if MUSIC_REC_FILE_FILTER
///只播放录音文件歌曲
u8 music_only_record_dir_filter(void)
{
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
#if REC_FAST_PLAY
        if (msc_cb.rec_scan & BIT(0)) {
            msc_cb.rec_scan |= BIT(1);
        }
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///只播放录音文件歌曲
u8 music_only_record_file_filter(void)
{
    char extension[3];
    fs_get_fname_extension(extension);
    str_tolower(extension, 3);
    if (file_is_music(extension) && fs_get_dir_depth()) {   //去掉根目录正常歌曲
#if REC_FAST_PLAY
        bsp_update_final_rec_file_num();
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///去掉录音文件
u8 music_rm_record_dir_filter(void)
{
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
        return 0;
    }

    return 1;
}
#endif // MUSIC_REC_FILE_FILTER

#if REC_FAST_PLAY
void bsp_update_final_rec_file_num(void)
{
    if (fs_get_dir_depth() && (msc_cb.rec_scan & BIT(1))) {
        if (fs_get_ftime() > sys_cb.ftime) {
            sys_cb.ftime = fs_get_ftime();
            sys_cb.rec_num = fs_get_file_count();
        }
    }
}
#endif // REC_FAST_PLAY

AT(.text.stream)
int stream_read(void *buf, unsigned int size)
{
    UINT len;
    u8 res = fs_read(buf, size, &len);
    if (res == FR_OK) {
#if MUSIC_ENCRYPT_EN
        if (msc_cb.encrypt) {
            music_stream_decrypt(buf, len);
        }
#endif
        return len;
    } else {
        return -1;
    }
}

AT(.text.stream)
bool stream_seek(unsigned int ofs, int whence)
{
#if MUSIC_ENCRYPT_EN
    if (msc_cb.encrypt) {
        if (whence == SEEK_SET) {
            ofs += 1;
        }
    }
#endif
    u8 res = fs_lseek(ofs, whence);
    if (res == FR_OK) {
        return true;
    }
    return false;
}

#if MUSIC_FILE_SORT_CHAR_EN
static bool bo_sort_en = false;
file_sort_var_t file_sort_var AT(.pff_sort.buf);
file_sort_t file_sort_buf_ONE[MUSIC_FILE_SORT_NUM] AT(.pff_sort.buf);
file_sort_t file_sort_buf_TWO[MUSIC_FILE_SORT_NUM] AT(.pff_sort.buf);
AT(.text.file_sort)
bool file_sort_en(void)
{
    return bo_sort_en;
}
AT(.text.file_sort)
void set_sort_en(bool boset)
{
    bo_sort_en = boset;
}
AT(.text.file_sort)
u16 file_sort_num(void)
{
    return MUSIC_FILE_SORT_NUM;
}

AT(.text.file_sort)
void file_sort_init(void)
{
    memset(file_sort_buf_ONE, 0, sizeof(file_sort_buf_ONE));
    memset(file_sort_buf_TWO, 0, sizeof(file_sort_buf_TWO));
    memset(&file_sort_var, 0, sizeof(file_sort_var));
    file_sort_var.dir_01_save_num = 1;
    file_sort_var.dir_02_save_num = 1;
    set_sort_en(true);
}

AT(.text.file_sort)
void dir_num_deal(char *p_dir_fname)
{
    if(0 == memcmp(p_dir_fname, "01", 2)){
        file_sort_var.file_01_offset = file_sort_var.dir_cur_num;
    }else if(0 == memcmp(p_dir_fname, "02", 2)){
        file_sort_var.file_02_offset = file_sort_var.dir_cur_num;
    }else if(0 == memcmp(p_dir_fname, "03", 2)){
        file_sort_var.file_03_offset = file_sort_var.dir_cur_num;
    }
    file_sort_var.dir_cur_num++;
}

AT(.text.file_sort)
void file_sort_char_deal(u16 dir_count, char *p_file_fname, u16 file_count)
{
    file_sort_t temp_file_sort_1;
    file_sort_t temp_file_sort_2;
    file_sort_t *p_sort1 = file_sort_buf_ONE;
    file_sort_t *p_sort2 = file_sort_buf_TWO;
    // printf("offset:%d, %d, %d\n", file_sort_var.file_01_offset, file_sort_var.file_02_offset, dir_count);
    if(file_sort_var.file_01_offset == dir_count){
        temp_file_sort_1.file_count = file_count;
        memcpy(&temp_file_sort_1.fname, p_file_fname, 5);
        // printf("temp_file_sort_1.fname:[%s]\n", temp_file_sort_1.fname);

        if(1 == file_sort_var.dir_01_save_num){//第一首直接放数组1
            p_sort1[file_sort_var.dir_01_save_num] = temp_file_sort_1;
            file_sort_var.dir_01_save_num++;
        }else if((file_sort_var.dir_01_save_num > 1) && (file_sort_var.dir_01_save_num < MUSIC_FILE_SORT_NUM)){
            for(u8 i = 1; i <= file_sort_var.dir_01_save_num; i++){//从数组1开始比较到数组n
                temp_file_sort_1 = p_sort1[i];

                int re_sta = memcmp(p_file_fname ,&temp_file_sort_1.fname, 3);
                if(!re_sta){
                    printf("dir1 file name error\n");
                    break;
                }else if(re_sta < 0){//s1 < s2
                    for(u8 j = file_sort_var.dir_01_save_num; j > i; j--){
                        p_sort1[j] = p_sort1[j-1];
                    }
                    temp_file_sort_1.file_count = file_count;
                    memcpy(&temp_file_sort_1.fname, p_file_fname, 3);
                    p_sort1[i] = temp_file_sort_1;//插值
                    break;
                }

                if(i == file_sort_var.dir_01_save_num){//如果遍历到数组n还未找到比之大的，则直接放最后也就是数组n位置
                    temp_file_sort_1.file_count = file_count;
                    memcpy(&temp_file_sort_1.fname, p_file_fname, 3);
                    p_sort1[file_sort_var.dir_01_save_num] = temp_file_sort_1;
                }
            }
            file_sort_var.dir_01_save_num++;
        }else{
            printf("dir_01_save_num error\n");
        }
    }else if(file_sort_var.file_02_offset == dir_count){
        temp_file_sort_2.file_count = file_count;
        memcpy(&temp_file_sort_2.fname, p_file_fname, 5);
        // printf("temp_file_sort_2.fname:[%s]\n", temp_file_sort_2.fname);

        if(1 == file_sort_var.dir_02_save_num){//第一首直接放数组1
            p_sort2[file_sort_var.dir_02_save_num] = temp_file_sort_2;
            file_sort_var.dir_02_save_num++;
        }else if((file_sort_var.dir_02_save_num > 1) && (file_sort_var.dir_02_save_num < MUSIC_FILE_SORT_NUM)){
            for(u8 i = 1; i <= file_sort_var.dir_02_save_num; i++){//从数组1开始比较到数组n
                temp_file_sort_2 = p_sort2[i];

                int re_sta = memcmp(p_file_fname ,&temp_file_sort_2.fname, 3);
                if(!re_sta){
                    printf("dir2 file name error\n");
                    break;
                }else if(re_sta < 0){//s1 < s2
                    for(u8 j = file_sort_var.dir_02_save_num; j > i; j--){
                        p_sort2[j] = p_sort2[j-1];
                    }
                    temp_file_sort_2.file_count = file_count;
                    memcpy(&temp_file_sort_2.fname, p_file_fname, 3);
                    p_sort2[i] = temp_file_sort_2;//插值
                    break;
                }

                if(i == file_sort_var.dir_02_save_num){//如果遍历到数组n还未找到比之大的，则直接放最后也就是数组n位置
                    temp_file_sort_2.file_count = file_count;
                    memcpy(&temp_file_sort_2.fname, p_file_fname, 3);
                    p_sort2[file_sort_var.dir_02_save_num] = temp_file_sort_2;
                }
            }
            file_sort_var.dir_02_save_num++;
        }else{
            printf("dir_01_save_num error\n");
        }
    }else{

    }
    return;
}
#endif //MUSIC_FILE_SORT_CHAR_EN

#endif // FUNC_MUSIC_EN
