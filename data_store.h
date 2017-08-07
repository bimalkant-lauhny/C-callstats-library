#include <sqlite3.h>

static sqlite3 *db = NULL;
static char *err_msg = NULL;

struct user_info {
  char *user_id;
  char *user_num;
  char *conf_id;
  char *conf_num;
  char *device_id;
  char *session_id;
  char *handle_id;
  char *audio_ssrc;
  char *video_ssrc;
  char *local_candidate;
  char *remote_candidate;
};

typedef struct user_info user_info;

size_t initialize_db() {
    size_t rc = sqlite3_open("test.db", &db); 
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    
    char *sql = "DROP TABLE IF EXISTS Stats_Info;"
                "CREATE TABLE Stats_Info(user_num TEXT," 
                                         "user_id TEXT," 
                                         "conf_num TEXT," 
                                         "conf_id TEXT,"
                                         "device_id TEXT,"
                                         "session_id TEXT,"
                                         "handle_id TEXT,"
                                         "audio_ssrc TEXT"
                                         "video_ssrc TEXT"
                                         "local_candidate TEXT"
                                         "remote_candidate TEXT);";
                                         
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        return -1;
    } 
    return 0;
}

size_t insert_userinfo(user_info *user) {
 return 1; 
}

size_t close_db() {
  size_t rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot close database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    return 0
}
