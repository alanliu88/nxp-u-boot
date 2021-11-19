#include <common.h>
#include <i2c.h>
#include <exports.h>
#include <asm/io.h>
#include <linux/io.h>

#define CPLD_I2C_ADDR  0x42

#define FPGA_CONFIG_CTRL 0x04
#define FPGA_CONFIG_STATUS 0x05

//4.4	FPGA Configuration Control Register
#define PROGRAM_B 0x04
#define CFG_CS    0x01

#define INIT_B    0x02
#define CFG_DONE  0x01

#define GPIO1_GPDIR  0x2300000
#define GPIO1_GPDAT  0x2300008

#define GPIO2_GPDIR  0x2310000
#define GPIO2_GPDAT  0x2310008

static struct udevice* i2cdev;

int init_i2c(void)
{
	int ret = 0;
	
	ret = i2c_get_chip_for_busnum(CONFIG_SYS_I2C_CPLD_BUS_NUM, 
			CONFIG_SYS_I2C_CPLD_ADDR, 1, &i2cdev);
	if (ret != 0) 
	{
		printf("%s failed to get i2c dev %d:0x%x, ret=%d\n",
			__func__, CONFIG_SYS_I2C_CPLD_BUS_NUM, CONFIG_SYS_I2C_CPLD_ADDR,ret);
	}

	return ret;
}

uint8_t cpld_read(uint8_t offset)
{
	return dm_i2c_reg_read(i2cdev, offset); 
}

void cpld_write(uint8_t offset, uint8_t val)
{
	dm_i2c_reg_write(i2cdev, offset, val);
}
/*
  GPIO1_16/SDHC1_DAT0     CPU_GPIO_Data0, for FPGA configuration
  GPIO1_17/SDHC1_DAT1     CPU_GPIO_Data1, for FPGA configuration
  GPIO1_18/SDHC1_DAT2     CPU_GPIO_Data2, for FPGA configuration
  GPIO1_19/SDHC1_DAT3     CPU_GPIO_Data3, for FPGA configuration
  GPIO2_00/ USB1_DRVVBUS  CPU_CFG_CLKIN
*/
void init_gpio(void)
{
	uint32_t val;
	
	//init data gpio
	val = ((1<<15)|(1<<14)|(1<<13)|(1<<12));
	val = cpu_to_be32(val);
	__raw_writel(val, (volatile void __iomem *)GPIO1_GPDIR);
	__raw_writel(0, (volatile void __iomem *)GPIO1_GPDAT);

	//init clock gpio.
	__raw_writel(0x00000080, (volatile void __iomem *)GPIO2_GPDIR);
	__raw_writel(0, (volatile void __iomem *)GPIO2_GPDAT);
	
}

void gdelay(void)
{
	uint64_t tmp;

	tmp = get_ticks() + 10;	/* get current timestamp */

	while (get_ticks() < tmp+1)	/* loop till event */	
	/*NOP*/;
}

void gpio_xmit_byte(uint8_t val)
{
	uint32_t reg_val = 0;

	{
		uint8_t swap_val = 0;
		swap_val |= ((val & 0x80) >> 3);
		swap_val |= ((val & 0x40) >> 1);
		swap_val |= ((val & 0x20) << 1);
		swap_val |= ((val & 0x10) << 3);
		swap_val |= ((val & 0x08) >> 3);
		swap_val |= ((val & 0x04) >> 1);
		swap_val |= ((val & 0x02) << 1);
		swap_val |= ((val & 0x01) << 3);
		val = swap_val;
	}

	reg_val = cpu_to_be32(((val & 0xf0) << 8));
	__raw_writel(reg_val, (volatile void __iomem *)GPIO1_GPDAT);
	//gdelay();
	
	//clock high
	__raw_writel(0x00000080, (volatile void __iomem *)GPIO2_GPDAT);
	
	reg_val = cpu_to_be32(((val & 0x0f) << 12));
	__raw_writel(reg_val, (volatile void __iomem *)GPIO1_GPDAT);		
	//gdelay();

	//clock low
	__raw_writel(0, (volatile void __iomem *)GPIO2_GPDAT);
}

/*
1)	SW write a ‘1’ to field ‘FPGA chip select’ of FPGA Configuration Control Register to activate the FPGA chip for configuration. 
    This field should be asserted before configuration finished.
2)	SW write a ‘0’ and then write a ‘1’ to field ‘PROGRAM_B’ of FPGA Configuration Control Register, which tells the FPGA to start configuration.
3)	SW wait until field ‘INIT_B’ of FPGA Status Register goes to ‘1’, which indicates FPGA initialization finished.
4)	CPU send bitstream by GPIO.
5)	SW read DONE signal of FPGA Configuration Status Register. The configuration is completed if DONE goes high during 10 times read, 
    otherwise the configuration is failed.
6)	SW write a ‘0’ to field ‘FPGA chip select’ of FPGA Configuration Control Register to disable the FPGA chip for configuration.
*/

void program_fpga(uint8_t* data, int len )
{
	int i = 0;
	uint8_t fpga_cfg_status = 0;
	int timeout = 0;

	init_gpio();
	init_i2c();

	//reset
	cpld_write(FPGA_CONFIG_CTRL, 0);
	udelay(10);

	cpld_write(FPGA_CONFIG_CTRL, CFG_CS);
	udelay(1);

	//debug	 only 
	fpga_cfg_status = cpld_read(FPGA_CONFIG_STATUS);
	printf("after cs:  fpga configure status reg:0x05,value: 0x%x \n", fpga_cfg_status);

	cpld_write(FPGA_CONFIG_CTRL, PROGRAM_B | CFG_CS);
	
	timeout = 10;
	do {
		udelay(1000);
		timeout--;
		fpga_cfg_status = cpld_read(FPGA_CONFIG_STATUS);
	}while ((!(fpga_cfg_status & INIT_B)) && timeout > 0);

	printf("finish wait: fpga configure status reg:0x05,value: 0x%x \n", fpga_cfg_status);
	if (!(fpga_cfg_status & INIT_B)) {
		printf("warning: start fpga configuration timeout!\n");
		return;
	}
	
	//gpio write
	for (i = 0; i < len; i++) {
		gpio_xmit_byte(*(data+i));
	}

	timeout = 100;
	do {
		udelay(100);
		timeout--;
		fpga_cfg_status = cpld_read(FPGA_CONFIG_STATUS);
	}while (!(fpga_cfg_status & CFG_DONE) && timeout > 0);

	//check ten times
	for (i = 0; i < 10; i++) {
		fpga_cfg_status = cpld_read(FPGA_CONFIG_STATUS);
		if (!(fpga_cfg_status & CFG_DONE)) {
			break;
		}
	}

	if (fpga_cfg_status & CFG_DONE) {
		printf("program fpga done !\n");
	}else {	
		printf("error: wait fpga configuration done signal timeout!\n");
	}

	//unset chip select;
	cpld_write(FPGA_CONFIG_CTRL, PROGRAM_B);
}

static int fpgaload_cmd(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	char *end;
	uint8_t* data_ptr = 0;
	int len = 0;
	ulong value = 0;
	
	if (argc != 3) {
		return CMD_RET_USAGE;
	}
    
	value = simple_strtoul(argv[1], &end, 16);
	data_ptr = (uint8_t*)(value);

	value = simple_strtoul(argv[2], &end, 16);
	len = (int)value;

	program_fpga(data_ptr, len);

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	fpgaload, 3, 0, fpgaload_cmd,
	"program fpga",
	"<image address> <image length>"
);

