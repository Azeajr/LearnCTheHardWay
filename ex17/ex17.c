#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 100

struct Address{
    int id;
    int set;
    char name[MAX_DATA];
    char email[MAX_DATA];
    //char *name;
    //char *email;
};

struct Database{
    struct Address rows[MAX_ROWS];
    //struct Address **rows;
    //int row_size;
    //int data_size;
};

struct Connection {
    FILE *file;
    struct Database *db;
};

void Database_close(struct Connection *conn);

void die(struct Connection *conn, const char *message){
    if(errno){
        perror(message);
        /**
         * @brief Close and clean up conn object
         * 
         */
        Database_close(conn);
    } else {
        printf("ERROR: %s\n", message);
        /**
         * @brief Close and clean up conn object
         * 
         */
        Database_close(conn);
    }

    exit(1);
}

void Address_print(struct Address *addr){
    printf("%d %s %s\n",addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn){
    int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
    if(rc != 1){
        die(conn, "Failed to load database.");
    }
}

struct Connection *Database_open(const char *filename, char mode, int row_size, int data_size){
    struct Connection *conn = malloc(sizeof(struct Connection));
    if(!conn){
        die(conn, "Memory error");
    }

    conn->db = malloc(sizeof(struct Database));
    if (!conn->db){
        die(conn, "Memory error");
    }

    //conn->db->rows = (struct Address **)malloc(sizeof(struct Address) * row_size);
    //if (!conn->db->rows){
    //    die(conn, "Memory error");
    //}
    //conn->db->data_size = data_size;
    //conn->db->row_size = row_size;


    if(mode == 'c'){
        conn->file = fopen(filename, "w");
    } else {
        conn->file = fopen(filename, "r+");

        if(conn->file){
            Database_load(conn);
        }
    }

    if(!conn->file){
        die(conn, "Failed to open file");
    }

    return conn;
}

void Database_close(struct Connection *conn){
    if(conn){
        if(conn->file){
            fclose(conn->file);
        }
        if(conn->db){
            free(conn->db);
        }
        free(conn);
    }
}

void Database_write(struct Connection *conn){
    rewind(conn->file);

    int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
    if(rc != 1){
        die(conn, "Failed to write database.");
    }

    rc = fflush(conn->file);
    if(rc == -1){
        die(conn, "Cannot flush database.");
    }
}

void Database_create(struct Connection *conn){
    int i = 0;
    for(i = 0; i < MAX_ROWS; i++){
        struct Address addr = {.id = i, .set = 0};
        //addr.name = malloc(sizeof(char) * conn->db->data_size);
        //addr.email = malloc(sizeof(char) * conn->db->data_size);
        conn->db->rows[i] = addr;
    }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email){
    struct Address *addr = &conn->db->rows[id];
    if(addr->set){
        die(conn, "Already set, delete it first");
    }

    addr->set = 1;

    char *res = strncpy(addr->name, name, MAX_DATA);
    if(!res){
        die(conn, "Name copy failed");
    }
    /**
     * @brief Make sure that addr->name conforms to C strings.
     * 
     */
    addr->name[MAX_DATA-1] = '\0';
    res = strncpy(addr->email, email, MAX_DATA);
    if(!res){
        die(conn, "Email copy failed");
    }
    /**
     * @brief Make sure that addr->email conforms to C strings.
     * 
     */
    addr->email[MAX_DATA-1] = '\0';
}

void Database_get(struct Connection *conn, int id){
    struct Address *addr = &conn->db->rows[id];

    if(addr->set){
        Address_print(addr);
    } else {
        die(conn, "ID is not set");
    }
}


void Database_delete(struct Connection *conn, int id){
    struct Address addr = {.id =id, .set = 0};
    conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn){
    int i = 0;
    struct Database *db = conn->db;

    for(i = 0; i < MAX_ROWS; i++){
        struct Address *cur = &db->rows[i];

        if(cur->set){
            Address_print(cur);
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 3){
        /**
         * @brief NULL needs to be passed here since the conn pointer has not 
         * been setup yet.
         */
        die(NULL, "USAGE: ex17 <dbfile> <action> [action params]");
    }

    char *filename = argv[1];
    char action = argv[2][0];
    struct Connection *conn = Database_open(filename, action, MAX_ROWS, MAX_DATA);
    int id = 0;

    if(argc > 3){
        id = atoi(argv[3]);
    }
    if(id >= MAX_ROWS){
        die(conn, "There are not that many records");
    }

    switch(action){
        case 'c':
            Database_create(conn);
            Database_write(conn);
            break;

        case 'g':
            if(argc != 4){
                die(conn, "Need an id to get");
            }
            Database_get(conn, id);
            break;

        case 's':
            if(argc != 6){
                die(conn, "Need id, name, email to set");
            }
            Database_set(conn, id, argv[4], argv[5]);
            Database_write(conn);
            break;

        case 'd':
            if(argc != 4){
                die(conn, "Need id to delete");
            }
            Database_delete(conn, id);
            Database_write(conn);
            break;

        case 'l':
            Database_list(conn);
            break;

        default:
            die(conn, "Invalid action, only: c=create, g=get, s=set, d=del, l=list");
    }

    Database_close(conn);

    return 0;
}




