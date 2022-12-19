#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>             	
#include <linux/uaccess.h>          	
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/device.h>
#include <linux/memblock.h>
#include <linux/types.h>

#define PROCFS_MAX_SIZE 1000
#define PROCFS_NAME "example"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("lab2 osi");
MODULE_VERSION("1.0");

static unsigned long procfs_buffer_size = 0;
static struct proc_dir_entry *our_proc_file;

static int64_t pid = 1;
static int64_t struct_id = 1;


static int  	__init procfs2_init(void);
static void 	__exit procfs2_exit(void);


static int  	procfile_open(struct inode *inode, struct file *file);
static int  	procfile_release(struct inode *inode, struct file *file);
static ssize_t procfile_read(struct file *filePointer, char __user *buffer, size_t buffer_length, loff_t *offset);
static ssize_t procfile_write(struct file *file, const char __user *buff,size_t count, loff_t *off);

static struct page *get_my_page(struct mm_struct* mm,
                unsigned long address) {
    pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	
 
    pgd = pgd_offset(mm, address);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        return NULL;
    }
 
   	p4d = p4d_offset(pgd, address);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
        return NULL;
    }
 
    pud = pud_offset(p4d, address);
    if (pud_none(*pud) || pud_bad(*pud)) {
        return NULL;
    }
 
   	pmd = pmd_offset(pud, address);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        return NULL;
    }
 
    pte = pte_offset_kernel(pmd, address);
    if (!pte) {
        return NULL;
    }
 
    return pte_page(*pte);
     
 
}
 
 
static size_t write_page_struct(char __user *ubuf,
                struct task_struct *task_struct_ref) {
    char buf[PROCFS_MAX_SIZE];
    size_t len = 0;

    struct page *page_struct;
    struct mm_struct *mm = task_struct_ref->mm;
	struct vm_area_struct *vm_current;

	uint64_t start;
	uint64_t end; 
 
 
    if (mm == NULL) {
        sprintf(buf, "Task_struct's mm is NULL\n");
        return 0;
    }

   	vm_current = mm->mmap;
    start = vm_current->vm_start;
    end = vm_current->vm_end;
    while (start <= end) {
        page_struct = get_my_page(mm, start);
        if (page_struct != NULL) {
            len += sprintf(buf + len, "flags = %ld\n", page_struct->flags);
            len += sprintf(buf + len, "start_address = 0x%llx\n", start);
            len += sprintf(buf + len, "pa= 0x%llx\n", page_struct->mapping);
            break;
        }
        start += PAGE_SIZE;
    }
    if (copy_to_user(ubuf, buf, len)) {
        return -EFAULT;
    }
    return len;
 
}
 
 
static size_t write_thread_struct(char __user *ubuf,
                struct task_struct *task_struct_ref) {
    char buf[PROCFS_MAX_SIZE];
    size_t len = 0;
    struct thread_struct running_thread;
	running_thread = task_struct_ref -> thread;

    len += sprintf(buf + len, "sp = 0x%lx\n", running_thread.sp);
	len += sprintf(buf + len, "fsbase = 0x%lx\n", running_thread.fsbase);
	len += sprintf(buf + len, "gsbase = %lx\n", running_thread.gsbase);
        if (copy_to_user(ubuf, buf, len)) {
        return -EFAULT;
    }
    return len;
}

static int procfile_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file opend.....\t");
    return 0;
}
  
//zakritie
static int procfile_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file released.....\n");
    return 0;
}
  

static ssize_t procfile_read(struct file *filePointer, char __user *buffer,
			     size_t buffer_length, loff_t *offset)
{

  
   	char buf[PROCFS_MAX_SIZE];
   	int64_t len = 0;

	struct task_struct *running_task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
      
	printk(KERN_INFO "proc file read.....\n");
	if (*offset > 0 || buffer_length < PROCFS_MAX_SIZE){
		return 0;
	}

	if (running_task == NULL){
		len += sprintf(buf,"task_struct for pid %lld is NULL.\n",pid);

		if (copy_to_user(buffer, buf, len)){
			return -EFAULT;
		}

		*offset = len;
		return len;
	}

	switch(struct_id){
		default:
		case 0:
	    		len = write_page_struct(buffer, running_task);
	    		break;
		case 1:
		    	len = write_thread_struct(buffer, running_task);
		    	break;
	}

	*offset = len;
	return len;
}



static ssize_t procfile_write(struct file *file, const char __user *buff,
                              size_t count, loff_t *off)
{
	int64_t num_of_args, a, b;
	char procfs_buffer[PROCFS_MAX_SIZE];

	printk(KERN_INFO "proc file wrote.....\n");

	if (count > PROCFS_MAX_SIZE)
		return -EFAULT;
	if (copy_from_user(procfs_buffer, buff, count))
		return -EFAULT;

	*off += procfs_buffer_size;
	pr_info("procfile write %s\n", procfs_buffer);

	
	num_of_args = sscanf(procfs_buffer, "%lld %lld", &a, &b);
	if (num_of_args != 2){
		return -EFAULT;
	}
	
	struct_id = a;
	pid = b;
	*off = strlen(procfs_buffer);

	return strlen(procfs_buffer);
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
	.proc_open = procfile_open, 
	.proc_read = procfile_read,
	.proc_write = procfile_write,
        .proc_release = procfile_release
};
#else
static const struct file_operations proc_file_fops = {
	
	.open = procfile_open, 
	.read = procfile_read,
	.write = procfile_write,
        .release = procfile_release

};
#endif

static int __init procfs2_init(void)
 {
	our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
	if (NULL == our_proc_file) {
		proc_remove(our_proc_file);
		pr_alert("Error:Could not initialize /proc/%s\n", PROCFS_NAME);
		return -ENOMEM;
	}
	pr_info("/proc/%s created\n", PROCFS_NAME);
	return 0;
}

static void __exit procfs2_exit(void)
{
	proc_remove(our_proc_file);
	pr_info("/proc/%s removed\n", PROCFS_NAME);
}

module_init(procfs2_init);
module_exit(procfs2_exit);
