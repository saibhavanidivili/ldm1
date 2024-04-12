#include <linux/init.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/slab.h>
#include "ldm.h"

static struct kset *my_kset;
static struct kobj_type *my_ktype;
static struct kobject *root;
static struct kobject *left;
static struct kobject *right;

static struct sysfs_ops ops =
{
	.show = &ldm_show,
	.store = &ldm_store
}; // end ops

static int f1 = 10;
static int f2 = 20;
//const mode_t mode = 0664;

static attr_map map[] =
{
	{&f1,{.name = "f1", .mode = 0666}},
	{&f2,{.name = "f2", .mode = 0666}},
	{NULL, {.name = 0, .mode = 0}}
}; // end map[]

ssize_t ldm_show(struct kobject *kobj, struct attribute *attr, char *buffer)
{
	int *object;
	int i = 0;
	for (i = 0; map[i].attr.name; i++)
	{
		if (strcmp(attr->name, map[i].attr.name) == 0)
		{
			object = map[i].object;
			return sprintf(buffer, "%d\n", *object);
		} // end if
	} // end for
	
	// error
	return -1;
} // end ldm_show()


ssize_t ldm_store(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size)
{
	int object;
	kstrtoint(buffer, 10, &object);
	
	int i = 0;
	for (i = 0; map[i].attr.name; i++)
	{
		if (strcmp(attr->name, map[i].attr.name) == 0)
		{
			*map[i].object = object;
			return size;
			
		} // end if
	} // end for		
	
	// error 
	return -1;
} // end ldm_store()


void ldm_release( struct kobject *kobj)
{
	printk(KERN_INFO "Release function...\n");
} // end ldm_release()
static int __init ldm_init(void)
{
	// set up the kobject type
	my_ktype = kmalloc(sizeof(struct kobj_type), GFP_KERNEL);
	memset(my_ktype, 0, sizeof(*my_ktype));
	my_ktype->release = &ldm_release;
	my_ktype->sysfs_ops = &ops;
	my_ktype->default_attrs = NULL;
	
	// set up the kset
	my_kset = kset_create_and_add("T", NULL, kernel_kobj);
	
	// set up the root kobject
	root = kmalloc(sizeof(struct kobject), GFP_KERNEL);
	memset(root, 0, sizeof(*root));
	root->kset = my_kset;
	kobject_init_and_add(root, my_ktype, NULL, "t");
	kobject_uevent(root, KOBJ_ADD);
	
	
	// set up the left sub-kobject
	left = kmalloc(sizeof(struct kobject), GFP_KERNEL);
	memset(left, 0, sizeof(*left));
	left->kset = my_kset;
	kobject_init_and_add(left, my_ktype, root, "l");
	kobject_uevent(left, KOBJ_ADD);
	
	// set up the right sub-kobject
	right = kmalloc(sizeof(struct kobject), GFP_KERNEL);
	memset(right, 0, sizeof(*right));
	right->kset = my_kset;
	kobject_init_and_add(right, my_ktype, root, "r");
	kobject_uevent(right, KOBJ_ADD);
	
	
	// add attributes to the left kobject
	int i;	
	for (i = 0; map[i].object; i++)
	{
		sysfs_create_file(left, &map[i].attr);
	} // end for
	printk(KERN_INFO "Done initializing.\n");
	return 0;
} // end ldm_init()

static void __exit ldm_exit(void)
{

	// remove attributes from the left sub-kobject
	int i;
	for (i = 0; map[i].object; i++)
	{
		sysfs_remove_file(left, &map[i].attr);
	} // end for
	
	// delete the kobjects themselves
	kobject_put(right);
	kobject_put(left);
	kobject_put(root);
	kset_unregister(my_kset);
	printk(KERN_INFO "Done exiting.\n");
} // end ldm_exit()

module_init(ldm_init);
module_exit(ldm_exit);

MODULE_DESCRIPTION("ldm");
MODULE_AUTHOR("Joe Nathan Abellard");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");
