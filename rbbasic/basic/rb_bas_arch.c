
#include "rb_bas_arch.h"
#include "rb_bas_platform.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

int arch_init(void) {
	printf("Document path: %s\n", platform_get_project_path());
    
    return 0;
}

static char* _get_path(void) {
    return (char*)platform_get_project_path();
}

static char* _get_temp_path(void) {
    return (char*)platform_get_temp_path();
}

int arch_load(char* name, char* path, arch_load_out_cb cb, void* context) {
    char* filename;
    asprintf(&filename, "%s/%s", path, name);
    
    FILE* fp = fopen(filename, "r");
    if (!fp){
        return 1;
    }
    
    char line[256];
    while(fgets(line, 256, fp) != NULL) {
        cb(line, context);
    }
    
    fclose(fp);
    free(filename);
    
    return 0;
}

int arch_save(char* name, arch_save_cb cb, void* context) {
    char* line;
    char* filename;
    asprintf(&filename, "%s/%s.BAS", _get_path(), name);

    FILE* fp = fopen(filename, "w");
    if (!fp){
        return 1;
    }

    for (;;) {
        uint16_t number = cb(&line, context);
    
        if (line == NULL) {
            break;
        }
        
        fprintf(fp, "%d %s\n",number, line);
    }

    fclose(fp);
	free(filename);
	
    return 0;
}

int arch_dir(char *ext, arch_dir_out_cb cb, void* context) {
    char out[256];
    struct stat stats;
    struct dirent *ent;
    
    DIR *dir;
    dir = opendir(_get_path());
    
    while ((ent = readdir(dir)) != NULL) {
        char* name = ent->d_name;
        
        if (strlen(name)>4){
            char *extension = name + strlen(name) - strlen(ext);
            
            if (strncmp(extension, ext, strlen(ext)) == 0) {
                snprintf(out,sizeof(out),"%s/%s%s", _get_path(), name, ext);
                stat(out, &stats);
                name[strlen(name)-strlen(ext)] = '\0';
                cb(name, ext, stats.st_size, false, context);
            }
        }
    }
    
    closedir(dir);

    return 0;
}

int arch_delete(char* name) {
    char* filename;
    asprintf(&filename, "%s/%s.BAS", _get_path(), name);
    remove(filename);
    free(filename);
    
    return 0;
}


