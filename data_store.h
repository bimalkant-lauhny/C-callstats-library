#include <sqlite3.h>
#include <stdio.h>

#define BUFFER_SIZE_SQLITE 10*1024

static sqlite3 *db = NULL;
static char *err_msg = NULL;

struct user_info {
  char *user_num;
  char *user_id;
  char *conf_num;
  char *conf_id;
  char *device_id;
  char *session_id;
  char *handle_id;
  char *audio_ssrc;
  char *video_ssrc;
  char *local_candidate;
  char *remote_candidate;
};

typedef struct user_info user_info;

void initialize_user_info(user_info *user) {
    user->user_num= NULL;
    user->user_id = NULL;
    user->conf_num= NULL;
    user->conf_id = NULL;
    user->device_id = NULL;
    user->session_id = NULL;
    user->handle_id = NULL;
    user->audio_ssrc = NULL;
    user->video_ssrc= NULL;
    user->local_candidate = NULL;
    user->remote_candidate = NULL;
}

void free_user_info(user_info *user) {
    free(user->user_num);
    free(user->user_id);
    free(user->conf_num);
    free(user->conf_id);
    free(user->device_id);
    free(user->session_id);
    free(user->handle_id);
    free(user->audio_ssrc);
    free(user->video_ssrc);
    free(user->local_candidate);
    free(user->remote_candidate);
}

size_t initialize_db() {
    if (db != NULL) {
        return 0;
    }
    size_t rc = sqlite3_open("test.db", &db); 
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
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
                                         "audio_ssrc TEXT,"
                                         "video_ssrc TEXT,"
                                         "local_candidate TEXT,"
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
    char *sql = "INSERT INTO Stats_Info VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');";
    char sql_buffer[BUFFER_SIZE_SQLITE];
    snprintf(sql_buffer, sizeof(sql_buffer), sql, 
            (user->user_num)?user->user_num:"NULL",
            (user->user_id)?user->user_id:"NULL",
            (user->conf_num)?user->conf_num:"NULL",
            (user->conf_id)?user->conf_id:"NULL",
            (user->device_id)?user->device_id:"NULL",
            (user->session_id)?user->session_id:"NULL",
            (user->handle_id)?user->handle_id:"NULL",
            (user->audio_ssrc)?user->audio_ssrc:"NULL",
            (user->video_ssrc)?user->video_ssrc:"NULL",
            (user->local_candidate)?user->local_candidate:"NULL",
            (user->remote_candidate)?user->remote_candidate:"NULL");
    printf("Buffer: %s\n", sql_buffer);
    size_t rc = sqlite3_exec(db, sql_buffer, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        return -1;
    } 
    return 1; 
}
    
size_t update_userinfo(user_info* user) {
    char *sql = "UPDATE Stats_Info SET user_num='%s', user_id='%s', conf_num='%s',"
                "conf_id='%s', device_id='%s', audio_ssrc='%s', video_ssrc='%s',"
                "local_candidate='%s', remote_candidate='%s' WHERE session_id='%s' AND handle_id='%s';";
    char sql_buffer[BUFFER_SIZE_SQLITE];
    snprintf(sql_buffer, sizeof(sql_buffer), sql, 
            (user->user_num)?user->user_num:"NULL",
            (user->user_id)?user->user_id:"NULL",
            (user->conf_num)?user->conf_num:"NULL",
            (user->conf_id)?user->conf_id:"NULL",
            (user->device_id)?user->device_id:"NULL",
            (user->audio_ssrc)?user->audio_ssrc:"NULL",
            (user->video_ssrc)?user->video_ssrc:"NULL",
            (user->local_candidate)?user->local_candidate:"NULL",
            (user->remote_candidate)?user->remote_candidate:"NULL");
    printf("Buffer: %s\n", sql_buffer);
    size_t rc = sqlite3_exec(db, sql_buffer, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        return -1;
    } 
    return 1; 
}

size_t add_user_num(char *session_id, char *handle_id, char *user_num) {
    char *sql = "UPDATE Stats_Info SET user_num='%s' "
        "WHERE session_id='%s' AND handle_id='%s';";
    char sql_buffer[BUFFER_SIZE_SQLITE];
    snprintf(sql_buffer, sizeof(sql_buffer), sql, 
            user_num, session_id, handle_id);
    printf("Buffer: %s\n", sql_buffer);
    size_t rc = sqlite3_exec(db, sql_buffer, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        return -1;
    } 
    return 1; 
}

size_t get_user_info(char *session_id, char *handle_id, user_info *user) {
    char *sql = "SELECT * FROM Stats_Info WHERE session_id='%s' AND handle_id='%s';";
    char sql_buffer[BUFFER_SIZE_SQLITE];
    snprintf(sql_buffer, sizeof(sql_buffer), sql, 
            session_id, handle_id);
    printf("Buffer: %s\n", sql_buffer);
    sqlite3_stmt *stmt;
    size_t rc = sqlite3_prepare_v2(db, sql_buffer, -1, &stmt, NULL);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to prepare SQL statement.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return -1;
    }  
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *res = sqlite3_column_text(stmt, 0);
        user->user_num = malloc(strlen(res) + 1);
        strcpy(user->user_num, res);
        
        res = sqlite3_column_text(stmt, 1);
        user->user_id = malloc(strlen(res) + 1);
        strcpy(user->user_id, res);
        
        res = sqlite3_column_text(stmt, 2);
        user->conf_num= malloc(strlen(res) + 1);
        strcpy(user->conf_num, res);
        
        res = sqlite3_column_text(stmt, 3);
        user->conf_id = malloc(strlen(res) + 1);
        strcpy(user->conf_id, res);
        
        res = sqlite3_column_text(stmt, 4);
        user->device_id = malloc(strlen(res) + 1);
        strcpy(user->device_id, res);
        
        res = sqlite3_column_text(stmt, 5);
        user->session_id = malloc(strlen(res) + 1);
        strcpy(user->session_id, res);
            
        
        res = sqlite3_column_text(stmt, 6);
        user->handle_id = malloc(strlen(res) + 1);
        strcpy(user->handle_id, res);
        
        res = sqlite3_column_text(stmt, 7);
        user->audio_ssrc = malloc(strlen(res) + 1);
        strcpy(user->audio_ssrc, res);
        
        res = sqlite3_column_text(stmt, 8);
        user->video_ssrc = malloc(strlen(res) + 1);
        strcpy(user->video_ssrc, res);
        
        res = sqlite3_column_text(stmt, 9);
        user->local_candidate = malloc(strlen(res) + 1);
        strcpy(user->local_candidate, res);
        
        res = sqlite3_column_text(stmt, 10);
        user->remote_candidate = malloc(strlen(res) + 1);
        strcpy(user->remote_candidate, res);
    }
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to prepare SQL statement.\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return -1;
    }
    
    sqlite3_finalize(stmt);
    
    return 0;
}

size_t close_db() {
    sqlite3_free(err_msg);
    size_t rc = sqlite3_close(db);
    return 0;
}
