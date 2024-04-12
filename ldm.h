#ifndef LDM_H
#define LDM_H



typedef struct _attr_map 
{
	int *object;
	struct attribute attr;
}attr_map; // end struct _attr_map

ssize_t ldm_show(struct kobject *kobj, struct attribute *attr, char *buffer);
ssize_t ldm_store(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size);
void ldm_release( struct kobject *kobj);


#endif // LDM_H
