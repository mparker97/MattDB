#include <unistd.h>
#include <string.h>
#include "common.h"

int tables_search_table(unsigned char* nm, unsigned char* buf){
	if (tables_f == NULL){
		fail_out(ENODB);
	}
	fseek(tables_f, 0, SEEK_SET);
	buf[TABLE_NAME_SZ] = 0;
	while (fgets(buf, TABLE_NAME_SZ + 1, table_f)){
		if (strcmp(buf, nm) == 0){
			return 1;
		}
	}
	return 0;
}

// Open and return (via pointer) metadata and table files for table 'nm'
int tables_get_table(unsigned char* nm, struct table_fds* t){
	unsigned char buf[TABLE_NAME_SZ + 7];
	int nm_len;
	
	if (!tables_search_table(nm, buf)){
		fail_out(ENOTAB);
	}
	nm_len = strlen(nm);
	
	strcpy(buf + nm_len, ".mdbtmd");
	if ((t->meta_fd = open_pf(db_path, nm)) < 0){
		fail_out(ENOTMD);
	}
	
	strcpy(buf + nm_len, ".mdbtab");
	if ((t->table_fd = open_pf(db_path, nm)) < 0){
		closec(t->meta_fd);
		fail_out(ENOTFIL);
	}
	return 1;
}

// Create and metadata and table files for new table 'nm'
int tables_create_table(unsigned char* nm){ // TODO: more args
	unsigned char buf[TABLE_NAME_SZ + 7];
	int nm_len;
	
	if (tables_search_table(nm, buf)){
		fail_out(EEXIST);
	}
	nm_len = strlen(nm);
	
	strcpy(buf + nm_len, ".mdbtmd");
	if ((t->meta_fd = creat_pf(db_path, nm)) < 0){
		fail_out(ECREAT);
	}
	
	strcpy(buf + nm_len, ".mdbtab");
	if ((t->table_fd = creat_pf(db_path, nm)) < 0){
		closec(t->meta_fd); // TODO: delete this file somehow
		fail_out(ECREAT);
	}
	return 1;
}
