#!/bin/bash

#dict='Part_number_Database.sorted'
dict='small.txt'
query='small.query'
#query='query.txt'
#algo='linear'
algo='trie'

./search --algorithm $algo --dict $dict --query $query
