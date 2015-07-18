# infix_search

This program will construct a prefix trie where unique suffixes are not stored as edges.  For example, assume we have two dictionary entries

```
MSP1030200ABD
MSP1030201ADF
```

Exact key matches will recover unique IDs. However, keys MSP and P103 will recover both entries. However, since the branch point is after MSP103020, a key of ABD will not recover any entries.

To compile:

```
g++ -o search search.cpp
```

To run:

edit the run.sh file to point to a dictionary file and a query file. Then execute run.sh with no arguments.

The plan is to actively optimize this algorithm, test this, benchmark performance, and port to Java, and harness this library for the Pricing API project.


