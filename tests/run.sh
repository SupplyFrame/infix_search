#!/bin/bash

#dict='Part_number_Database.sorted'
dict='parts_lookup.small'
query='parts_lookup.query'
#dict='dict.txt'
#query='query.txt'
#dict='small.txt'
#query='small.query'
#dict='all_parts'
#dict='parts_lookup.small'
#query='jiao.query'
#algo='linear'
algo='trie'

../bin/search --algorithm $algo --dict $dict --query $query
