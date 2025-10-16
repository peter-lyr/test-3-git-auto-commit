#ifndef _MIDI_KEYBOARD_H
#define _MIDI_KEYBOARD_H

#define MKEY_25     25      //midi note����:48~72   
#define MKEY_32     32      //midi note����:48~79
#define MKEY_49     49      //midi note����:36~84
#define MKEY_61     61      //midi note����:36~96
#define MKEY_88     88      //midi note����:21~108

#if   (MIDI_KEYS_NUM == MKEY_25)
#define MKEY_NOTE_START         48              //midi������ʼ����
#define MKEY_RANGE_MIN          KEY_MAT_1       //�ټ���ʼ
#define MKEY_RANGE_MAX          KEY_MAT_25      //�ټ�����
#elif (MIDI_KEYS_NUM == MKEY_32)
#define MKEY_NOTE_START         48
#define MKEY_RANGE_MIN          KEY_MAT_1
#define MKEY_RANGE_MAX          KEY_MAT_32
#elif (MIDI_KEYS_NUM == MKEY_49)
#define MKEY_NOTE_START         36
#define MKEY_RANGE_MIN          KEY_MAT_1
#define MKEY_RANGE_MAX          KEY_MAT_49
#elif (MIDI_KEYS_NUM == MKEY_61)
#define MKEY_NOTE_START         36
#define MKEY_RANGE_MIN          KEY_MAT_1
#define MKEY_RANGE_MAX          KEY_MAT_61
#elif (MIDI_KEYS_NUM == MKEY_88)
#define MKEY_NOTE_START         21
#define MKEY_RANGE_MIN          KEY_MAT_1
#define MKEY_RANGE_MAX          KEY_MAT_88
#endif

#define MKEY_DEF_CH             MIDI_CHANNEL_1  //midi����Ĭ��ʹ��channel1
#define MKEY_DEF_VELOCITY       100             //midi����Ĭ������



void midi_keyboard_msg_proc(u16 msg);
#endif
