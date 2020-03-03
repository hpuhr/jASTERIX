import mysql.connector


class MySQLWrapper:
    def __init__(self, host, user, passwd, database):

        self._mydb = mysql.connector.connect(
            host=host,
            user=user,
            passwd=passwd,
            database=database
        )

        self._mycursor = self._mydb.cursor(dictionary=True)

        #print ('sd_track columns')
        #self._mycursor.execute("SHOW COLUMNS FROM sd_track")
        #for x in self._mycursor:
        #    print(x)

        self._mycursor.execute("SELECT count(*) FROM sd_track")

        self.__num_db_records = self._mycursor.fetchone()["count(*)"]

        #self._mycursor.execute("SELECT count(*) FROM sd_track WHERE detection_type=1 AND multiple_sources=\"N\"")
        #print ('tmp {}'.format(self._mycursor.fetchone()[0]))

        #self.print_distinct("report_type", "sd_track")
        self.print_distinct("detection_type", "sd_track")
        #self.print_distinct("multiple_sources", "sd_track")

    def prepare_read (self, variables_str, table_str):
        #rec_num, tod, track_num, detection_type
        self._mycursor.execute("SELECT {} FROM {}".format(variables_str, table_str)) #  WHERE track_num=4227 LIMIT 0,10000

    def fetch_one (self):
        return self._mycursor.fetchone()

    def print_distinct(self, variable, table):

        self._mycursor.execute("SELECT {},count(*) from {} GROUP BY {}".format(variable, table, variable))

        print('table {} variable {}, count:'.format(table, variable))
        for x in self._mycursor:
            print('{}: {}'.format(x[variable], x["count(*)"]))
