depend(0x00090000);

config(SUB, "系统配置", "系统的相关配置");
config(LISTVAL, "提示音语言选择", "选择系统的语言", LANG_ID, BIT, 2, 0, 4, ("英文", 0), ("中文", 1), ("中英文(出厂默认英文)", 2), ("中英文(出厂默认中文)", 3));
config(CHECK, "低电提示", "当电压比较低的时候，会有提示音警告", LOWPOWER_WARNING_EN, 0);
config(LIST, "低电语音提示电压", "选择低电语音提示电压", LPWR_WARNING_VBAT, 19, "1.9V", "2.0V", "2.1V", "2.2V", "2.3V", "2.4V", "2.5V", "2.6V", "2.7V", "2.8V", "2.9V", "3.0V", "3.1V", "3.2V", "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", 4, LOWPOWER_WARNING_EN);
config(LIST, "低电关机电压", "选择低电关机电压", LPWR_OFF_VBAT, 20, "不关机", "1.9V", "2.0V", "2.1V", "2.2V", "2.3V", "2.4V", "2.5V", "2.6V", "2.7V", "2.8V", "2.9V", "3.0V", "3.1V", "3.2V", "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", 2, LOWPOWER_WARNING_EN);
config(BYTE, "低电语音播报周期(秒)", "设置低电语音播报周期(秒)", LPWR_WARNING_PERIOD, 1, 240, 30, LOWPOWER_WARNING_EN);
config(LISTVAL, "音量级数", "选择系统音量级数", VOL_MAX, BIT, 2, 0, 3, ("16级音量", 0), ("32级音量", 1), ("50级音量", 2));
config(BYTE, "开机默认音量", "开机默认音量级数", SYS_INIT_VOL, 0, 50, 5);
config(BYTE, "提示音播放音量", "提示音播放音量级数", WARNING_VOLUME, 0, 50, 6);

config(SUB, "电源配置", "电源的相关配置");
config(LISTVAL, "VDDIO电压", "选择VDDIO电压, VDDAUD与VDDIO短接可能需要调VDDIO电压。", VDDIO_SEL, BIT, 5, 24, 24, ("1.5V", 0), ("1.6V", 2), ("1.7V", 3), ("1.8V", 4), ("1.9V", 6), ("2.0V", 7), ("2.1V", 8), ("2.2V", 10), ("2.3V", 11), ("2.4V", 12), ("2.5V", 14), ("2.6V", 15), ("2.7V", 16), ("2.8V", 18), ("2.9V", 19), ("3.0V", 20), ("3.1V", 22), ("3.2V", 23), ("3.3V", 24), ("3.4V", 26), ("3.5V", 27), ("3.6V", 28), ("3.7V", 30), ("3.8V", 31));
config(LISTVAL, "复位电压", "选择复位电压,vbat电池供电的最低电压设置。", RESET_SEL, BIT, 3, 4, 8,  ("1.8V", 0), ("2.0V", 1), ("2.2V", 2), ("2.4V", 3), ("2.6V", 4), ("2.8V", 5), ("3.0V", 6), ("1.7V", 7));
config(LISTVAL, "中断电压", "选择中断电压,vbat电池供电的最低电压设置。", RESET_SEL2, BIT, 3, 6, 8,  ("1.8V", 0), ("2.0V", 1), ("2.2V", 2), ("2.4V", 3), ("2.6V", 4), ("2.8V", 5), ("3.0V", 6), ("1.7V", 7));

config(SUB, "DAC配置", "DAC的相关配置");
config(LISTVAL, "DAC最大音量", "配置DAC最大输出幅度", DAC_MAX_GAIN, BIT, 4, 1, 16, ("0DB", 0), ("-1DB", 1), ("-2DB", 2), ("-3DB", 3), ("-4DB", 4), ("-5DB", 5), ("-6DB", 6), ("-7DB", 7), ("-8DB", 8), ("-9DB", 9), ("-10DB", 10), ("-11DB", 11), ("-12DB", 12), ("-13DB", 13), ("-14DB", 14), ("-15DB", 15));
config(BIT, "DAC软件增益(0~31DB)", "配置DAC后置软件增益, Step 1DB, 总共(0~31DB)", DAC_SOFT_GAIN, 6, 0, 63, 0);

