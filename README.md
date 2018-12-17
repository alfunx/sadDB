# sadDB

A super awesome distributed database system, that implements the distributed
track-join.


## Build

To prepare the build environment, run the following commands once:

```bash
cd build
cmake ..
```

To build the project, run:

```bash
cd build
make
```


## Usage

First start at least one server. Note that the server will use up to 10 ports,
in sequence starting from the specified port.

```bash
cd build
# ./sadDB [port]
./sadDB 14010 &
./sadDB 14020 &
```

After starting the server(s), start the client to launch the track-join. The
first parameter `<database>` refers to a setup of relations and nodes, and must be
the name of a directory in `relations/`, e.g. `proof`. `<address>` must be in
the form `ip:port` and must be specified for every server that should
participate in the join.

```bash
cd build
# ./sadDB_client <database> <address> [<address>...]
./sadDB_client proof localhost:14010 localhost:14020
```

The server outputs the result relation on `stdout` using the CSV format, and
some debugging messages on `stderr`.


### Database

The databases are stored in the `relations/` directory. The directory structure
is expected to be as follows:

```
relations
└── proof
    ├── 1
    │   ├── R.txt
    │   └── S.txt
    └── 2
        ├── R.txt
        └── S.txt
```

As an example, consider the database named `proof`, which consists of two
relations `R` and `S` which are horizontally fragmented and stored on two nodes.
Note that the relations must be named `R` and `S`.


### Generate Databases

Databases can be generated using the `sadDB_gen` program.

```bash
cd build
# ./sadDB_gen [-i <input-file>] [-o <output-file>] \
#     [-p <payload-size>] [-t <number-of-tuples>] [-r <min> <max>]
./sadDB_gen -o R.txt -p 100 -t 2000 -r 1 50
```

The database generator either takes an input file in the following format:

```
1
value for first key
2
value for second key
```

Or it generates the specified number of tuples with keys chosen uniformly at
random in the given range. As payload, it will use a string of the specified
length. The resulting relation will be stored as serialized relation, that can
be used by the sadDB server.


### Concrete Example

To test the track-join, use the `proof` dataset.

```bash
cd build
./sadDB 14010 \
    1>node_1-rel.csv \
    2>node_1-out.txt &
./sadDB 14020 \
    1>node_2-rel.csv \
    2>node_2-out.txt &
./sadDB_client proof \
    localhost:14010 \
    localhost:14020
```

The results will be stored in the files `node_1-rel.csv` and `node_2-rel.csv`,
and some debugging messages will be stored in the files `node_1-out.txt` and
`node_2-out.txt`.
