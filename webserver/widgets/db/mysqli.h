/**
 * @file mysqli.h
 * @author LittleYang0531 (dev@lyoj.ml)
 * @brief MySQL/MariaDB连接器
 * @version 2.0.0
 * @date 2022-09-06
 * 
 * @copyright Copyright (c) 2022 LittleYang0531
 * 
 */

/**
 * @brief MySQL/MariaDB连接器
 * 
 */
class Mysqli {
    MYSQL mysqld;

    public:
    /**
     * @brief 初始化MySQL/MariaDB连接器
     * 
     */
    Mysqli() {
        if (mysql_init(&mysqld) == NULL) 
            writeLog(LOG_LEVEL_ERROR, "Failed to initialize MySQL/MariaDB");
        if (!mysql_real_connect(
                &mysqld, 
                mysqli_host.c_str(), 
                mysqli_user.c_str(), 
                mysqli_passwd.c_str(), 
                mysqli_db.c_str(), 
                mysqli_port, nullptr, 0)) 
            writeLog(LOG_LEVEL_ERROR, "Failed to connect to MySQL/MariaDB: " + string(mysql_error(&mysqld)));
    }

    /**
     * @brief MySQL/MariaDB查询语句
     * 
     * @param sql SQL语句
     * @return vector<argvar> 
     */
    vector<argvar> query(string sql) {
        vector<argvar> res; argvar tmp;
        MYSQL_RES* res2;
        if (mysql_query(&mysqld, sql.c_str())) 
            return writeLog(LOG_LEVEL_ERROR, "Failed to query MySQL/MariaDB: " + string(mysql_error(&mysqld))), res;
        if (!(res2 = mysql_store_result(&mysqld)))
            return writeLog(LOG_LEVEL_ERROR, "Failed to store MySQL/MariaDB: " + string(mysql_error(&mysqld))), res;
        vector<string> field; MYSQL_FIELD* fd; MYSQL_ROW row;
        for (int i = 0; fd = mysql_fetch_field(res2); i++) field.push_back(fd->name);
        while (row = mysql_fetch_row(res2)) {
            for (int i = 0; i < field.size(); i++) tmp[field[i]] = row[i] == NULL ? "" : row[i];
            res.push_back(tmp);
        } 
        mysql_free_result(res2);
        return res;
    }

    /**
     * @brief MySQL/MariaDB执行语句
     * 
     * @param sql SQL语句
     */
    void execute(string sql) {
        if (mysql_query(&mysqld, sql.c_str())) 
            writeLog(LOG_LEVEL_ERROR, "Failed to execute MySQL/MariaDB: " + string(mysql_error(&mysqld)));
    }
};