#!/bin/bash

#dict='Part_number_Database.sorted'
dict='dict.txt'
query='query.txt'
#dict='small.txt'
#query='small.query'
#algo='linear'
algo='trie'

./search --algorithm $algo --dict $dict --query $query
