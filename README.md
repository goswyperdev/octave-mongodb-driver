# mex mongodb client
MongoDB driver to be used with octavia for plotting etc.

Developed by our interns Tamiz and Pradeep from CBIT,Hyd

Derived from: https://github.com/markuman/mex-mariadb

Test condition
  * Linux + Octave + MongoDB

# to build mex file

```
$ make
 mkoctfile --mex  -L/usr/lib  -lpthread -lz -lm -ldl -lssl -lcrypto -lmongoc-1.0 -lbson-1.0  mongodb.c -I/usr/local/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0

```


# usage in octave

```
octave:1> cpu_idle = mongodb("timestamp", "$gte", "1529997800", "cpu_idle_perc", 32)

hostname = 192.168.1.136
port =  27017
username = root
password = access
database = cpu_stat
output = cell
collection = cpu_util
Hostname: 192.168.1.136
Port: 27017
LIMIT: 32
Username: root
Password: access
Collection: cpu_util
Database: cpu_stat
Filter Field: timestamp
Condition: $gte
Filter Value: 1529997800
Output Field: cpu_idle_perc
cpu_idle =

   109.842
   109.842
   109.842
   109.842
   109.842
   109.842
   109.842
    94.150
    94.150
   109.842
   109.842
   109.842
   109.842
   109.842
   109.842
   109.842
    94.150
    94.150
   109.842
   109.842
   109.842
   109.842
   109.842
   109.842
   109.842
    94.150
    94.150
   109.842
   109.842
   109.842
   109.842
   109.842


octave:2> graphics_toolkit("gnuplot")
octave:3> plot(cpu_idle)
```
```
## return structure

the return structure is always a cell.

octave:1> cpu_idle = mongodb("timestamp", "$gte", "1529997800", "cpu_idle_perc", 32)
cpu_idle
 = [1,1] = 94.150000
   [2,1] = 109.841667
   [3,1] = 109.841667
   [4,1] = 109.841667
   [5,1] = 109.841667
   [6,1] = 109.841667

```
```
# mongodb()

## properties

* `hostname`
  * char, default `192.168.1.136`
* `port`
  * number, default `27017`
* `username`
  * char, default `root`
* `password`
  * char, default `password`
* `database`
  * char, default `cpu_stat`. Connects to database.
* `collection`
  * char, default `cpu_init`
* `filter_field`
  * char, passed by the user (collection field used for filtering)
* `condition`
  *char, passed by user value($gte, $gt, $lt, $lte, $eq, $ne)
* `filter_value`
  * char, passed by the user (collection field value to filter for, hardcoded to long value)
* `output_field`:
  * char, passed by the user (collection field required as output)
* `limit`:
  * number, passed by the user (number of samples from collection)
```

```
## methods

* mongodb(filter_field, condition, filter_value, output_field, limit)`
  * executes the mongo command
* `to_mat()`
  * only used internally to convert the cell output to matrix format.

```
