# sadDB

A super awesome distributed database system, that implements the distributed
track-join.


## Prepare

To prepare the build environment, run the following commands once:

```bash
cd build
cmake ..
```

## Build

To build the project, run:

```bash
cd build
make
```


## Run

To start the server, run:

```bash
cd build
./sadDB
```

And to start the client, execute:

```bash
cd build
./sadDB_client
```


# Test

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