config(SUB, "MIC配置", "MIC的相关配置");
config(LISTVAL, "MIC偏置电路配置", "MIC电路BIAS配置，省电容电阻配置", MIC_BIAS_METHOD, BIT, 3, 0, 5, ("省电阻单端输入", 0), ("省电阻差分输入", 1), ("省电阻电容", 2), ("外部电阻电容单端输入", 3), ("外部电阻电容差分输入", 4));
config(LEVEL, 0x00);
config(RELATE, MIC_BIAS_METHOD, <2);
config(LISTVAL, "MIC内部电阻选择", "MIC内部电阻选择", MIC_RIAS_SEL,   BIT, 4, 8, 15, ("16.36K", 1), ("8.18K", 2), ("5.45K", 3), ("4.09K", 4), ("3.27K", 5), ("2.73K", 6), ("2.34K", 7), ("2.05K", 8), ("1.82K", 9), ("1.64K", 10), ("1.49K", 11), ("1.36K", 12), ("1.26K", 13), ("1.17K", 14), ("1.09K", 15));
config(LEVEL, 0x03);
config(RELATE)
config(RELATE, MIC_BIAS_METHOD, !2);
config(CHECK, "MIC_BIAS使能", "PA13是否用作mic_bias", MIC_BIAS_EN, 1);
config(LISTVAL, "MIC模拟增益(6~12DB)", "MIC模拟增益, 6DB或12DB", MIC_ANL_GAIN, BIT, 2, 0, 2 ("6DB", 0), ("12DB", 1));
config(RELATE)
config(BIT, "MIC数字增益(0~36DB)", "MIC SDADC数字增益, Step 1DB, 总共(0~39DB)", MIC_DIG_GAIN, 6, 0, 36, 24);
config(BIT, "MIC软件增益(0~31DB)", "MIC SDADC软件增益, Step 1DB, 总共(0~31DB)", MIC_SOFT_GAIN, 6, 0, 63, 0);

config(SUB, "按键配置", "选择按键及按键功能配置");
config(CHECK, "ADKEY1功能", "是否使用第一组ADKEY", USER_ADKEY1_EN, 0);
config(CHECK, "ADKEY2功能", "是否使用第二组ADKEY", USER_ADKEY2_EN, 0);
config(CHECK, "IOKEY功能", "是否使用IOKEY", USER_IOKEY_EN, 0);
config(LISTVAL, "ADKEY1通路选择", "第一组ADKEY io mapping select", ADKEY_CH,   BIT, 4, 9, 14, ("ADCCH_PA0", 0), ("ADCCH_PA1", 1), ("ADCCH_PA2", 2), ("ADCCH_PA3", 3), ("ADCCH_PA4", 4), ("ADCCH_PA5", 5), ("ADCCH_PA10", 6), ("ADCCH_PA11", 7), ("ADCCH_PA13", 8), ("ADCCH_PA14", 9), ("ADCCH_PA15", 10), ("ADCCH_PB0", 11), ("ADCCH_PB3", 12), ("ADCCH_PB4", 13), USER_ADKEY1_EN);
config(LISTVAL, "ADKEY2通路选择", "第一组ADKEY io mapping select", ADKEY_CH2,   BIT, 4, 9, 14, ("ADCCH_PA0", 0), ("ADCCH_PA1", 1), ("ADCCH_PA2", 2), ("ADCCH_PA3", 3), ("ADCCH_PA4", 4), ("ADCCH_PA5", 5), ("ADCCH_PA10", 6), ("ADCCH_PA11", 7), ("ADCCH_PA13", 8), ("ADCCH_PA14", 9), ("ADCCH_PA15", 10), ("ADCCH_PB0", 11), ("ADCCH_PB3", 12), ("ADCCH_PB4", 13), USER_ADKEY2_EN);
config(CHECK, "按键多击功能", "是否使能按键多击（2/3/4/5击）功能", USER_KEY_MULTI_PRESS_EN, 1);
config(LISTVAL, "双击响应时间选择", "选择双击按键响应间隔时间", DOUBLE_KEY_TIME, BIT, 3, 1, 8, ("200ms", 0), ("300ms", 1), ("400ms", 2), ("500ms", 3), ("600ms", 4), ("700ms", 5), ("800ms", 6), ("900ms", 7), USER_KEY_MULTI_PRESS_EN);
config(LISTVAL, "软开机长按时间选择", "长按PWRKEY多长时间后软开机", PWRON_PRESS_TIME,  BIT, 3, 3, 8, ("0.1秒", 0), ("0.5秒", 1), ("1秒", 2), ("1.5秒", 3), ("2秒", 4), ("2.5秒", 5), ("3秒", 6), ("3.5秒", 7));
config(LISTVAL, "软关机长按时间选择", "长按PWRKEY多长时间后软关机", PWROFF_PRESS_TIME, BIT, 3, 3, 8, ("1.5秒", 0), ("2秒", 1), ("2.5秒", 2), ("3秒", 3), ("3.5秒", 4), ("4秒", 5), ("4.5秒", 6), ("5秒", 7));

