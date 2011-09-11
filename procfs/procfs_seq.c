/*
 * Linux procfs hacking example, this module demonstrate how to register 
 * procfs with sequence file operation.
 *
 * Copyright (C) 2011 - Ardhan Madras <ajhwb@knac.com>
 *
 * This program is free software under the GPL license.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static int remove_existing = 1;

module_param(remove_existing, bool, 0644);

#define procfs_name "cpuinfo"
#define PROCFS_OBSOLETE 1

static int cpu_mod_read_fs(struct seq_file *seq, void *offset)
{
	const int cpu_cores = 6;
	int id;

	for (id = 0; id < cpu_cores; id++) {
		seq_printf(seq, "processor\t: %i\n", id);
		seq_printf(seq, "vendor_id\t: GenuineIntel\n");
		seq_printf(seq, "cpu family\t: 6\n");
		seq_printf(seq, "model\t\t: 26\n");
		seq_printf(seq, "model name\t: Intel(R) Xeon(R) CPU           "
				"W3690  @ 3.40GHz\n");
		seq_printf(seq, "stepping\t: 5\n");
		seq_printf(seq, "cpu MHz\t\t: 1706.000\n");
		seq_printf(seq, "cache size\t: 12288 KB\n");
		seq_printf(seq, "physical id\t: 0\n");
		seq_printf(seq, "siblings\t: 4\n");
		seq_printf(seq, "core id\t\t: %i\n", id);
		seq_printf(seq, "cpu cores\t: 4\n");
		seq_printf(seq, "fdiv_bug\t: no\n");
		seq_printf(seq, "hlt_bug\t\t: no\n");
		seq_printf(seq, "f00f_bug\t: no\n");
		seq_printf(seq, "coma_bug\t: no\n");
		seq_printf(seq, "fpu\t\t: yes\n");
		seq_printf(seq, "fpu_exception\t: yes\n");
		seq_printf(seq, "cpuid level\t: 11\n");
		seq_printf(seq, "wp\t\t: yes\n");
		seq_printf(seq, "flags\t\t: fpu vme de pse tsc msr pae mce cx8 "
				"apic sep mtrr pge mca cmov pat pse36 clflush "
				"dts acpi mmx fxsr sse sse2 ss ht tm pbe nx "
				"rdtscp lm constant_tsc pni monitor ds_cpl vmx "
				"est tm2 cx16 xtpr popcnt lahf_lm\n");
		seq_printf(seq, "bogomips\t: 6403.60\n");

/*
 * Newer kernel has these informations, mine is 2.6.32.
 */

#if !PROCFS_OBSOLETE
		seq_printf(seq, "clflush size\t: 64\n");
		seq_printf(seq, "cache_alignment\t: 64\n");
		seq_printf(seq, "address sizes\t: 36 bits physical, 48 bits virtual\n");
		seq_printf(seq, "power management:\n");
#endif
		seq_printf(seq, "\n");
	}

	return 0;
}

static int cpu_mod_open_fs(struct inode *inode, struct file *file)
{
	return single_open(file, cpu_mod_read_fs, NULL);
}

static const struct file_operations cpu_mod_ops = {
	.owner = THIS_MODULE,
	.open = cpu_mod_open_fs,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release
};

/*
 * Old procfs API use create_proc_entry() newer API change it to proc_create().
 */
int init_cpuinfo(void)
{
	struct proc_dir_entry *entry;

	if (remove_existing)
		remove_proc_entry(procfs_name, NULL);
	printk(KERN_ALERT "remove_existing: %i\n", remove_existing);

#if PROCFS_OBSOLETE
	entry = create_proc_entry(procfs_name, 0644, NULL);
#else
	entry = proc_create(procfs_name, S_IFREG | S_IRUGO, NULL, &cpu_mod_ops);
#endif

	if (entry == NULL) {
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "%s: could not initialize proc entry\n", procfs_name);
		return -ENOMEM;
	}

#if PROCFS_OBSOLETE
	entry->proc_fops = &cpu_mod_ops;
#endif

	return 0;
}

void cleanup_cpuinfo(void)
{
	remove_proc_entry(procfs_name, NULL);
}

module_init(init_cpuinfo);
module_exit(cleanup_cpuinfo);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ardhan Madras <ajhwb@knac.com>");
MODULE_DESCRIPTION("procfs demo");
