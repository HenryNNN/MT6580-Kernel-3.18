#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
//	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
//	#include <asm/arch/mt_gpio.h>
#else
//	#include <mach/mt_gpio.h>
#endif


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT (800)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xDD   // END OF REGISTERS MARKER
#define LCM_ID       (0x9806)


//#define LCM_DSI_CMD_MODE									0


#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif
	
#ifdef BUILD_LK
#define LCM_PRINT printf
#else
#if defined(BUILD_UBOOT)
	#define LCM_PRINT printf
#else
	#define LCM_PRINT printk
#endif
#endif

#define LCM_DBG(fmt, arg...) \
	LCM_PRINT("[LCM_ILI9806E_FWVGA_DSI_VDO_TXD] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

//#define FRAME_WIDTH  (480)
//#define FRAME_HEIGHT (854)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

static unsigned int lcm_compare_id(void);
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)										lcm_util.dsi_read_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
 
 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] =
{
{0xFF,5,{0xFF,0x98,0x06,0x04,0x01}},
{0x08,1,{0X10}},
{0x21,1,{0X01}},
{0x30,1,{0X02}},
{0x31,1,{0X02}},
{0x40,1,{0X18}},
{0x41,1,{0X66}},
{0x42,1,{0X02}},
{0x43,1,{0X09}},
{0x44,1,{0X09}},
{0x45,1,{0X1b}},
{0x46,1,{0X44}},
{0x47,1,{0X44}},
{0x50,1,{0XAC}},
{0x51,1,{0XA8}},
{0x52,1,{0X00}},
{0x53,1,{0X5c}},
{0xA0,1,{0X00}},
{0xA1,1,{0X06}},
{0xA2,1,{0X17}},
{0xA3,1,{0X19}},
{0xA4,1,{0X14}},
{0xA5,1,{0X1F}},
{0xA6,1,{0X0F}},
{0xA7,1,{0X0F}},
{0xA8,1,{0X01}},
{0xA9,1,{0X06}},
{0xAA,1,{0X00}},
{0xAB,1,{0X02}},
{0xAC,1,{0X0A}},
{0xAD,1,{0X2C}},
{0xAE,1,{0X28}},
{0xAF,1,{0X00}},
{0xC0,1,{0X00}},
{0xC1,1,{0X08}},
{0xC2,1,{0X16}},
{0xC3,1,{0X14}},
{0xC4,1,{0X0E}},
{0xC5,1,{0X1F}},
{0xC6,1,{0X0F}},
{0xC7,1,{0X0A}},
{0xC8,1,{0X00}},
{0xC9,1,{0X08}},
{0xCA,1,{0X05}},
{0xCB,1,{0X04}},
{0xCC,1,{0X0C}},
{0xCD,1,{0X19}},
{0xCE,1,{0X1B}},
{0xCF,1,{0X00}},
{0xFF,5,{0xFF,0x98,0x06,0x04,0x06}},
{0x00,1,{0X20}},
{0x01,1,{0X05}},
{0x02,1,{0X00}},
{0x03,1,{0X00}},
{0x04,1,{0X01}},
{0x05,1,{0X01}},
{0x06,1,{0X88}},
{0x07,1,{0X04}},
{0x08,1,{0X01}},
{0x09,1,{0X90}},
{0x0A,1,{0X04}},
{0x0B,1,{0X01}},
{0x0C,1,{0X01}},
{0x0D,1,{0X01}},
{0x0E,1,{0X00}},
{0x0F,1,{0X00}},
{0x10,1,{0X55}},
{0x11,1,{0X50}},
{0x12,1,{0X01}},
{0x13,1,{0X0C}},
{0x14,1,{0X0D}},
{0x15,1,{0X43}},
{0x16,1,{0X0B}},
{0x17,1,{0X00}},
{0x18,1,{0X00}},
{0x19,1,{0X00}},
{0x1A,1,{0X00}},
{0x1B,1,{0X00}},
{0x1C,1,{0X00}},
{0x1D,1,{0X00}},
{0x20,1,{0X01}},
{0x21,1,{0X23}},
{0x22,1,{0X45}},
{0x23,1,{0X67}},
{0x24,1,{0X01}},
{0x25,1,{0X23}},
{0x26,1,{0X45}},
{0x27,1,{0X67}},
{0x30,1,{0X02}},
{0x31,1,{0X22}},
{0x32,1,{0X11}},
{0x33,1,{0XAA}},
{0x34,1,{0XBB}},
{0x35,1,{0X66}},
{0x36,1,{0X00}},
{0x37,1,{0X22}},
{0x38,1,{0X22}},
{0x39,1,{0X22}},
{0x3A,1,{0X22}},
{0x3B,1,{0X22}},
{0x3C,1,{0X22}},
{0x3D,1,{0X22}},
{0x3E,1,{0X22}},
{0x3F,1,{0X22}},
{0x40,1,{0X22}},
{0x53,1,{0X10}},
{0xFF,5,{0xFF,0x98,0x06,0x04,0x07}},
{0x18,1,{0X1D}},
{0x17,1,{0X32}},
{0x02,1,{0X77}},
{0xE1,1,{0X79}},
{0x26,1,{0XB2}},
{0x06,1,{0X13}},
{0xFF,5,{0xFF,0x98,0x06,0x04,0x00}},
{0x35,1,{0X00}},
{0x11,1,{0X00}},
{REGFLAG_DELAY, 120, {}},
{0x29,1,{0X00}},
{REGFLAG_DELAY, 20, {}},
{0xFF,5,{0xFF,0x98,0x06,0x04,0x01}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_set_window[] = {

};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
	//Normal mode on
	{0x13, 1, {0x00}},
	{REGFLAG_DELAY,20,{}},
    // Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},
    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
    // Sleep Mode On
	{0x28, 1, {0xE7, 0x0E}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_backlight_level_setting[] = {

};

static struct LCM_setting_table lcm_compare_id_setting[] = {

	{0xD3,	3,	{0xFF, 0x83, 0x79}},
	{REGFLAG_DELAY, 10, {}}, 	

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
		        dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;
		
 //   params->physical_width  = 61.63;
//    params->physical_height = 109.65;
    
		// enable tearing-free
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
		//params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

		params->dsi.mode   = SYNC_PULSE_VDO_MODE;

		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=480*3;

 //edit by Magnum 2013-7-25 , solve esd read id error
	//	 cycle_time = (4 * 1000 * div2 * div1 * pre_div * post_div)/ (fbk_sel * (fbk_div+0x01) * 26) + 
	// 1 = 
  // ui = (1000 * div2 * div1 * pre_div * post_div)/ (fbk_sel * (fbk_div+0x01) * 26 * 2) + 1;
		
		params->dsi.vertical_sync_active				= 5;
		params->dsi.vertical_backporch					= 9;
		params->dsi.vertical_frontporch					= 8;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 


		params->dsi.horizontal_sync_active				= 8;
		params->dsi.horizontal_backporch				= 50;
		params->dsi.horizontal_frontporch				= 46;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		params->dsi.compatibility_for_nvk = 0;	
		params->dsi.ssc_disable=0;
		params->dsi.ssc_range=6;
	    params->dsi.PLL_CLOCK				= 170;
		params->dsi.esd_check_enable = 1;
		params->dsi.customization_esd_check_enable = 0;
		params->dsi.lcm_esd_check_table[0].cmd          = 0x0A;
		params->dsi.lcm_esd_check_table[0].count        = 1;
		params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;

		/* ESD or noise interference recovery For video mode LCM only. */
		// Send TE packet to LCM in a period of n frames and check the response.
	/*	params->dsi.lcm_int_te_monitor = FALSE;
		params->dsi.lcm_int_te_period = 1;		// Unit : frames

		// Need longer FP for more opportunity to do int. TE monitor applicably.
		if(params->dsi.lcm_int_te_monitor)
			params->dsi.vertical_frontporch *= 2;
		
		// Monitor external TE (or named VSYNC) from LCM once per 2 sec. (LCM VSYNC must be wired to baseband TE pin.)
		params->dsi.lcm_ext_te_monitor = FALSE;
		// Non-continuous clock
		params->dsi.noncont_clock = TRUE;
		params->dsi.noncont_clock_period = 2;	// Unit : frames  */
}


static void lcm_init(void)
{
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}



static void lcm_suspend(void)
{

	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(120);
}

static void lcm_resume(void)
{
	lcm_init();
}
//extern void Tinno_set_HS_read();
//extern void Tinno_restore_HS_read();
//static unsigned int lcm_esd_test = FALSE; 

static unsigned int lcm_compare_id(void)
{
        int array[4];
        char buffer[5];
        char id_high=0;
        char id_midd=0;
        char id_low=0;
        int id=0;

        //Do reset here
        SET_RESET_PIN(1);
	MDELAY(2);
        SET_RESET_PIN(0);
        MDELAY(25);       
        SET_RESET_PIN(1);
        MDELAY(120);   
        
        array[0]=0x00063902;
        array[1]=0x0698ffff;
        array[2]=0x00000104;
        dsi_set_cmdq(array, 3, 1);
        MDELAY(10);
 
        array[0]=0x00023700;//0x00023700;
        dsi_set_cmdq(array, 1, 1);
        //read_reg_v2(0x04, buffer, 3);//if read 0x04,should get 0x008000,that is both OK.
    
        read_reg_v2(0x00, buffer,1);
        id_high = buffer[0]; ///////////////////////0x98
	LCM_DBG("ILI9806:  id_high =%x\n", id_high);

	read_reg_v2(0x01, buffer,1);
	id_midd = buffer[0]; ///////////////////////0x06
	LCM_DBG("ILI9806:  id_midd =%x\n", id_midd);

	read_reg_v2(0x02, buffer,1);
	id_low = buffer[0]; ////////////////////////0x04
	LCM_DBG("ILI9806:  id_low =%x\n", id_low);
 
//	id = id_high;
        id = (id_high << 8) | id_midd;
		LCM_DBG("ILI9806: id = %x\n", id);
	return (LCM_ID == id)?1:0;
}


LCM_DRIVER ili9806e_wvga_dsi_vdo_lx_boe40_lcm_drv = 
{
    .name			= "ili9806e_wvga_dsi_vdo_lx_boe40",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,
};

