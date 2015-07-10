#!/bin/bash

#dict='Part_number_Database.sorted'
dict='dict.txt'
query='query.txt'
#algo='linear'
algo='trie'

./search --algorithm $algo --dict $dict --query $query
