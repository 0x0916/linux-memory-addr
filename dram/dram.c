/***********************************************
 * module : dram
 *
 * Wang Long (w@laoqinren.net)
 **********************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/highmem.h>

#define DRAM_MAJOR			85
static unsigned long dram_size = 0;

static ssize_t
dramdev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct page *pp;
	int page_number, page_indent;
	void *from;
	int more;

	// we cannot read byyond the end-of-file
	if (*f_pos >= dram_size)
		return 0;

	// determine which physical page to temporarily map
	// and how far into that page to begin readding from
	page_number = *f_pos / PAGE_SIZE;
	page_indent = *f_pos % PAGE_SIZE;

	// get the page from pfn
	pp = pfn_to_page(page_number);

	from = kmap(pp) + page_indent;

	// can not read beyond the end of this mapped page
	if (page_indent + count > PAGE_SIZE)
		count = PAGE_SIZE - page_indent;

	// now transfer count bytes from mapped page to user-supplied buffer
	more = copy_to_user(buf, from, count);

	// ok now to discard the temporary page mapping
	kunmap(pp);

	// an error occurred if less than count bytes got copied
	if (more)
		return -EFAULT;
	// otherwise advance file-pointer and report number of bytes read
	*f_pos += count;
	return count;
}

loff_t dramdev_llseek(struct file *file, loff_t offset, int whence)
{
	unsigned long newpos = -1;

	switch(whence) {
	case SEEK_SET: newpos = offset; break;
	case SEEK_CUR: newpos = file->f_pos + offset; break;
	case SEEK_END: newpos = dram_size + offset; break;
	}

	if ((newpos < 0) || (newpos > dram_size))
		return -EINVAL;

	file->f_pos = newpos;

	return newpos;
}

static const struct file_operations dramdev_fops = {
	.owner =	THIS_MODULE,
	.read =		dramdev_read,
//	.open =		dramdev_open,
	.llseek =	dramdev_llseek,
};

static int __init dram_init(void)
{
	int status;

	printk(KERN_ALERT "[Hello] dram \n");
	dram_size = (unsigned long )get_num_physpages() << PAGE_SHIFT;
	printk(KERN_ALERT "dram major = %d\n", DRAM_MAJOR);
	printk(KERN_ALERT "ramtop = %08lX(%lu MB)\n", dram_size, dram_size >> 20);

	status = register_chrdev(DRAM_MAJOR, "dram", &dramdev_fops);
	if (status < 0)
		return status;
	return 0;
}

static void __exit dram_exit(void)
{
	printk(KERN_ALERT "[Goodbye] dram\n");
	unregister_chrdev(DRAM_MAJOR, "dram");
}

module_init(dram_init);
module_exit(dram_exit);
MODULE_LICENSE("GPL");