config(CHECK, "支持多击的按键定制", "是否配置支持多击的按键，不勾选使用SDK默认的双击按键配置列表", KEY_MULTI_CONFIG_EN, 0, USER_KEY_MULTI_PRESS_EN);
config(LISTVAL, "多击按键1选择", "支持多击第1个按键", DBLKEY_NUM0, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键2选择", "支持多击第2个按键", DBLKEY_NUM1, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键3选择", "支持多击第3个按键", DBLKEY_NUM2, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键4选择", "支持多击第4个按键", DBLKEY_NUM3, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键5选择", "支持多击第5个按键", DBLKEY_NUM4, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), KEY_MULTI_CONFIG_EN);

config(LISTVAL, "IOKEY按键1的IO", "IOKEY第1个按键的GPIO口选择", IOKEY_IO0, BIT, 5, 0, 29, ("None", 0), ("PA0", 1), ("PA1", 2), ("PA2", 3), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PA8", 9), ("PA9", 10), ("PA10", 11), ("PA11", 12), ("PA12", 13), ("PA13", 14), ("PA14", 15), ("PA15", 16), ("PB0", 17), ("PB1", 18), ("PB2", 19), ("PB3", 20), ("PB4", 21), ("PB5", 22), ("PB6", 23), ("PB7", 24), ("PB8", 25), ("PB9", 26), ("PB10", 27), ("PB11", 28), USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键2的IO", "IOKEY第2个按键的GPIO口选择", IOKEY_IO1, BIT, 5, 0, 29, ("None", 0), ("PA0", 1), ("PA1", 2), ("PA2", 3), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PA8", 9), ("PA9", 10), ("PA10", 11), ("PA11", 12), ("PA12", 13), ("PA13", 14), ("PA14", 15), ("PA15", 16), ("PB0", 17), ("PB1", 18), ("PB2", 19), ("PB3", 20), ("PB4", 21), ("PB5", 22), ("PB6", 23), ("PB7", 24), ("PB8", 25), ("PB9", 26), ("PB10", 27), ("PB11", 28), USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键3的IO", "IOKEY第3个按键的GPIO口选择", IOKEY_IO2, BIT, 5, 0, 29, ("None", 0), ("PA0", 1), ("PA1", 2), ("PA2", 3), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PA8", 9), ("PA9", 10), ("PA10", 11), ("PA11", 12), ("PA12", 13), ("PA13", 14), ("PA14", 15), ("PA15", 16), ("PB0", 17), ("PB1", 18), ("PB2", 19), ("PB3", 20), ("PB4", 21), ("PB5", 22), ("PB6", 23), ("PB7", 24), ("PB8", 25), ("PB9", 26), ("PB10", 27), ("PB11", 28), USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键4的IO", "IOKEY第4个按键的GPIO口选择", IOKEY_IO3, BIT, 5, 0, 29, ("None", 0), ("PA0", 1), ("PA1", 2), ("PA2", 3), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PA8", 9), ("PA9", 10), ("PA10", 11), ("PA11", 12), ("PA12", 13), ("PA13", 14), ("PA14", 15), ("PA15", 16), ("PB0", 17), ("PB1", 18), ("PB2", 19), ("PB3", 20), ("PB4", 21), ("PB5", 22), ("PB6", 23), ("PB7", 24), ("PB8", 25), ("PB9", 26), ("PB10", 27), ("PB11", 28), USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键5的IO", "IOKEY第5个按键的GPIO口选择", IOKEY_IO4, BIT, 5, 0, 29, ("None", 0), ("PA0", 1), ("PA1", 2), ("PA2", 3), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PA8", 9), ("PA9", 10), ("PA10", 11), ("PA11", 12), ("PA12", 13), ("PA13", 14), ("PA14", 15), ("PA15", 16), ("PB0", 17), ("PB1", 18), ("PB2", 19), ("PB3", 20), ("PB4", 21), ("PB5", 22), ("PB6", 23), ("PB7", 24), ("PB8", 25), ("PB9", 26), ("PB10", 27), ("PB11", 28), USER_IOKEY_EN);

config(LISTVAL, "IOKEY按键1功能", "IOKEY第1个按键功能定制", IOKEY_NUM0, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键2功能", "IOKEY第2个按键功能定制", IOKEY_NUM1, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键3功能", "IOKEY第3个按键功能定制", IOKEY_NUM2, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键4功能", "IOKEY第4个按键功能定制", IOKEY_NUM3, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键5功能", "IOKEY第5个按键功能定制", IOKEY_NUM4, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), USER_IOKEY_EN);
config(LISTVAL, "两IO中间的按键功能", "IOKEY1与IOKEY2两IO中间的按键", IOKEY_MIDKEY_NUM, BIT, 5, 0, 22, ("None", 0), ("PP/POWER", 1), ("P/P", 2), ("POWER", 3), ("NEXT/VOL+", 4), ("VOL+/NEXT", 5), ("PREV/VOL-", 6), ("VOL-/PREV", 7), ("VOL+", 8), ("VOL-", 9), ("NEXT", 10), ("PREV", 11), ("MODE", 12), ("PP/MODE", 13), ("HSF", 14), ("EQ", 15), ("REPEAT", 16), ("MUTE", 17), ("MODE/POWER", 18), ("REC", 19), ("PLAY/HSF", 20), ("A-B", 21), USER_IOKEY_EN);

