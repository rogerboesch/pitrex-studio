
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "rb_bas_dictionary.h"

typedef struct dicentry dicentry;
struct dicentry {
    dicentry* next;
    char* name;
    void* value;
};

#define HASHSIZE 13

struct dictionary {
    dicentry *hashtab[HASHSIZE];
};

static unsigned int hash(char *name) {
    unsigned int hashval;
    
    for (hashval = 0; *name != '\0'; name++) {
        hashval = *name + 31 * hashval;
    }
    
    return hashval % HASHSIZE;
}

static dicentry* _get(dictionary* d, char *name) {
    dicentry* entry;
    for (entry = d->hashtab[hash(name)]; entry != NULL; entry = entry->next) {
        if (strcmp(name, entry->name) == 0) {
            return entry;
        }
    }

    return NULL;
}

void* dictionary_get(dictionary* d, char* name) {
    dicentry* entry = _get(d, name);

    if (entry) {
        return entry->value;
    }

    return NULL;
}

bool dictionary_has(dictionary* d, char *name) {
    dicentry* entry = _get(d, name);

    if (entry) {
        return true;
    }

    return false;
}

void dictionary_put(dictionary* d, char* name, void* value) {
    dicentry* element;
    unsigned int hashval;

    element = _get(d, name);

    if (element == NULL) {
        element = (dicentry*) malloc(sizeof(*element));
        
        if (element == NULL || (element->name = strdup(name)) == NULL) {
            return;
        }
        
        hashval = hash(name);
        element->next = d->hashtab[hashval];
        d->hashtab[hashval] = element;
    }
    
    element->value = value;
}

void* dictionary_del(dictionary* d, char* name) {
    dicentry* root = d->hashtab[hash(name)];

    if (root == NULL) {
        return NULL;
    }

    if (strcmp(name, root->name) == 0) {
        d->hashtab[hash(name)] = root->next;
        void *value = root->value;
        free(root->name);
        free(root);
        
        return value;
    }

    dicentry* element = root;
    while(element->next) {
        dicentry *next = element->next;

        if (strcmp(name, next->name) == 0) {
            element->next = next->next;
            void *value = next->value;
            free(next->name);
            free(next);
            
            return value;
        }
    }

    return NULL;
}

void dictionary_each(dictionary* d, dictionary_each_cb cb, void* context) {
    dicentry* next_entry = NULL;

    if (!cb) {
        return;
    }

    for (size_t i=0; i < HASHSIZE; i++) {
        dicentry* entry = d->hashtab[i];
      
        while(entry) {
            next_entry = entry->next;
            cb(entry->name, entry->value, context);
            entry = next_entry;
        }
    }
}

dictionary* dictionary_new() {
    dictionary* d = malloc(sizeof(dictionary));
    
    if (d == NULL) {
        return NULL;
    }
    
    for (size_t i=0; i < HASHSIZE; i++) {
        d->hashtab[i] = NULL;
    }
    
    return d;
}

typedef struct {
    dictionary* d;
    dictionary_each_cb cb;
} _free_s;

static void destroy_cb_pass_1(char* name, void* value, void* context) {
    _free_s *ctx = (_free_s*)context;
    dictionary_each_cb free_cb = ctx->cb;
    
    free_cb(name, value, NULL);
}

static void destroy_cb_pass_2(char* name, void* value, void* context) {
    _free_s *ctx = (_free_s*) context;
    dictionary *d = ctx->d;
    
    dictionary_del(d, name);
}

void dictionary_destroy(dictionary* d, dictionary_each_cb free_cb) {
    _free_s ctx = {.d = d, .cb = free_cb};

    dictionary_each(d, destroy_cb_pass_1, &ctx);
    dictionary_each(d, destroy_cb_pass_2, &ctx);
    
    free(d);
}
