/***********************************************
 * module : sys_reg
 *
 * Wang Long (w@laoqinren.net)
 **********************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/pgtable.h>

#define SYS_REG_FILE "sys_reg"

struct gdtr_struct {
	short limit;
	unsigned long address __attribute__((packed));
};

static int reg_proc_show(struct seq_file *m, void *v)
{
	unsigned long cr0, cr3, cr4;
	struct mm_struct *mm;
	struct gdtr_struct gdtr;

	mm = current->active_mm;
	cr0 = read_cr0();
	cr3 = read_cr3();
	cr4 = read_cr4();
	asm("sgdt %0":"=m"(gdtr));
	
	seq_printf(m, "cr4=0x%08lX, ", cr4);
	seq_printf(m, "PSE=0x%08lX, ", (cr4>>4)&1);
	seq_printf(m, "PAE=0x%08lX\n", (cr4>>5)&1);
	seq_printf(m, "cr3=0x%08lX, cr0=0x%08lX\n", cr3, cr4);
	seq_printf(m, "PGD:0x%08lX\n", (unsigned long)mm->pgd);
	seq_printf(m, "gdtr address = 0x%08lX, limit = 0x%08X\n", gdtr.address, gdtr.limit);
	return 0;
}

static int reg_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, reg_proc_show, NULL);
}

static const struct file_operations reg_proc_fops = {
	.open		= reg_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init sys_reg_init(void)
{
	struct proc_dir_entry *pe;

	printk(KERN_ALERT "[Hello] sys_reg \n");
	pe = proc_create(SYS_REG_FILE, 0644, NULL, &reg_proc_fops);
	return 0;
}

static void __exit sys_reg_exit(void)
{
	printk(KERN_ALERT "[Goodbye] sys_reg\n");
	remove_proc_entry(SYS_REG_FILE, NULL);
}

module_init(sys_reg_init);
module_exit(sys_reg_exit);
MODULE_LICENSE("GPL");