config(SUB, "SPI1配置", "SPI1相关配置");
config(LISTVAL, "SPI1主从模式", "选择主机或从机", SPI1_SLAVE_MASTER, BIT, 2, 0, 2, ("SPI1从机", 0), ("SPI1主机", 1));

config(SUB, "LED灯配置", "选择及配置LED状态指示灯");
config(CHECK, "系统指示灯(蓝灯)", "是否使用系统状态指示灯", LED_DISP_EN, 1);
config(CHECK, "电源状态灯(红灯)", "是否使用充电/电源指示灯", LED_PWR_EN, 1);
config(CHECK, "电池低电闪灯", "电池低电时，红灯或蓝灯是否闪烁？", RLED_LOWBAT_EN, 0, LED_DISP_EN);
config(CHECK, "省电阻1个IO推两个灯", "省电阻1个IO推两个灯，不能兼容升级功能", PORT_2LED_RESLESS_EN, 1, LED_DISP_EN);
config(LISTVAL, "蓝灯IO选择", "蓝灯的GPIO口选择", BLED_IO_SEL, BIT, 5, 0, 29, ("None", 0), ("PA0", 1), ("PA1", 2), ("PA2", 3), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PA8", 9), ("PA9", 10), ("PA10", 11), ("PA11", 12), ("PA12", 13), ("PA13", 14), ("PA14", 15), ("PA15", 16), ("PB0", 17), ("PB1", 18), ("PB2", 19), ("PB3", 20), ("PB4", 21), ("PB5", 22), ("PB6", 23), ("PB7", 24), ("PB8", 25), ("PB9", 26), ("PB10", 27), ("PB11", 28), LED_DISP_EN);
config(LISTVAL, "红灯IO选择", "红灯的GPIO口选择", RLED_IO_SEL, BIT, 5, 0, 29, ("None", 0), ("PA0", 1), ("PA1", 2), ("PA2", 3), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PA8", 9), ("PA9", 10), ("PA10", 11), ("PA11", 12), ("PA12", 13), ("PA13", 14), ("PA14", 15), ("PA15", 16), ("PB0", 17), ("PB1", 18), ("PB2", 19), ("PB3", 20), ("PB4", 21), ("PB5", 22), ("PB6", 23), ("PB7", 24), ("PB8", 25), ("PB9", 26), ("PB10", 27), ("PB11", 28), LED_PWR_EN);

config(LEVEL, 0x100);
makecfgfile(xcfg.bin);
makecfgdef(xcfg.h);
xcopy(xcfg.h, ../../xcfg.h);
